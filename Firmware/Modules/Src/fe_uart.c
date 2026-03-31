/*
 * fe_uart.c
 *
 *  Created on: Feb 26, 2026
 *      Author: Muazam
 */

// Half-duplex UART (single-wire) driver using TX_EN + UART DMA TX/RX
// Assumptions:
//  - TX is allowed only when TX_EN=1
//  - RX is allowed only when TX_EN=0
//  - Default TX_EN=0
//  - For DMA safety, TX data is copied into a persistent buffer inside fe_uart_t

#include "../../Modules/Inc/fe_uart.h"

#include "stm32f1xx_hal.h"
#include <string.h>

void fe_uart_init(fe_uart_t *u, fe_port_t port)
{
    u->port  = port;
    u->state = FE_UART_IDLE;

    u->tx_len = 0;

    u->rx_buf = 0;
    u->rx_len = 0;

    // Default: RX mode
    fe_platform_txen_set(&u->port, 0);
}

int fe_uart_send_dma(fe_uart_t *u, const uint8_t *data, uint16_t len)
{
    if (u->state != FE_UART_IDLE) return -1;
    if (len == 0) return -4;
    if (len > FE_UART_TXBUF_MAX) return -3;

    // Stop RX DMA (single-wire bus)
    fe_platform_uart_abort_rx(&u->port);

    // Copy into persistent buffer so DMA doesn't read dead stack memory
    memcpy(u->tx_buf, data, len);
    u->tx_len = len;

    // Enter TX mode
    fe_platform_txen_set(&u->port, 1);

    u->state = FE_UART_TX_DMA;

    // Start DMA TX using persistent buffer
    int st = fe_platform_uart_tx_dma(&u->port, u->tx_buf, u->tx_len);

    // Recovery: if HAL reports BUSY, abort and retry once.
    if (st == -(int)HAL_BUSY) {
        UART_HandleTypeDef *huart = (UART_HandleTypeDef*)u->port.huart;
        (void)HAL_UART_AbortTransmit(huart);
        st = fe_platform_uart_tx_dma(&u->port, u->tx_buf, u->tx_len);
    }

    if (st != 0) {
        fe_platform_txen_set(&u->port, 0);
        u->state = FE_UART_IDLE;
        return st; // negative HAL status (e.g. -HAL_BUSY, -HAL_ERROR)
    }

    return 0;
}

void fe_uart_on_tx_dma_complete(fe_uart_t *u)
{
    // DMA finished feeding UART; now wait for TC using interrupt.
    u->state = FE_UART_WAIT_TC;
    fe_platform_uart_enable_tc_it(&u->port);
}

void fe_uart_on_tc_irq(fe_uart_t *u)
{
    // Disable TC interrupt (one-shot)
    fe_platform_uart_disable_tc_it(&u->port);

    // Return to RX mode
    fe_platform_txen_set(&u->port, 0);

    u->state = FE_UART_IDLE;
}

int fe_uart_start_rx_dma(fe_uart_t *u, uint8_t *buf, uint16_t len)
{
    if (u->state != FE_UART_IDLE) return -1;
    if (buf == 0 || len == 0) return -4;

    // RX is only valid in RX mode (TX_EN should be low in IDLE)
    u->rx_buf = buf;
    u->rx_len = len;

    u->state = FE_UART_RX_DMA;

    if (fe_platform_uart_rx_dma(&u->port, buf, len) != 0) {
        u->state = FE_UART_IDLE;
        return -2;
    }

    return 0;
}

void fe_uart_on_rx_dma_complete(fe_uart_t *u)
{
    u->state = FE_UART_IDLE;
}
