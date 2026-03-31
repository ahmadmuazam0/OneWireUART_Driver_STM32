/*
 * fe_platform_stm32_hal.c
 *
 *  Created on: Feb 26, 2026
 *      Author: Muazam
 */

#include "stm32f1xx_hal.h"
#include "../../Platform/Inc/fe_platform.h"

static GPIO_TypeDef* G(uint32_t opaque) { return (GPIO_TypeDef*)opaque; }
static UART_HandleTypeDef* U(const fe_port_t *p) { return (UART_HandleTypeDef*)p->huart; }

void fe_platform_txen_set(const fe_port_t *p, uint8_t level)
{
    HAL_GPIO_WritePin(G(p->txen_port), p->txen_pin, level ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

int fe_platform_uart_tx_dma(const fe_port_t *p, const uint8_t *data, uint16_t len)
{
    UART_HandleTypeDef *huart = (UART_HandleTypeDef*)p->huart;
    HAL_StatusTypeDef st = HAL_UART_Transmit_DMA(huart, (uint8_t*)data, len);
    // Return 0 on OK, otherwise return negative HAL status (BUSY=2, ERROR=1, TIMEOUT=3)
    return (st == HAL_OK) ? 0 : -(int)st;
}

int fe_platform_uart_rx_dma(const fe_port_t *p, uint8_t *data, uint16_t len)
{
    if (HAL_UART_Receive_DMA(U(p), data, len) == HAL_OK) return 0;
    return -1;
}

void fe_platform_uart_enable_tc_it(const fe_port_t *p)
{
    __HAL_UART_ENABLE_IT(U(p), UART_IT_TC);
}

void fe_platform_uart_disable_tc_it(const fe_port_t *p)
{
    __HAL_UART_DISABLE_IT(U(p), UART_IT_TC);
}

uint8_t fe_platform_uart_is_tc(const fe_port_t *p)
{
    return (__HAL_UART_GET_FLAG(U(p), UART_FLAG_TC) != RESET) ? 1 : 0;
}

void fe_platform_uart_clear_tc(const fe_port_t *p)
{
    (void)p;
    /* STM32F1 note:
     * TC is cleared by writing to the data register (DR) (see RM0008),
     * not by writing to a flag-clear register. We keep this as a no-op.
     * Safe for our one-shot TC interrupt usage.
     */
}

void fe_platform_uart_abort_rx(const fe_port_t *p)
{
    (void)HAL_UART_AbortReceive(U(p));
}
