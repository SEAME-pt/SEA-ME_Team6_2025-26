#pragma once

#include <stdint.h>
#include <stddef.h>
#include "system_ctx.h"

/* ---------- Logging ---------- */

/**
 * Thread-safe printf using the SystemCtx printf mutex.
 * Usage: sys_log(ctx, "[TAG] value=%d\n", v);
 */
void sys_log(SystemCtx* ctx, const char* fmt, ...);

/* ---------- CAN packing helpers ---------- */

void put_u8(uint8_t* buf, size_t* idx, uint8_t v);
void put_i16_le(uint8_t* buf, size_t* idx, int16_t v);
void put_u16_le(uint8_t* buf, size_t* idx, uint16_t v);

/* ---------- CRC-8 helpers ---------- */

uint8_t calculate_crc8(const uint8_t* data, size_t len);
uint8_t validate_crc8(const uint8_t* data, size_t len_with_crc);
