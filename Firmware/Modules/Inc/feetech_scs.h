/*
 * feetech_scs.h
 *
 *  Created on: Feb 26, 2026
 *      Author: Lenovo
 */

#ifndef INC_FEETECH_SCS_H_
#define INC_FEETECH_SCS_H_

#pragma once
#include <stdint.h>
#include "../../Modules/Inc/fe_uart.h"

// ===== Protocol constants (common Feetech/SCServo style) =====
#define SCS_HDR              0xFF
#define SCS_CMD_PING         0x01
#define SCS_CMD_READ         0x02
#define SCS_CMD_WRITE        0x03

// Typical goal position register for SCS/SCServo families (2 bytes, L then H)
#define SCS_REG_GOAL_POS_L   0x2A

// Typical position scale (often 0..1023 for 0..240 deg)
#define SCS_REG_PRESENT_POS_L   0x38
#define SCS_POS_MAX             4095u
#define SCS_ANGLE_RANGE_DEG     300.0f
// ===== Types =====
typedef struct {
    fe_uart_t *bus;

    // RX buffer used by this module (small, fixed)
    uint8_t rx_buf[16];
    volatile uint32_t rx_done_count;
} scs_t;

// ===== API =====
void scs_init(scs_t *s, fe_uart_t *bus);

// Low-level
int  scs_ping(scs_t *s, uint8_t id, uint8_t *out_error, uint32_t timeout_ms);
int  scs_read(scs_t *s, uint8_t id, uint8_t reg, uint8_t nbytes, uint8_t *out, uint8_t *out_error, uint32_t timeout_ms);
int  scs_write(scs_t *s, uint8_t id, uint8_t reg, const uint8_t *data, uint8_t nbytes, uint8_t *out_error, uint32_t timeout_ms);

// Convenience motion
uint16_t scs_deg_to_pos(float deg);
int  scs_set_goal_position_raw(scs_t *s, uint8_t id, uint16_t pos, uint32_t timeout_ms);
int  scs_set_goal_position_deg(scs_t *s, uint8_t id, float deg, uint32_t timeout_ms);

// Scan helper (calls ping repeatedly)
typedef void (*scs_found_cb_t)(uint8_t id, uint8_t err, void *user);
void scs_scan_ids(scs_t *s, uint8_t start_id, uint8_t end_id, uint32_t per_id_timeout_ms,
                  scs_found_cb_t cb, void *user);

// Hook from HAL UART RX complete callback (huart2 -> scs_on_rx_cplt)
void scs_on_rx_cplt_isr(scs_t *s);
#endif /* INC_FEETECH_SCS_H_ */
