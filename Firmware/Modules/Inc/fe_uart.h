/*
 * fe_uart.h
 *
 *  Created on: Feb 26, 2026
 *      Author: Muazam
 */

#ifndef INC_FE_UART_H_
#define INC_FE_UART_H_

#pragma once
#include <stdint.h>
#include "../../Platform/Inc/fe_platform.h"

#define FE_UART_TXBUF_MAX  128   // enough for Feetech packets

typedef enum {
    FE_UART_IDLE = 0,
    FE_UART_TX_DMA,
    FE_UART_WAIT_TC,
    FE_UART_RX_DMA
} fe_uart_state_t;

typedef struct {
    fe_port_t port;
    volatile fe_uart_state_t state;

    // Persistent TX buffer for DMA (IMPORTANT)
    uint8_t  tx_buf[FE_UART_TXBUF_MAX];
    uint16_t tx_len;

    // optional RX
    uint8_t *rx_buf;
    uint16_t rx_len;
} fe_uart_t;

void fe_uart_init(fe_uart_t *u, fe_port_t port);

// Copies data into u->tx_buf then starts DMA
int  fe_uart_send_dma(fe_uart_t *u, const uint8_t *data, uint16_t len);

int  fe_uart_start_rx_dma(fe_uart_t *u, uint8_t *buf, uint16_t len);

void fe_uart_on_tx_dma_complete(fe_uart_t *u);
void fe_uart_on_tc_irq(fe_uart_t *u);
void fe_uart_on_rx_dma_complete(fe_uart_t *u);

#endif /* INC_FE_UART_H_ */
