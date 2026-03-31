/*
 * fe_platform.h
 *
 *  Created on: Feb 26, 2026
 *      Author: Lenovo
 */

#ifndef INC_FE_PLATFORM_H_
#define INC_FE_PLATFORM_H_

#pragma once
#include <stdint.h>

typedef struct {
    void *huart;          // UART_HandleTypeDef*
    uint32_t txen_port;   // GPIO_TypeDef* cast to uint32_t
    uint16_t txen_pin;
} fe_port_t;

void fe_platform_txen_set(const fe_port_t *p, uint8_t level);

int  fe_platform_uart_tx_dma(const fe_port_t *p, const uint8_t *data, uint16_t len);
int  fe_platform_uart_rx_dma(const fe_port_t *p, uint8_t *data, uint16_t len);

void fe_platform_uart_enable_tc_it(const fe_port_t *p);
void fe_platform_uart_disable_tc_it(const fe_port_t *p);
uint8_t fe_platform_uart_is_tc(const fe_port_t *p);
void fe_platform_uart_clear_tc(const fe_port_t *p);

void fe_platform_uart_abort_rx(const fe_port_t *p);


#endif /* INC_FE_PLATFORM_H_ */
