/*
 * app.c
 *
 *  Created on: Mar 5, 2026
 *      Author: Muazam
 *
 * Raw UART bridge with 1-wire TX_EN on UART2
 *
 * UART1 <-> UART2
 *
 * UART1:
 *   normal RX/TX DMA bridge side
 *
 * UART2:
 *   1-wire Feetech side
 *   PA8 = TX_EN
 *
 * Behavior:
 *   UART1 RX chunk  -> UART2 TX DMA
 *   UART2 RX chunk  -> UART1 TX DMA
 *
 * Important:
 *   For UART2 TX, PA8 is driven HIGH before transmit.
 *   PA8 goes LOW only after the real USART2 TC interrupt.
 */

#include "app.h"
#include <string.h>
#include "usart.h"
#include "gpio.h"

#define BRIDGE_BUF_SIZE 256

#define TXEN_PORT GPIOA
#define TXEN_PIN  GPIO_PIN_8

static uint8_t u1_rx_buf[BRIDGE_BUF_SIZE];
static uint8_t u2_rx_buf[BRIDGE_BUF_SIZE];

static volatile uint8_t u1_tx_busy = 0;
static volatile uint8_t u2_tx_busy = 0;
static volatile uint8_t u2_wait_tc = 0;

static void disable_dma_ht(UART_HandleTypeDef *huart)
{
    if (huart->hdmarx != NULL) {
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    if (huart->hdmatx != NULL) {
        __HAL_DMA_DISABLE_IT(huart->hdmatx, DMA_IT_HT);
    }
}

static void start_u1_rx(void)
{
    (void)HAL_UARTEx_ReceiveToIdle_DMA(&huart1, u1_rx_buf, BRIDGE_BUF_SIZE);
    disable_dma_ht(&huart1);
}

static void start_u2_rx(void)
{
    (void)HAL_UARTEx_ReceiveToIdle_DMA(&huart2, u2_rx_buf, BRIDGE_BUF_SIZE);
    disable_dma_ht(&huart2);
}

static void stop_u2_rx(void)
{
    (void)HAL_UART_AbortReceive(&huart2);
}

static void start_u2_tx(uint8_t *data, uint16_t size)
{
    if (size == 0) {
        return;
    }

    if (u2_tx_busy) {
        return;
    }

    u2_tx_busy = 1;
    u2_wait_tc = 0;

    /* Stop UART2 RX before driving the 1-wire bus */
    stop_u2_rx();

    /* Enable transmitter on the 1-wire direction pin */
    HAL_GPIO_WritePin(TXEN_PORT, TXEN_PIN, GPIO_PIN_SET);

    /* Clear TC before starting a new frame */
    __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_TC);

    if (HAL_UART_Transmit_DMA(&huart2, data, size) != HAL_OK) {
        HAL_GPIO_WritePin(TXEN_PORT, TXEN_PIN, GPIO_PIN_RESET);
        u2_tx_busy = 0;
        start_u2_rx();
    } else {
        disable_dma_ht(&huart2);
    }
}

static void start_u1_tx(uint8_t *data, uint16_t size)
{
    if (size == 0) {
        return;
    }

    if (u1_tx_busy) {
        return;
    }

    u1_tx_busy = 1;

    if (HAL_UART_Transmit_DMA(&huart1, data, size) != HAL_OK) {
        u1_tx_busy = 0;
    } else {
        disable_dma_ht(&huart1);
    }
}

void app_init(void)
{
    u1_tx_busy = 0;
    u2_tx_busy = 0;
    u2_wait_tc = 0;

    HAL_GPIO_WritePin(TXEN_PORT, TXEN_PIN, GPIO_PIN_RESET);

    start_u1_rx();
    start_u2_rx();
}

void app_loop(void)
{
    /* Nothing needed */
}

void app_on_uart_rx_event(UART_HandleTypeDef *huart, uint16_t size)
{
    if (size == 0) {
        return;
    }

    if (huart == &huart1) {
        /* Forward host data to 1-wire bus */
        if (!u2_tx_busy) {
            start_u2_tx(u1_rx_buf, size);
        }
        start_u1_rx();
        return;
    }

    if (huart == &huart2) {
        /* Forward motor/bus reply back to host */
        if (!u1_tx_busy) {
            start_u1_tx(u2_rx_buf, size);
        }
        start_u2_rx();
        return;
    }
}

void app_on_uart_tx_cplt(UART_HandleTypeDef *huart)
{
    if (huart == &huart1) {
        u1_tx_busy = 0;
        return;
    }

    if (huart == &huart2) {
        /*
         * DMA complete is NOT enough for 1-wire direction switch.
         * Wait for real TC interrupt.
         */
        u2_wait_tc = 1;
        __HAL_UART_ENABLE_IT(&huart2, UART_IT_TC);
        return;
    }
}

void app_on_uart_error(UART_HandleTypeDef *huart)
{
    if (huart == &huart1) {
        u1_tx_busy = 0;
        (void)HAL_UART_Abort(&huart1);
        start_u1_rx();
        return;
    }

    if (huart == &huart2) {
        u2_tx_busy = 0;
        u2_wait_tc = 0;

        (void)HAL_UART_Abort(&huart2);
        __HAL_UART_DISABLE_IT(&huart2, UART_IT_TC);
        __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_TC);

        HAL_GPIO_WritePin(TXEN_PORT, TXEN_PIN, GPIO_PIN_RESET);
        start_u2_rx();
        return;
    }
}

void app_on_usart2_tc_irq(void)
{
    if (!u2_wait_tc) {
        return;
    }

    u2_wait_tc = 0;
    u2_tx_busy = 0;

    __HAL_UART_DISABLE_IT(&huart2, UART_IT_TC);
    __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_TC);

    /* Back to receive mode on 1-wire bus */
    HAL_GPIO_WritePin(TXEN_PORT, TXEN_PIN, GPIO_PIN_RESET);

    start_u2_rx();
}
