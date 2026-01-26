#include "main.h"
#include "mcp2515.h"
#include "tx_api.h"
#include <stdio.h>

// External mutex for printf protection
extern TX_MUTEX printf_mutex;

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
	uint8_t test_val;

	tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
	printf("[MCP2515] Iniciando...\r\n");
	tx_mutex_put(&printf_mutex);

	// Reset MCP2515
	MCP2515_Reset();
	tx_thread_sleep(10);  // Wait after reset

	// Test SPI communication - read CANSTAT after reset
	test_val = MCP2515_ReadRegister(REG_CANSTAT);
	tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
	printf("[MCP2515] CANSTAT após reset: 0x%02X (esperado: 0x80)\r\n", test_val);
	tx_mutex_put(&printf_mutex);

	// Enter config mode
	MCP2515_WriteRegister(REG_CANCTRL, 0x80); // Config mode
	tx_thread_sleep(5);

	// Verify config mode
	test_val = MCP2515_ReadRegister(REG_CANCTRL);
	tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
	printf("[MCP2515] CANCTRL após config: 0x%02X (esperado: 0x87)\r\n", test_val);
	tx_mutex_put(&printf_mutex);

    // MCP2515 with 8 MHz crystal → 500 kbps
	MCP2515_WriteRegister(REG_CNF1, 0x00); // BRP = 1  (TQ = 2*1/8MHz = 250ns)
	MCP2515_WriteRegister(REG_CNF2, 0x90); // BTLMODE=1, SAM=0, PHSEG1=4, PRSEG=1
	MCP2515_WriteRegister(REG_CNF3, 0x02); // PHSEG2=6

	tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
	printf("[MCP2515] Bitrate 500 kbps configurado\r\n");
	tx_mutex_put(&printf_mutex);

	MCP2515_WriteRegister(0X60, 0x60); //RXB0CTRL
	MCP2515_WriteRegister(0X70, 0x60); //RXB1CTRL

	//Mask = 0 (accept all)
	MCP2515_WriteRegister(0X20, 0x00);
	MCP2515_WriteRegister(0X21, 0x00);
	MCP2515_WriteRegister(0X24, 0x00);
	MCP2515_WriteRegister(0X25, 0x00);

	//Clear interrupt flags
	MCP2515_WriteRegister(0X2C, 0x00);

	//Enable RX0IE + RX1IE
	MCP2515_WriteRegister(0x2B, 0x03);

	// Enter normal mode
	MCP2515_WriteRegister(REG_CANCTRL, 0x00); // Normal mode
	tx_thread_sleep(5);

	// Verify normal mode
	test_val = MCP2515_ReadRegister(REG_CANCTRL);
	tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
	printf("[MCP2515] CANCTRL em normal mode: 0x%02X (esperado: 0x00 ou 0x07)\r\n", test_val);
	tx_mutex_put(&printf_mutex);

	// Read CANSTAT to check mode
	test_val = MCP2515_ReadRegister(REG_CANSTAT);
	tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
	printf("[MCP2515] CANSTAT em normal mode: 0x%02X\r\n", test_val);
	if ((test_val & 0xE0) == 0x00)
	{
		printf("[MCP2515] ✓ Inicialização OK - Modo Normal Ativo!\r\n");
	}
	else
	{
		printf("[MCP2515] ✗ ERRO: MCP2515 não entrou em modo normal!\r\n");
	}
	tx_mutex_put(&printf_mutex);
}

// ----------------------------------------
// Check if a CAN message is available
// ----------------------------------------
uint8_t MCP2515_CheckReceive(void)
{
    uint8_t status = MCP2515_ReadRegister(REG_CANINTF);
    return (status & 0x03);  // Check RX0IF (bit 0) or RX1IF (bit 1)
}

// ----------------------------------------
// Read a CAN message from RX buffer
// ----------------------------------------
uint8_t MCP2515_ReadMessage(CAN_Message_t *msg)
{
    uint8_t status = MCP2515_ReadRegister(REG_CANINTF);
    uint8_t buf[13];  // Buffer to hold entire RX message
    uint8_t cmd;

    if (status & 0x01)  // RX0IF set (message in RXB0)
    {
        cmd = MCP_READ_RX;  // Read RX buffer 0 command (0x90)
    }
    else if (status & 0x02)  // RX1IF set (message in RXB1)
    {
        cmd = 0x94;  // Read RX buffer 1 command
    }
    else
    {
        return 0;  // No message available
    }

    // Read entire RX buffer via SPI
    MCP_CS_L();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(&hspi1, buf, 13, HAL_MAX_DELAY);
    MCP_CS_H();

    // Parse message
    // buf[0] = SIDH, buf[1] = SIDL, buf[2] = EID8, buf[3] = EID0, buf[4] = DLC

    msg->id = ((uint16_t)buf[0] << 3) | (buf[1] >> 5);
    msg->dlc = buf[4] & 0x0F;

    // Copy data bytes
    for (uint8_t i = 0; i < msg->dlc && i < 8; i++)
    {
        msg->data[i] = buf[5 + i];
    }

    // Clear interrupt flags - limpar RX0IF/RX1IF e também flags de erro
    MCP2515_BitModify(REG_CANINTF, (status & 0x03), 0x00);

    // Limpar também flags de erro (ERRIF bit 5, MERRF bit 7) se estiverem ativas
    uint8_t error_flags = status & 0xA0;  // Bits 5 e 7
    if (error_flags)
    {
        MCP2515_BitModify(REG_CANINTF, error_flags, 0x00);
    }

    return 1;  // Message received successfully
}
