/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "mcp2515.h"
#include "tx_api.h"
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
extern SPI_HandleTypeDef hspi1;
uint8_t tx_data[8];
uint8_t rx_data[8];
/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
TX_THREAD can_thread;
uint8_t can_thread_stack[1024];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void main_thread_entry(ULONG thread_input);
/* USER CODE END PFP */

UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  ret = tx_thread_create(&can_thread, "Main Thread", app_threadx_entry, 0,
                         can_thread_stack, sizeof(can_thread_stack),
                         1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);


  return ret;
}
/* USER CODE END App_ThreadX_Init */
  /**
  * @brief  Function that implements the kernel's initialization.
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN Before_Kernel_Start */
  /* USER CODE END Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN Kernel_Start_Error */
  /* USER CODE END Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */
void mcp_send_message(uint16_t id, uint8_t *data, uint8_t len) //Function for sending CAN message
{
    uint8_t buf[6 + 8];  // 5 header + DLC + up to 8 data bytes

    buf[0] = MCP_LOAD_TX;                 // 0x40 command
    buf[1] = (id >> 3) & 0xFF;            // SIDH
    buf[2] = (id & 0x07) << 5;            // SIDL
    buf[3] = 0x00;                        // EID8 (not used)
    buf[4] = 0x00;                        // EID0 (not used)
    buf[5] = len & 0x0F;                  // DLC (0–8)

    // Copy payload
    for (uint8_t i = 0; i < len; i++)
        buf[6 + i] = data[i];

    // Send SPI frame
    MCP_CS_L();
    HAL_SPI_Transmit(&hspi1, buf, 6 + len, HAL_MAX_DELAY);
    MCP_CS_H();

    // Request-to-send TX buffer 0
    uint8_t cmd = MCP_RTS_TXB0;
    MCP_CS_L();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    MCP_CS_H();
}

static int mcp_read_message(void) // Function for reading CAN message, returns lenght of data read
{
    uint8_t status = MCP2515_ReadRegister(REG_CANINTF);
    uint8_t cmd;
    uint8_t buffer[13];
    int dlc = 0;

    // Determine which RX buffer received the message
    if (status & CANSTAT_RX0IF)  // Message in RXB0
        cmd = MCP_READ_RX;  // 0x90
    else if (status & CANSTAT_RX1IF) // Message in RXB1
        cmd = MCP_READ_RX | 0x04; // 0x94
    else
        return 0;

    // Read the message
    MCP_CS_L();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(&hspi1, buffer, 13, HAL_MAX_DELAY);
    MCP_CS_H();

    // Extract the length of the CAN message (it is stored in [4]) (sometimes it can be in the [3] need to verify it better)
    dlc = buffer[4] & 0x0F;
    if (dlc > 8) dlc = 8;

    for (int i = 0; i < dlc; i++)
        rx_data[i] = buffer[4 + i];

    // Clear the RX interrupt flag
    if (status & CANSTAT_RX0IF)
        MCP2515_BitModify(REG_CANINTF, CANSTAT_RX0IF, 0);
    if (status & CANSTAT_RX1IF)
        MCP2515_BitModify(REG_CANINTF, CANSTAT_RX1IF, 0);

    return dlc;
}


void app_threadx_entry(ULONG thread_input) // CAN Thread entry pioint
{
    tx_thread_sleep(10);

    mcp_init();

    tx_thread_sleep(100);  // 1 second
    // Send frame to indicate startup
    uint8_t msg[1] = {0x01};
    mcp_send_message(0x103, msg, 1); //test message: ID: 103, DATA: 01, LENGTH: 1

    while (1)
    {
        tx_thread_sleep(100);  // 1 second

        // Check RX interrupt flags in CANINTF
        uint8_t status = MCP2515_ReadRegister(REG_CANINTF);
        int len = 0;
        if (status & CANSTAT_RX0IF || status & CANSTAT_RX1IF)
        {
            len = mcp_read_message();  // Reads RX0 or RX1 automatically - FILLS DATA IN rx_data[]

            uint8_t seq = rx_data[0]; // The ID from the received message

            uint8_t ack_message[2] = {seq, 0xAC};
            mcp_send_message(0x102, ack_message, 2); // send acknowledge message received

            for (int i = 0; i < len; i++) //loop the bytes received (winks LED)
            {
                HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_7);  // Toggle LED once

                tx_thread_sleep(100);
                // rx_data[] contains received payload
            }
        }

        tx_thread_sleep(100);  // 1 second
    }
}
