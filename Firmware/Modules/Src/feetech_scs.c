/*
 * feetech_scs.c
 *
 *  Created on: Feb 26, 2026
 *      Author: Muazam
 */

#include "../../Modules/Inc/feetech_scs.h"

#include "stm32f1xx_hal.h"   // for HAL_GetTick / HAL_Delay (portable enough; swap later if needed)
#include <string.h>

// ---------- internal helpers ----------
static uint8_t scs_checksum_bytes(const uint8_t *pkt, uint8_t total_len)
{
    // checksum: ~(sum of bytes from ID to last param) & 0xFF
    uint16_t sum = 0;
    for (uint8_t i = 2; i < (uint8_t)(total_len - 1); i++) sum += pkt[i];
    return (uint8_t)(~(sum & 0xFF));
}

static int wait_bus_idle(fe_uart_t *bus, uint32_t timeout_ms)
{
    uint32_t t0 = HAL_GetTick();
    while (bus->state != FE_UART_IDLE) {
        if ((HAL_GetTick() - t0) > timeout_ms) return -100; // TX never completed
    }
    return 0;
}

static int wait_rx_done(scs_t *s, uint32_t prev_cnt, uint32_t timeout_ms)
{
    uint32_t t0 = HAL_GetTick();
    while (s->rx_done_count == prev_cnt) {
        if ((HAL_GetTick() - t0) > timeout_ms) return -200; // RX timeout
    }
    return 0;
}

// reply minimal: FF FF ID LEN ERR ... CHK
static int validate_reply_min(const uint8_t *r, uint8_t rlen, uint8_t expect_id)
{
    if (rlen < 6) return 0;
    if (r[0] != SCS_HDR || r[1] != SCS_HDR) return 0;
    if (r[2] != expect_id) return 0;
    // checksum validate
    uint8_t chk = r[rlen - 1];
    uint8_t calc = scs_checksum_bytes(r, rlen);
    return (chk == calc) ? 1 : 0;
}

// ---------- public ----------
void scs_init(scs_t *s, fe_uart_t *bus)
{
    s->bus = bus;
    s->rx_done_count = 0;
    memset(s->rx_buf, 0, sizeof(s->rx_buf));
}

void scs_on_rx_cplt_isr(scs_t *s)
{
    // call from HAL_UART_RxCpltCallback for huart2
    s->rx_done_count++;
}

int scs_ping(scs_t *s, uint8_t id, uint8_t *out_error, uint32_t timeout_ms)
{
    // FF FF ID 02 01 CHK
    uint8_t pkt[6];
    pkt[0] = 0xFF;
    pkt[1] = 0xFF;
    pkt[2] = id;
    pkt[3] = 0x02;
    pkt[4] = SCS_CMD_PING;
    pkt[5] = scs_checksum_bytes(pkt, 6);

    // abort any prior RX (safe)
    (void)HAL_UART_AbortReceive((UART_HandleTypeDef*)s->bus->port.huart);

    uint32_t rx_mark = s->rx_done_count;

    int r = fe_uart_send_dma(s->bus, pkt, sizeof(pkt));
    if (r != 0) return r;

    r = wait_bus_idle(s->bus, timeout_ms);
    if (r != 0) return r;

    // Expect 6 bytes reply for ping: FF FF ID 02 ERR CHK
    r = fe_uart_start_rx_dma(s->bus, s->rx_buf, 6);
    if (r != 0) return r;

    r = wait_rx_done(s, rx_mark, timeout_ms);
    if (r != 0) {
        (void)HAL_UART_AbortReceive((UART_HandleTypeDef*)s->bus->port.huart);
        return 0; // no response
    }

    if (!validate_reply_min(s->rx_buf, 6, id)) return 0;

    if (out_error) *out_error = s->rx_buf[4];
    return 1; // found
}

