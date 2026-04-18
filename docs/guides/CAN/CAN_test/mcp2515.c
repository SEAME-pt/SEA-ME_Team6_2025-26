#include "main.h"
#include "mcp2515.h"
#include "tx_api.h"

// External SPI handle (from CubeMX generated code)
extern SPI_HandleTypeDef hspi1;

// Chip select macros (define according to your GPIO)
#ifndef MCP_CS_L
#define MCP_CS_L() HAL_GPIO_WritePin(CS_PIN_GPIO_Port, CS_PIN_Pin, GPIO_PIN_RESET)
#endif
#ifndef MCP_CS_H
#define MCP_CS_H() HAL_GPIO_WritePin(CS_PIN_GPIO_Port, CS_PIN_Pin, GPIO_PIN_SET)
#endif



// ----------------------------------------
// Send MCP2515 Reset Command
// ----------------------------------------
void MCP2515_Reset(void)
{
    uint8_t cmd = MCP_RESET;
    MCP_CS_L();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    MCP_CS_H();

    //HAL_Delay(10);  // Give some time to reset
    tx_thread_sleep(1);
}

// ----------------------------------------
// Read a single register
// ----------------------------------------
uint8_t MCP2515_ReadRegister(uint8_t reg)
{
    uint8_t cmd[2];
    uint8_t val;

    cmd[0] = MCP_READ;
    cmd[1] = reg;

    MCP_CS_L();
    HAL_SPI_Transmit(&hspi1, cmd, 2, HAL_MAX_DELAY);
    HAL_SPI_Receive(&hspi1, &val, 1, HAL_MAX_DELAY);
    MCP_CS_H();

    return val;
}

// ----------------------------------------
// Write a single register
// ----------------------------------------
void MCP2515_WriteRegister(uint8_t reg, uint8_t val)
{
    uint8_t buf[3];

    buf[0] = MCP_WRITE;
    buf[1] = reg;
    buf[2] = val;

    MCP_CS_L();
    HAL_SPI_Transmit(&hspi1, buf, 3, HAL_MAX_DELAY);
    MCP_CS_H();
}

// ----------------------------------------
// Modify specific bits in a register
// ----------------------------------------
void MCP2515_BitModify(uint8_t reg, uint8_t mask, uint8_t val)
{
    uint8_t buf[4];
    buf[0] = MCP_BIT_MODIFY;
    buf[1] = reg;
    buf[2] = mask;
    buf[3] = val;

    MCP_CS_L();
    HAL_SPI_Transmit(&hspi1, buf, 4, HAL_MAX_DELAY);
    MCP_CS_H();
}

// ----------------------------------------
// Set MCP2515 to Normal Operation Mode
// ----------------------------------------
void MCP2515_SetNormalMode(void)
{
    // Clear mode bits (REQOP) and set Normal
    MCP2515_BitModify(REG_CANCTRL, CANCTRL_REQOP_MASK, CANCTRL_REQOP_NORMAL);

    // Optional: check that mode changed
    uint8_t stat = MCP2515_ReadRegister(REG_CANSTAT);
    stat &= CANCTRL_REQOP_MASK;
    if (stat != CANCTRL_REQOP_NORMAL)
    {
        // Error handling: could not set normal mode
    }
}

void mcp_init(void)
{
	MCP2515_Reset();
	MCP2515_WriteRegister(REG_CANCTRL, 0x80); // Config mode

    // MCP2515 with 8 MHz crystal → 500 kbps
	MCP2515_WriteRegister(REG_CNF1, 0x00); // BRP = 1  (TQ = 2*1/8MHz = 250ns)
	MCP2515_WriteRegister(REG_CNF2, 0x90); // BTLMODE=1, SAM=0, PHSEG1=4, PRSEG=1
	MCP2515_WriteRegister(REG_CNF3, 0x02); // PHSEG2=6

	MCP2515_WriteRegister(0X60, 0x60); //RXB0CTRL
	MCP2515_WriteRegister(0X70, 0x60); //RXB0CTRL

	//Mask = 0 (accept all)
	MCP2515_WriteRegister(0X20, 0x00);
	MCP2515_WriteRegister(0X21, 0x00);
	MCP2515_WriteRegister(0X24, 0x00);
	MCP2515_WriteRegister(0X25, 0x00);

	//Clear interrupt flags
	MCP2515_WriteRegister(0X2C, 0x00);

	//Enable RX0IE + RX1IE
	MCP2515_WriteRegister(0x2B, 0x03);

	MCP2515_WriteRegister(REG_CANCTRL, 0x00); // Normal mode
}
