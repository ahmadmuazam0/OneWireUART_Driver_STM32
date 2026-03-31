/*
 * app_callbacks.c
 *
 *  Created on: Mar 5, 2026
 *      Author: Muazam
 *
 *
 * HAL callback bridge to application layer
 */


#include "app.h"

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    app_on_uart_rx_event(huart, Size);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    app_on_uart_tx_cplt(huart);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    app_on_uart_error(huart);
}