int scs_read(scs_t *s, uint8_t id, uint8_t reg, uint8_t nbytes,
             uint8_t *out, uint8_t *out_error, uint32_t timeout_ms)
{
    // FF FF ID LEN 02 reg nbytes CHK
    // params=2, LEN=params+2=4, total=2+1+1+1+2+1=8
    uint8_t pkt[8];
    pkt[0] = 0xFF;
    pkt[1] = 0xFF;
    pkt[2] = id;
    pkt[3] = 0x04;
    pkt[4] = SCS_CMD_READ;
    pkt[5] = reg;
    pkt[6] = nbytes;
    pkt[7] = scs_checksum_bytes(pkt, 8);

    (void)HAL_UART_AbortReceive((UART_HandleTypeDef*)s->bus->port.huart);
    uint32_t rx_mark = s->rx_done_count;

    int r = fe_uart_send_dma(s->bus, pkt, sizeof(pkt));
    if (r != 0) return r;

    r = wait_bus_idle(s->bus, timeout_ms);
    if (r != 0) return r;

    // Reply length = 2(hdr)+id+len+err + nbytes + chk = nbytes + 6
    uint8_t rlen = (uint8_t)(nbytes + 6);
    if (rlen > sizeof(s->rx_buf)) return -300;

    r = fe_uart_start_rx_dma(s->bus, s->rx_buf, rlen);
    if (r != 0) return r;

    r = wait_rx_done(s, rx_mark, timeout_ms);
    if (r != 0) {
        (void)HAL_UART_AbortReceive((UART_HandleTypeDef*)s->bus->port.huart);
        return 0;
    }

    if (!validate_reply_min(s->rx_buf, rlen, id)) return 0;

    if (out_error) *out_error = s->rx_buf[4];
    if (out) memcpy(out, &s->rx_buf[5], nbytes);
    return 1;
}

int scs_write(scs_t *s, uint8_t id, uint8_t reg, const uint8_t *data, uint8_t nbytes,
              uint8_t *out_error, uint32_t timeout_ms)
{
    // FF FF ID LEN 03 reg data... CHK
    uint8_t pkt[32];
    uint8_t params = (uint8_t)(1 + nbytes);
    uint8_t len_field = (uint8_t)(params + 2);
    uint8_t total = (uint8_t)(2 + 1 + 1 + 1 + params + 1);

    if (total > sizeof(pkt)) return -301;

    uint8_t i = 0;
    pkt[i++] = 0xFF;
    pkt[i++] = 0xFF;
    pkt[i++] = id;
    pkt[i++] = len_field;
    pkt[i++] = SCS_CMD_WRITE;
    pkt[i++] = reg;
    for (uint8_t k = 0; k < nbytes; k++) pkt[i++] = data[k];
    pkt[i++] = scs_checksum_bytes(pkt, total);

    (void)HAL_UART_AbortReceive((UART_HandleTypeDef*)s->bus->port.huart);
    uint32_t rx_mark = s->rx_done_count;

    int r = fe_uart_send_dma(s->bus, pkt, total);
    if (r != 0) return r;

    r = wait_bus_idle(s->bus, timeout_ms);
    if (r != 0) return r;

    // Many servos reply with status packet to WRITE: FF FF ID 02 ERR CHK
    r = fe_uart_start_rx_dma(s->bus, s->rx_buf, 6);
    if (r != 0) return r;

    r = wait_rx_done(s, rx_mark, timeout_ms);
    if (r != 0) {
        (void)HAL_UART_AbortReceive((UART_HandleTypeDef*)s->bus->port.huart);
        return 0;
    }

    if (!validate_reply_min(s->rx_buf, 6, id)) return 0;
    if (out_error) *out_error = s->rx_buf[4];
    return 1;
}

uint16_t scs_deg_to_pos(float deg)
{
    if (deg < 0.0f) deg = 0.0f;
    if (deg > (float)SCS_ANGLE_RANGE_DEG) deg = (float)SCS_ANGLE_RANGE_DEG;

    float p = (deg * (float)SCS_POS_MAX) / (float)SCS_ANGLE_RANGE_DEG;
    if (p < 0.0f) p = 0.0f;
    if (p > (float)SCS_POS_MAX) p = (float)SCS_POS_MAX;
    return (uint16_t)(p + 0.5f);
}

int scs_set_goal_position_raw(scs_t *s, uint8_t id, uint16_t pos, uint32_t timeout_ms)
{
    if (pos > SCS_POS_MAX) pos = SCS_POS_MAX;
    uint8_t d[2];
    d[0] = (uint8_t)(pos & 0xFF);
    d[1] = (uint8_t)((pos >> 8) & 0xFF);
    return scs_write(s, id, SCS_REG_GOAL_POS_L, d, 2, NULL, timeout_ms);
}

int scs_set_goal_position_deg(scs_t *s, uint8_t id, float deg, uint32_t timeout_ms)
{
    return scs_set_goal_position_raw(s, id, scs_deg_to_pos(deg), timeout_ms);
}

void scs_scan_ids(scs_t *s, uint8_t start_id, uint8_t end_id, uint32_t per_id_timeout_ms,
                  scs_found_cb_t cb, void *user)
{
    for (uint16_t id = start_id; id <= end_id; id++) {
        uint8_t err = 0;
        int ok = scs_ping(s, (uint8_t)id, &err, per_id_timeout_ms);
        if (ok == 1) {
            if (cb) cb((uint8_t)id, err, user);
        }
        HAL_Delay(2);
    }
}
