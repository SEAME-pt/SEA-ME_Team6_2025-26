#ifndef MCP2515_H
#define MCP2515_H

#include "main.h"
#include "stm32u5xx_hal_conf.h"
// -------------------------------
// MCP2515 SPI Commands
// -------------------------------
#define MCP_RESET      0xC0
#define MCP_READ       0x03
#define MCP_READ_RX    0x90  // Read RX buffer
#define MCP_WRITE      0x02
#define MCP_LOAD_TX    0x40  // Load TX buffer
#define MCP_RTS_TXB0   0x81  // Request to send TXB0
#define MCP_RTS_TXB1   0x82  // Request to send TXB1
#define MCP_RTS_TXB2   0x84  // Request to send TXB2
#define MCP_READ_STATUS 0xA0
#define MCP_BIT_MODIFY 0x05

// -------------------------------
// MCP2515 Registers (Commonly Used)
// -------------------------------
#define REG_CANCTRL    0x0F
#define REG_CANSTAT    0x0E
#define REG_CNF1       0x2A
#define REG_CNF2       0x29
#define REG_CNF3       0x28
#define REG_TXB0SIDH   0x31
#define REG_TXB0SIDL   0x32
#define REG_TXB0DLC    0x35
#define REG_TXB0D0     0x36
#define REG_RXB0SIDH   0x61
#define REG_RXB0SIDL   0x62
#define REG_RXB0DLC    0x65
#define REG_RXB0D0     0x66

// -------------------------------
// CANCTRL Register Bits
// -------------------------------
#define CANCTRL_REQOP_MASK 0xE0
#define CANCTRL_REQOP_CONFIG 0x80
#define CANCTRL_REQOP_NORMAL 0x00

// -------------------------------
// CAN Status Bits
// -------------------------------
#define CANSTAT_RX0IF 0x01
#define CANSTAT_RX1IF 0x02

// MCP2515 interrupt flag register
#define REG_CANINTF   0x2C
// MCP2515 error flag register
#define REG_EFLG      0x2D

// -------------------------------
// CAN Message Structure
// -------------------------------
typedef struct {
    uint16_t id;        /* CAN message ID (11-bit standard) */
    uint8_t  dlc;       /* Data length code (0-8) */
    uint8_t  data[8];   /* Data bytes */
} CAN_Message_t;

// -------------------------------
// MCP2515 Initialization
// -------------------------------
void MCP2515_Reset(void);
uint8_t MCP2515_ReadRegister(uint8_t reg);
void MCP2515_WriteRegister(uint8_t reg, uint8_t val);
void MCP2515_BitModify(uint8_t reg, uint8_t mask, uint8_t val);
void MCP2515_SetNormalMode(void);
void mcp_init(void);

// -------------------------------
// CAN Message Reception
// -------------------------------
uint8_t MCP2515_CheckReceive(void);
uint8_t MCP2515_ReadMessage(CAN_Message_t *msg);

// -------------------------------
// SPI Chip Select (to define in main)
// -------------------------------
// Example usage in main code:
// #define MCP_CS_L() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
// #define MCP_CS_H() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)

#endif /* MCP2515_H */
