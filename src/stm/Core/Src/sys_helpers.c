#include "sys_helpers.h"

#include <stdio.h>
#include <stdarg.h>
#include "tx_api.h"

/* ============================================================
 * Logging
 * ============================================================ */

#include <stdarg.h>
#include <stdio.h>

void sys_log(SystemCtx* ctx, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    /* Try-lock instead of forever */
    if (tx_mutex_get(&ctx->printf_mutex, TX_NO_WAIT) != TX_SUCCESS) {
        va_end(ap);
        return; // drop log if mutex is busy
    }

    vprintf(fmt, ap);
    printf("\r\n");

    tx_mutex_put(&ctx->printf_mutex);
    va_end(ap);
}

/* ============================================================
 * CAN packing helpers
 * ============================================================ */

void put_u8(uint8_t *dst, uint8_t v)
{
    dst[0] = v;
}

void put_i16_le(uint8_t *dst, int16_t v) // little-endian storage
{
    uint16_t uv = (uint16_t)v;        // reinterpret bits
    dst[0] = (uint8_t)(uv & 0xFF);    // LSB
    dst[1] = (uint8_t)(uv >> 8);      // MSB
}

void put_u16_le(uint8_t *dst, uint16_t v)
{
  dst[0] = (uint8_t)(v & 0xFF);
  dst[1] = (uint8_t)((v >> 8) & 0xFF);
}

/* ============================================================
 * CRC-8 (polynomial 0x07, initial 0x00)
 * ============================================================ */

 /**
  * @brief  Calculate CRC-8 (polynomial 0x07, init 0x00)
  * @param  data: pointer to data buffer
  * @param  len: data length
  * @retval CRC-8 value
  */
uint8_t calculate_crc8(const uint8_t* data, size_t len)
{
    uint8_t crc = 0x00;

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t b = 0; b < 8; b++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }

    return crc;
}

/**
  * @brief  Validate CRC-8 of received frame
  * @param  data: pointer to frame data (including CRC byte at end)
  * @param  len: total frame length (including CRC byte)
  * @retval 1 if valid, 0 if invalid
  */
uint8_t validate_crc8(const uint8_t* data, size_t len_with_crc)
{
    if (len_with_crc < 1)
        return 0;

    uint8_t expected = data[len_with_crc - 1];
    uint8_t computed = calculate_crc8(data, len_with_crc - 1);

    return (expected == computed);
}