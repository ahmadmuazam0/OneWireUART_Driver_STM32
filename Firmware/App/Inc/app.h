/*
 * app.h
 *
 *  Created on: Mar 5, 2026
 *      Author: Muazam
 */

#ifndef APP_H_
#define APP_H_


#include "stm32f1xx_hal.h"

void app_init(void);
void app_loop(void);

void app_on_uart_rx_event(UART_HandleTypeDef *huart, uint16_t size);
void app_on_uart_tx_cplt(UART_HandleTypeDef *huart);
void app_on_uart_error(UART_HandleTypeDef *huart);
void app_on_usart2_tc_irq(void);


#endif /* APP_H_ */
