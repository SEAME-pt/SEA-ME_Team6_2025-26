#include "can_tx.h"
#include "system_ctx.h"

extern SPI_HandleTypeDef hspi1;

/**
  * @brief  Send CAN message via MCP2515
  * @param  id: CAN message ID
  * @param  data: pointer to data buffer
  * @param  len: data length (0-8 bytes)
  * @retval None
  */
void mcp_send_message(uint16_t id, uint8_t *data, uint8_t len)
{
    uint8_t buf[6 + 8];  // 5 header + DLC + up to 8 data bytes

    buf[0] = MCP_LOAD_TX;                 // 0x40 command
    buf[1] = (id >> 3) & 0xFF;            // SIDH
    buf[2] = (id & 0x07) << 5;            // SIDL
    buf[3] = 0x00;                        // EID8 (not used)
    buf[4] = 0x00;                        // EID0 (not used)
    buf[5] = len & 0x0F;                  // DLC (0–8)

    for (uint8_t i = 0; i < len; i++)
        buf[6 + i] = data[i];

    TX_MUTEX *spi_mtx = &system_ctx()->spi1_mutex;
    tx_mutex_get(spi_mtx, TX_WAIT_FOREVER);

    MCP_CS_L();
    HAL_SPI_Transmit(&hspi1, buf, 6 + len, HAL_MAX_DELAY);
    MCP_CS_H();

    uint8_t cmd = MCP_RTS_TXB0;
    MCP_CS_L();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    MCP_CS_H();

    tx_mutex_put(spi_mtx);
}
