#pragma once
#include <stdint.h>
#include <stddef.h>
#include "mcp2515.h"
#include "main.h"

/**
 * Send a CAN message through MCP2515.
 * @param can_id Standard 11-bit ID
 * @param data   Payload pointer
 * @param len    Payload length (0..8)
 * @return 0 on success, non-zero on error
 */
int mcp_send_message(uint16_t can_id, const uint8_t* data, size_t len);
