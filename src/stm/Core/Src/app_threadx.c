/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mcp2515.h"
#include "lps22hh.h"
#include "speedometer.h"
#include "ism330dhcx.h"
#include "iis2mdc.h"
#include "veml6030.h"
#include "vl53l5cx_driver.h"
#include "hts221.h"
#include "main.h"
#include <stdio.h>
#include "can_id.h"
#include "thread_config.h"
#include "lcd1602.h"
#include "motor_control.h"
#include "servo.h"
#include "tim.h"
#include "emergency_stop.h"
#include "srf08.h"
#include "ina226.h"
#include "gesture_simple.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* LCD1602 Control - Set to 1 to enable LCD, 0 to disable */
#define ENABLE_LCD 1

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* Thread control blocks */
TX_THREAD heartbeat_thread;
TX_THREAD can_rx_thread;
TX_THREAD temperature_thread;
TX_THREAD speed_thread;
TX_THREAD imu_thread;
TX_THREAD tof_thread;
TX_THREAD srf08_thread;
TX_THREAD battery_thread;
TX_THREAD thread_relay;
TX_QUEUE  can_rx_queue;
/* Thread stacks */
UCHAR           thread_relay_stack[1024];
uint8_t         queue_buffer[64]; // Espaço para a queue (tamanho da msg * profundidade)

static uint8_t heartbeat_thread_stack[HEARTBEAT_THREAD_STACK_SIZE];
static uint8_t can_rx_thread_stack[CAN_RX_THREAD_STACK_SIZE];
static uint8_t temperature_thread_stack[TEMP_THREAD_STACK_SIZE];
static uint8_t speed_thread_stack[SPEED_THREAD_STACK_SIZE];
static uint8_t imu_thread_stack[IMU_THREAD_STACK_SIZE];
static uint8_t tof_thread_stack[TOF_THREAD_STACK_SIZE];
static uint8_t srf08_thread_stack[SRF08_THREAD_STACK_SIZE];
static uint8_t battery_thread_stack[1024];

/* Mutex for printf protection */
TX_MUTEX printf_mutex;

/* VL53L5CX Configuration Structure (simples) */
extern VL53L5CX_Configuration Dev;

/* SPI handle for MCP2515 */
extern SPI_HandleTypeDef hspi1;

/* I2C handle for LCD1602 */
extern I2C_HandleTypeDef hi2c1;

/* TIM handle for Servo PWM */
extern TIM_HandleTypeDef htim1;

/* Shared sensor data for LCD display */
static volatile float shared_voltage = 0.0f;
static volatile float shared_temperature = 0.0f;
static volatile float shared_humidity = 0.0f;
static volatile uint16_t shared_tof_distance = 0;
static volatile uint16_t shared_srf08_distance = 0;
static volatile float shared_speed = 0.0f;

/* Emergency stop state management */
volatile uint8_t emergency_stop_active = 0;
volatile uint8_t srf08_speed_limit = 100;  /* Speed limit 0-100% (100=full speed) */
static EmergencyStopState_t emergency_state = ESTOP_STATE_NORMAL;
static EmergencyStopState_t srf08_emergency_state = ESTOP_STATE_NORMAL;

/* System state tracking for Heartbeat */
static volatile SystemState_t current_system_state = SYSTEM_STATE_INIT;
static volatile DriveMode_t current_drive_mode = DRIVE_MODE_IDLE;
static volatile uint8_t system_error_flags = 0;

/* Odometry tracking for WheelSpeed */
static volatile uint32_t odometry_pulse_counter = 0;
static volatile uint8_t wheel_direction = 0; // 0=forward, 1=reverse

/* Motor status tracking for MotorStatus_t */
static volatile int8_t actual_throttle_applied = 0;
static volatile int8_t actual_steering_applied = 0;
static volatile uint16_t motor_current_estimate_ma = 0;
static volatile uint8_t motor_status_counter = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void HeartBeat_Thread_Entry(ULONG thread_input);
static void CAN_RX_Thread_Entry(ULONG thread_input);
static void Temperature_Thread_Entry(ULONG thread_input);
static void Speed_Thread_Entry(ULONG thread_input);
static void IMU_Thread_Entry(ULONG thread_input);
static void ToF_Thread_Entry(ULONG thread_input);
static void SRF08_Thread_Entry(ULONG thread_input);
static void Battery_Thread_Entry(ULONG thread_input);
/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;

  /* USER CODE BEGIN App_ThreadX_MEM_POOL */

  /* USER CODE END App_ThreadX_MEM_POOL */

  /* USER CODE BEGIN App_ThreadX_Init */

  /* Create mutex for printf protection */
  if (tx_mutex_create(&printf_mutex, "Printf Mutex", TX_NO_INHERIT) != TX_SUCCESS)
  {
    ret = TX_MUTEX_ERROR;
  }

  /* Create HeartBeat thread */
  if (tx_thread_create(&heartbeat_thread,
                       "HeartBeat Thread",
                       HeartBeat_Thread_Entry,
                       0,
                       heartbeat_thread_stack,
                       HEARTBEAT_THREAD_STACK_SIZE,
                       HEARTBEAT_THREAD_PRIORITY,
                       HEARTBEAT_THREAD_PRIORITY,
                       TX_NO_TIME_SLICE,
                       TX_AUTO_START) != TX_SUCCESS)
  {
    ret = TX_THREAD_ERROR;
  }

  /* Create CAN RX thread */
  if (tx_thread_create(&can_rx_thread,
                       "CAN RX Thread",
                       CAN_RX_Thread_Entry,
                       0,
                       can_rx_thread_stack,
                       CAN_RX_THREAD_STACK_SIZE,
                       CAN_RX_THREAD_PRIORITY,
                       CAN_RX_THREAD_PRIORITY,
                       TX_NO_TIME_SLICE,
                       TX_AUTO_START) != TX_SUCCESS)
  {
    ret = TX_THREAD_ERROR;
  }

  /* Create Temperature thread */
  if (tx_thread_create(&temperature_thread,
                       "Temperature Thread",
                       Temperature_Thread_Entry,
                       0,
                       temperature_thread_stack,
                       TEMP_THREAD_STACK_SIZE,
                       TEMP_THREAD_PRIORITY,
                       TEMP_THREAD_PRIORITY,
                       TX_NO_TIME_SLICE,
                       TX_AUTO_START) != TX_SUCCESS)
  {
    ret = TX_THREAD_ERROR;
  }

  /* Create Speed thread */
  if (tx_thread_create(&speed_thread,
                       "Speed Thread",
                       Speed_Thread_Entry,
                       0,
                       speed_thread_stack,
                       SPEED_THREAD_STACK_SIZE,
                       SPEED_THREAD_PRIORITY,
                       SPEED_THREAD_PRIORITY,
                       TX_NO_TIME_SLICE,
                       TX_AUTO_START) != TX_SUCCESS)
  {
    ret = TX_THREAD_ERROR;
  }

  /* Create IMU thread */
  if (tx_thread_create(&imu_thread,
                       "IMU Thread",
                       IMU_Thread_Entry,
                       0,
                       imu_thread_stack,
                       IMU_THREAD_STACK_SIZE,
                       IMU_THREAD_PRIORITY,
                       IMU_THREAD_PRIORITY,
                       TX_NO_TIME_SLICE,
                       TX_AUTO_START) != TX_SUCCESS)
  {
    ret = TX_THREAD_ERROR;
  }

  /* Create ToF thread */
  if (tx_thread_create(&tof_thread,
                       "ToF Thread",
                       ToF_Thread_Entry,
                       0,
                       tof_thread_stack,
                       TOF_THREAD_STACK_SIZE,
                       TOF_THREAD_PRIORITY,
                       TOF_THREAD_PRIORITY,
                       TX_NO_TIME_SLICE,
                       TX_AUTO_START) != TX_SUCCESS)
  {
    ret = TX_THREAD_ERROR;
  }

  /* Create SRF08 thread */
  if (tx_thread_create(&srf08_thread,
                       "SRF08 Thread",
                       SRF08_Thread_Entry,
                       0,
                       srf08_thread_stack,
                       SRF08_THREAD_STACK_SIZE,
                       SRF08_THREAD_PRIORITY,
                       SRF08_THREAD_PRIORITY,
                       TX_NO_TIME_SLICE,
                       TX_AUTO_START) != TX_SUCCESS)
  {
    ret = TX_THREAD_ERROR;
  }

  /* Create Battery thread */
  if (tx_thread_create(&battery_thread,
                       "Battery Thread",
                       Battery_Thread_Entry,
                       0,
                       battery_thread_stack,
                       1024,
                       15, /* Priority 15 (low priority - slow sensor) */
                       15,
                       TX_NO_TIME_SLICE,
                       TX_AUTO_START) != TX_SUCCESS)
  {
    ret = TX_THREAD_ERROR;
  }

  /* USER CODE END App_ThreadX_Init */

  return ret;
}

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
// HELPER FUNCTIONS

static inline void put_u8(uint8_t *dst, uint8_t v)
{
    dst[0] = v;
}

static inline void put_i16_le(uint8_t *dst, int16_t v) // little-endian storage
{
    uint16_t uv = (uint16_t)v;        // reinterpret bits
    dst[0] = (uint8_t)(uv & 0xFF);    // LSB
    dst[1] = (uint8_t)(uv >> 8);      // MSB
}

static inline void put_u16_le(uint8_t *dst, uint16_t v)
{
  dst[0] = (uint8_t)(v & 0xFF);
  dst[1] = (uint8_t)((v >> 8) & 0xFF);
}

/**
  * @brief  Calculate CRC-8 (polynomial 0x07, init 0x00)
  * @param  data: pointer to data buffer
  * @param  len: data length
  * @retval CRC-8 value
  */
static uint8_t calculate_crc8(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0x00;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
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
static uint8_t validate_crc8(const uint8_t *data, uint8_t len)
{
    if (len < 2) return 0; // Frame too short

    uint8_t received_crc = data[len - 1];
    uint8_t calculated_crc = calculate_crc8(data, len - 1);

    return (received_crc == calculated_crc);
}


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

/**
  * @brief  HeartBeat thread entry function
  * @param  thread_input: thread input parameter (not used)
  * @retval None
  */
static void HeartBeat_Thread_Entry(ULONG thread_input)
{
  (void)thread_input;

  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
  printf("\r\n[HeartBeat] Thread iniciada!\r\n");
  tx_mutex_put(&printf_mutex);

  tx_thread_sleep(10);
  mcp_init();

  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
  printf("[HeartBeat] MCP2515 inicializado\r\n");
  tx_mutex_put(&printf_mutex);

  /* Transição para estado READY após inicialização */
  current_system_state = SYSTEM_STATE_READY;

  /* Pequeno delay antes de começar o loop principal */
  tx_thread_sleep(100);

  /* Transição para RUNNING */
  current_system_state = SYSTEM_STATE_RUNNING;

  while (1)
  {
    /* === NOVA IMPLEMENTAÇÃO: Heartbeat_t (0x701) === */
    Heartbeat_t hb_frame;

    /* Estado do sistema */
    hb_frame.state = (uint8_t)current_system_state;

    /* Uptime em milissegundos */
    hb_frame.uptime_ms = HAL_GetTick();

    /* Flags de erro do sistema */
    hb_frame.errors = system_error_flags;

    /* Modo de condução atual */
    hb_frame.mode = (uint8_t)current_drive_mode;

    /* Calcular CRC-8 (todos os bytes exceto o último) */
    hb_frame.crc = calculate_crc8((uint8_t*)&hb_frame, sizeof(hb_frame) - 1);

    /* Enviar frame Heartbeat (8 bytes) */
    mcp_send_message(CAN_ID_HEARTBEAT_STM32, (uint8_t*)&hb_frame, sizeof(hb_frame));

    HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);

    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[HeartBeat] State=%u | Uptime=%lu ms | Errors=0x%02X | Mode=%u | CRC=0x%02X\r\n",
           hb_frame.state, hb_frame.uptime_ms, hb_frame.errors, hb_frame.mode, hb_frame.crc);
    tx_mutex_put(&printf_mutex);

    tx_thread_sleep(CAN_PERIOD_HEARTBEAT_MS);
  }
}


/**
  * @brief  Temperature thread entry function
  * @param  thread_input: thread input parameter (not used)
  * @retval None
  */

static void Temperature_Thread_Entry(ULONG thread_input)
{
  (void)thread_input;

  float temperature = 0.0f;
  float pressure = 0.0f;
  float humidity = 0.0f;
  uint16_t ambient_light = 0;

  HAL_StatusTypeDef status_lps, status_hts, status_veml;

  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
  printf("\r\n[Temperatura] Thread iniciada!\r\n");
  tx_mutex_put(&printf_mutex);

  /* I2C Scanner - Provides necessary delay to avoid I2C bus contention
   * Multiple threads (IMU, ToF, Temperature) all use I2C2 simultaneously
   * Scanner staggers initialization and reduces conflicts
   */
  #define ENABLE_I2C_SCAN 0  // DISABLED: para testar servo sem interferências

  #if ENABLE_I2C_SCAN
  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
  I2C_Scan(&hi2c2, "I2C2");
  tx_mutex_put(&printf_mutex);
  #endif

  /* Initialize sensors */
  status_lps = LPS22HH_Init();
  status_hts = HTS221_Init();
  status_veml = VEML6030_Init();

  /* Report sensor initialization status (only errors to reduce UART congestion) */
  if (status_lps != HAL_OK) {
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[Temp] LPS22HH ERRO=%d\r\n", status_lps);
    tx_mutex_put(&printf_mutex);
  }

  if (status_hts != HAL_OK) {
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[Temp] HTS221 ERRO=%d\r\n", status_hts);
    tx_mutex_put(&printf_mutex);
  }

  if (status_veml != HAL_OK) {
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[Temp] VEML6030 ERRO=%d\r\n", status_veml);
    tx_mutex_put(&printf_mutex);
  }

  /* BUGFIX: tx_thread_sleep() blocks indefinitely in this thread - skip delay
   * Sensors should be ready by now (scanner + other threads already ran)
   */

  /* LCD1602 Initialization - Now initialized AFTER sensors are confirmed working */
  #if ENABLE_LCD
  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
  printf("[LCD] Inicializando LCD1602 (I2C1)...\r\n");
  tx_mutex_put(&printf_mutex);

  LCD1602_Init(&hi2c1);

  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
  printf("[LCD] LCD1602 inicializado com sucesso!\r\n");
  tx_mutex_put(&printf_mutex);
  #else
  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
  printf("[LCD] LCD1602 DESATIVADO\r\n");
  tx_mutex_put(&printf_mutex);
  #endif

  while (1)
  {
    /* Ler dados dos sensores */
    LPS22HH_ReadTemperature(&temperature);
    LPS22HH_ReadPressure(&pressure);
    HTS221_ReadHumidity(&humidity);
    VEML6030_ReadALS(&ambient_light);

    #if ENABLE_LCD
    /* Atualizar variáveis partilhadas para o LCD */
    shared_temperature = temperature;
    shared_humidity = humidity;

    /* Atualizar ambas as linhas do LCD */
    /* Não atualizar durante emergency stop */
    if (!emergency_stop_active) {
        LCD1602_UpdateLine1(shared_voltage, shared_temperature, shared_humidity);
        LCD1602_UpdateLine2(shared_srf08_distance, shared_speed);
    }
    #endif

    /* === NOVA IMPLEMENTAÇÃO: Environment_t (0x420) === */
    Environment_t env_frame;

    /* Temperatura: 0.01°C resolution (ex: 25.3°C -> 2530) */
    env_frame.temperature = (int16_t)(temperature * 100.0f);

    /* Humidade: 0-100% (ex: 45.5% -> 45) */
    env_frame.humidity = (uint8_t)humidity;

    /* Ambient Light: escala x100 (ex: 1234 lux -> 12, max 25500 lux) */
    uint16_t light_scaled = ambient_light / 100;
    if (light_scaled > 255) light_scaled = 255;
    env_frame.ambient_light_x100 = (uint8_t)light_scaled;

    /* Pressão: Pa (ex: 101325 Pa) - 24 bits */
    uint32_t pressure_pa = (uint32_t)(pressure * 100.0f); // hPa -> Pa
    env_frame.pressure = pressure_pa & 0xFFFFFF;

    /* Status: sensor health flags (0 = OK) */
    env_frame.status = 0;
    if (status_lps != HAL_OK) env_frame.status |= (1 << 0); // LPS22HH error
    if (status_hts != HAL_OK) env_frame.status |= (1 << 1); // HTS221 error
    if (status_veml != HAL_OK) env_frame.status |= (1 << 2); // VEML6030 error

    /* Enviar frame Environment (8 bytes) - TUDO num só frame! */
    mcp_send_message(CAN_ID_ENVIRONMENT, (uint8_t*)&env_frame, sizeof(env_frame));

    HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);

    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[Environment] Enviado: %.2f°C | %.2f hPa | %.2f%% | %u lux (8 bytes)\r\n",
           temperature, pressure, humidity, ambient_light);
    tx_mutex_put(&printf_mutex);

    tx_thread_sleep(CAN_PERIOD_ENVIRONMENT_MS);
  }
}




/**
  * @brief  Speed thread entry function
  * @param  thread_input: thread input parameter (not used)
  * @retval None
  */
static void Speed_Thread_Entry(ULONG thread_input)
{
  (void)thread_input;

  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
  printf("\r\n[Speed] Thread iniciada!\r\n");
  tx_mutex_put(&printf_mutex);

  /* Initialize speedometer */
  Speedometer_Init();

  while (1)
  {
    /* Conta pulsos durante o período configurado (CAN_PERIOD_WHEEL_SPEED_MS) */
    for (uint32_t i = 0; i < CAN_PERIOD_WHEEL_SPEED_MS; i++)
    {
      Speedometer_CountPulse();
      tx_thread_sleep(1);
    }

    Speedometer_CalculateSpeed();

    float speed_kmh = Speedometer_GetSpeed();
    float rpm = Speedometer_GetRPM();

    if (speed_kmh < 0.0f) speed_kmh = 0.0f;

    /* === NOVA IMPLEMENTAÇÃO: WheelSpeed_t (0x403) === */
    WheelSpeed_t wheel_frame;

    /* RPM com sinal (positivo = frente, negativo = trás) */
    wheel_frame.rpm = (int16_t)rpm;

    /* Incrementar odometria baseado na velocidade atual */
    /* Estimativa: incrementar quando há movimento detectado */
    if (speed_kmh > 0.1f) {
      odometry_pulse_counter++;
    }
    wheel_frame.total_pulses = odometry_pulse_counter;

    /* Direção baseada no sinal do RPM */
    if (rpm > 0.5f) {
      wheel_direction = 0; // Forward
    } else if (rpm < -0.5f) {
      wheel_direction = 1; // Reverse
    }
    // Se RPM ~= 0, mantém direção anterior
    wheel_frame.direction = wheel_direction;

    /* Status: 0 = OK */
    wheel_frame.status = 0;

    /* Enviar frame WheelSpeed (8 bytes) */
    mcp_send_message(CAN_ID_WHEEL_SPEED, (uint8_t*)&wheel_frame, sizeof(wheel_frame));

    /* Atualizar variável partilhada para o LCD */
    shared_speed = speed_kmh;

    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[Speed] RPM=%d | Pulsos=%lu | Dir=%u | %.0f m/h\r\n",
           wheel_frame.rpm, wheel_frame.total_pulses, wheel_frame.direction, speed_kmh * 1000.0f);
    tx_mutex_put(&printf_mutex);
  }
}


/**
  * @brief  CAN RX thread entry function - Receives and processes actuation commands
  * @param  thread_input: thread input parameter (not used)
  * @retval None
  */

static void CAN_RX_Thread_Entry(ULONG thread_input)
{
  (void)thread_input;

  CAN_Message_t rx_msg;
  HAL_StatusTypeDef motor_status;
  uint32_t last_motor_status_tick = 0;
  uint32_t last_debug_tick = 0;  // Para debug periódico

  /* --- INICIALIZAÇÃO SEGURA DO RELAY --- */
  /* Garante que o Relay começa desligado (Port F, Pin 13) */
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);

  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
  printf("\033[1;36m\r\n[CAN_RX] Thread iniciada - Protocolo CAN atualizado!\r\n\033[0m");
  tx_mutex_put(&printf_mutex);

  /* --- INICIALIZAÇÃO DO MOTOR DRIVER --- */
  motor_status = Motor_Init(&hi2c1);

  if (motor_status == HAL_OK) {
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("\033[1;32m[CAN_RX] Motor Driver TB6612FNG inicializado!\r\n\033[0m");
    tx_mutex_put(&printf_mutex);
  } else {
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("\033[1;31m[CAN_RX] ERRO ao inicializar Motor Driver! Status: %d\r\n\033[0m", motor_status);
    tx_mutex_put(&printf_mutex);
  }

  /* --- INICIALIZAÇÃO DO SERVO MOTOR --- */
  HAL_StatusTypeDef servo_status = Servo_Init(&htim1, TIM_CHANNEL_1);

  if (servo_status == HAL_OK) {
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("\033[1;32m[CAN_RX] Servo MG996R inicializado (PA8/TIM1_CH1)!\r\n\033[0m");
    tx_mutex_put(&printf_mutex);
  } else {
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("\033[1;31m[CAN_RX] ERRO ao inicializar Servo! Status: %d\r\n\033[0m", servo_status);
    tx_mutex_put(&printf_mutex);
  }

  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
  printf("\033[1;36m[CAN_RX] Aguardando comandos (0x200, 0x201, 0x500, 0x700, 0x801)...\r\n\033[0m");
  tx_mutex_put(&printf_mutex);

  while (1)
  {
    /* === ENVIO PERIÓDICO DE MotorStatus_t (0x210) === */
    uint32_t current_tick = HAL_GetTick();
    if ((current_tick - last_motor_status_tick) >= CAN_PERIOD_MOTOR_STATUS_MS)
    {
      last_motor_status_tick = current_tick;

      MotorStatus_t motor_status_frame;

      /* Preencher dados do estado atual do motor */
      motor_status_frame.actual_throttle = actual_throttle_applied;
      motor_status_frame.actual_steering = actual_steering_applied;
      motor_status_frame.motor_current_ma = motor_current_estimate_ma;
      motor_status_frame.driver_temp = 25; // TODO: Ler temperatura real do TB6612
      motor_status_frame.pwm_duty = (uint8_t)((abs(actual_throttle_applied) * 255) / 100);
      motor_status_frame.counter = motor_status_counter++;

      /* Calcular CRC */
      motor_status_frame.crc = calculate_crc8((uint8_t*)&motor_status_frame,
                                               sizeof(motor_status_frame) - 1);

      /* Enviar frame (8 bytes) */
      mcp_send_message(CAN_ID_MOTOR_STATUS, (uint8_t*)&motor_status_frame,
                       sizeof(motor_status_frame));
    }

    /* === LIMPEZA PERIÓDICA DE FLAGS - a cada 5 segundos === */
    if ((current_tick - last_debug_tick) >= 5000)
    {
      last_debug_tick = current_tick;
      uint8_t canintf = MCP2515_ReadRegister(REG_CANINTF);

      /* Limpar flags de erro se estiverem ativas (bits 2-7) */
      uint8_t error_flags = canintf & 0xFC;  // Bits 2-7 (TX, erro, overflow, etc)
      if (error_flags)
      {
        MCP2515_BitModify(REG_CANINTF, error_flags, 0x00);
      }

      /* Verificar e limpar registo EFLG (overflow, erros de bus) */
      uint8_t eflg = MCP2515_ReadRegister(REG_EFLG);
      if (eflg != 0x00)
      {
        /* Limpar flags de overflow (RX0OVR bit 6, RX1OVR bit 7) */
        MCP2515_WriteRegister(REG_EFLG, 0x00);
      }
    }

    /* === RECEÇÃO DE MENSAGENS CAN === */
    uint8_t check_result = MCP2515_CheckReceive();

    if (check_result)
    {
      /* Read the message */
      if (MCP2515_ReadMessage(&rx_msg))
      {
        /* Process message based on ID - APENAS IDs definidos em can_id.h */
        switch (rx_msg.id)
        {
          /* === EMERGENCY STOP (0x001) - Bi-directional === */
          case CAN_ID_EMERGENCY_STOP:
            if (rx_msg.dlc >= sizeof(EmergencyStop_t))
            {
              EmergencyStop_t *estop = (EmergencyStop_t*)rx_msg.data;

              /* Validar CRC */
              if (!validate_crc8(rx_msg.data, sizeof(EmergencyStop_t)))
              {
                tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
                printf("\033[1;31m[CAN_RX] EmergencyStop CRC INVÁLIDO!\r\n\033[0m");
                tx_mutex_put(&printf_mutex);
                break;
              }

              if (estop->active)
              {
                /* Emergency Stop recebido do AGL */
                emergency_stop_active = 1;
                Motor_Stop();
                actual_throttle_applied = 0;
                actual_steering_applied = 0;

                tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
                printf("\033[1;31m[CAN_RX] EMERGENCY STOP from AGL! Source=%u Reason=0x%02X Dist=%u mm\r\n\033[0m",
                       estop->source, estop->reason, estop->distance_mm);
                tx_mutex_put(&printf_mutex);

                #if ENABLE_LCD
                LCD1602_SetRGB(LCD_COLOR_EMERGENCY_R, LCD_COLOR_EMERGENCY_G, LCD_COLOR_EMERGENCY_B);
                LCD1602_SetCursor(0, 0);
                LCD1602_Print("!ESTOP FROM AGL!");
                LCD1602_SetCursor(0, 1);
                char buf[17];
                snprintf(buf, sizeof(buf), "Src:%u R:0x%02X    ", estop->source, estop->reason);
                LCD1602_Print(buf);
                #endif
              }
              else
              {
                /* Emergency Stop cleared pelo AGL */
                emergency_stop_active = 0;

                tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
                printf("\033[1;32m[CAN_RX] Emergency CLEARED by AGL\r\n\033[0m");
                tx_mutex_put(&printf_mutex);

                #if ENABLE_LCD
                LCD1602_SetRGB(LCD_COLOR_NORMAL_R, LCD_COLOR_NORMAL_G, LCD_COLOR_NORMAL_B);
                LCD1602_UpdateLine2(shared_srf08_distance, shared_speed);
                LCD1602_SetCursor(0, 0);
                LCD1602_Print("T:--C H:--%    ");
                #endif
              }
            }
            break;

          /* === MOTOR COMMAND (0x200) - AGL -> STM32 === */
          case CAN_ID_MOTOR_CMD:
            if (rx_msg.dlc >= sizeof(MotorCmd_t))
            {
              MotorCmd_t *cmd = (MotorCmd_t*)rx_msg.data;

              /* Validar CRC */
              if (!validate_crc8(rx_msg.data, sizeof(MotorCmd_t)))
              {
                tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
                printf("\033[1;31m[CAN_RX] MotorCmd CRC INVÁLIDO!\r\n\033[0m");
                tx_mutex_put(&printf_mutex);
                break;
              }

              /* Processar flags */
              if (cmd->flags & CMD_FLAG_EMERGENCY_STOP)
              {
                Motor_Stop();
                actual_throttle_applied = 0;
                actual_steering_applied = 0;

                tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
                printf("\033[1;31m[CAN_RX] EMERGENCY STOP!\r\n\033[0m");
                tx_mutex_put(&printf_mutex);
                break;
              }

              /* --- CONTROLO DO MOTOR (Throttle: -100 a +100) --- */
              int8_t throttle = cmd->throttle;

              /* Bloquear durante emergency stop, EXCETO marcha-atrás */
              if (emergency_stop_active && throttle >= 0)
              {
                tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
                printf("\033[1;33m[CAN_RX] Forward BLOCKED - Emergency! (Reverse OK)\r\n\033[0m");
                tx_mutex_put(&printf_mutex);

                /* Para o motor se tentar andar para frente */
                Motor_Stop();
                actual_throttle_applied = 0;
                break;
              }

              /* Atualizar modo de condução */
              current_drive_mode = (DriveMode_t)cmd->mode;

              /* --- CONTROLO DO SERVO (Steering: -100 a +100) --- */
              int8_t steering = cmd->steering;
              if (steering < -100) steering = -100;
              if (steering > 100) steering = 100;

              uint8_t servo_angle = (uint8_t)((steering + 100) * 180 / 200);
              Servo_SetAngle(servo_angle);
              actual_steering_applied = steering;
              if (throttle < -100) throttle = -100;
              if (throttle > 100) throttle = 100;

              /* Aplicar limite de velocidade do SRF08 (proximity-based speed control) */
              if (throttle > (int8_t)srf08_speed_limit) {
                throttle = (int8_t)srf08_speed_limit;
              }

              if (cmd->flags & CMD_FLAG_BRAKE || throttle == 0)
              {
                Motor_Stop();
                actual_throttle_applied = 0;
              }
              else if (throttle > 0)
              {
                Motor_Forward((uint8_t)throttle);
                actual_throttle_applied = throttle;
              }
              else // throttle < 0
              {
                Motor_Backward((uint8_t)(-throttle));
                actual_throttle_applied = throttle;
              }

              /* Estimativa de corrente (proporcional ao throttle) */
              motor_current_estimate_ma = (uint16_t)(abs(throttle) * 20); // ~2A @ 100%

              /* Log periódico (a cada 10 comandos) */
              static uint8_t cmd_log_counter = 0;
              if (++cmd_log_counter >= 10)
              {
                cmd_log_counter = 0;
                tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
                printf("\033[1;32m[CAN_RX] MotorCmd: T=%d S=%d Mode=%u Flags=0x%02X\r\n\033[0m",
                       throttle, steering, cmd->mode, cmd->flags);
                tx_mutex_put(&printf_mutex);
              }
            }
            break;

          /* === CONFIG COMMAND (0x201) - Placeholder === */
          case CAN_ID_CONFIG_CMD:
            tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
            printf("\033[1;33m[CAN_RX] ConfigCmd recebido (não implementado)\r\n\033[0m");
            tx_mutex_put(&printf_mutex);
            break;

          /* === HEARTBEAT AGL (0x700) === */
          case CAN_ID_HEARTBEAT_AGL:
            if (rx_msg.dlc >= sizeof(Heartbeat_t))
            {
              Heartbeat_t *hb = (Heartbeat_t*)rx_msg.data;

              /* Validar CRC */
              if (!validate_crc8(rx_msg.data, sizeof(Heartbeat_t)))
              {
                tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
                printf("\033[1;31m[CAN_RX] Heartbeat AGL CRC INVÁLIDO!\r\n\033[0m");
                tx_mutex_put(&printf_mutex);
                break;
              }

              tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
              printf("\033[1;32m[CAN_RX] Heartbeat AGL: State=%u Uptime=%lu ms Errors=0x%02X\r\n\033[0m",
                     hb->state, hb->uptime_ms, hb->errors);
              tx_mutex_put(&printf_mutex);

              /* TODO: Implementar timeout watchdog */
            }
            break;

          /* === RELAY CONTROL (0x801) === */
          case CAN_ID_CMD_RELAY:
            if (rx_msg.dlc >= 1)
            {
              uint8_t relay_state = rx_msg.data[0];

              if (relay_state == 1)
              {
                HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
                tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
                printf("\033[1;32m[CAN_RX] RELAY ON\r\n\033[0m");
                tx_mutex_put(&printf_mutex);
              }
              else
              {
                HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
                tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
                printf("\033[1;31m[CAN_RX] RELAY OFF\r\n\033[0m");
                tx_mutex_put(&printf_mutex);
              }
            }
            break;

          /* === JOYSTICK CONTROL (0x500) - Legacy Support === */
          case CAN_ID_JOYSTICK:
            if (rx_msg.dlc >= 4)
            {
              /* Extrair steering e throttle (little-endian int16) */
              int16_t steering = (int16_t)(rx_msg.data[0] | (rx_msg.data[1] << 8));
              int16_t throttle = (int16_t)(rx_msg.data[2] | (rx_msg.data[3] << 8));

              /* Bloquear apenas FRENTE durante emergency stop (marcha-atrás permitida) */
              if (emergency_stop_active && throttle >= 0)
              {
                tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
                printf("\033[1;33m[CAN_RX] Joystick Forward BLOCKED - Emergency! (Reverse OK)\r\n\033[0m");
                tx_mutex_put(&printf_mutex);

                /* Para o motor se tentar andar para frente */
                Motor_Stop();
                actual_throttle_applied = 0;
                break;
              }

              /* Limitar valores */
              if (steering < -100) steering = -100;
              if (steering > 100) steering = 100;
              if (throttle < -100) throttle = -100;
              if (throttle > 100) throttle = 100;

              /* Aplicar limite de velocidade do SRF08 (proximity-based speed control) */
              if (throttle > (int8_t)srf08_speed_limit) {
                throttle = (int8_t)srf08_speed_limit;
              }

              /* Controlo do servo */
              uint8_t servo_angle = (uint8_t)((steering + 100) * 180 / 200);
              Servo_SetAngle(servo_angle);
              actual_steering_applied = (int8_t)steering;

              /* Controlo do motor */
              if (throttle > 10)
              {
                Motor_Forward((uint8_t)throttle);
                actual_throttle_applied = (int8_t)throttle;
              }
              else if (throttle < -10)
              {
                Motor_Backward((uint8_t)(-throttle));
                actual_throttle_applied = (int8_t)throttle;
              }
              else
              {
                Motor_Stop();
                actual_throttle_applied = 0;
              }

              /* Log periódico */
              static uint8_t joystick_log_counter = 0;
              if (++joystick_log_counter >= 10)
              {
                joystick_log_counter = 0;
                tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
                printf("\033[1;34m[CAN_RX] Joystick: S=%d (%u°) T=%d\r\n\033[0m",
                       steering, servo_angle, throttle);
                tx_mutex_put(&printf_mutex);
              }
            }
            break;

          default:
            /* ID desconhecido */
            tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
            printf("\033[1;36m[CAN_RX] ID desconhecido: 0x%03lX\r\n\033[0m", rx_msg.id);
            tx_mutex_put(&printf_mutex);
            break;
        }
      }
    }

    /* Small delay to avoid busy-waiting */
    tx_thread_sleep(10);  // 10 ticks (~10ms)
  }
}


/**
  * @brief  IMU thread entry function
  * @param  thread_input: thread input parameter (not used)
  * @retval None
  */
static void IMU_Thread_Entry(ULONG thread_input)
{
  (void)thread_input;

  ISM330DHCX_AxesRaw_t accel_data;
  ISM330DHCX_AxesRaw_t gyro_data;
  IIS2MDC_MagData_t mag_data;
  HAL_StatusTypeDef status_accel, status_gyro, status_mag;
  HAL_StatusTypeDef init_status_ism, init_status_iis;

  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
  printf("\r\n[IMU] Thread iniciada!\r\n");
  tx_mutex_put(&printf_mutex);

  /* Initialize ISM330DHCX */
  init_status_ism = ISM330DHCX_Init();

  if (init_status_ism == HAL_OK)
  {
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[IMU] ISM330DHCX inicializado com sucesso!\r\n");
    tx_mutex_put(&printf_mutex);
  }
  else
  {
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[IMU] ERRO ao inicializar ISM330DHCX! Status: %d\r\n", init_status_ism);
    tx_mutex_put(&printf_mutex);
  }

  /* Initialize IIS2MDC (Magnetometer) */
  init_status_iis = IIS2MDC_Init();

  if (init_status_iis == HAL_OK)
  {
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[IMU] IIS2MDC (Magnetometro) inicializado com sucesso!\r\n");
    tx_mutex_put(&printf_mutex);
  }
  else
  {
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[IMU] ERRO ao inicializar IIS2MDC! Status: %d\r\n", init_status_iis);
    tx_mutex_put(&printf_mutex);
  }

  while (1)
  {
    /* === ACELERÔMETRO: ImuAccel_t (0x400) === */
    status_accel = ISM330DHCX_ReadAccel(&accel_data);

    if (status_accel == HAL_OK)
    {
      ImuAccel_t accel_frame;

      /* Conversão para milli-g (ex: 1.234g -> 1234 mg) */
      accel_frame.acc_x = (int16_t)(accel_data.x * 1000.0f);
      accel_frame.acc_y = (int16_t)(accel_data.y * 1000.0f);
      accel_frame.acc_z = (int16_t)(accel_data.z * 1000.0f);

      accel_frame.reserved = 0;

      /* Status: bit 0 = sensor error */
      accel_frame.status = (init_status_ism != HAL_OK) ? (1 << 0) : 0;

      /* Enviar frame (8 bytes) */
      mcp_send_message(CAN_ID_IMU_ACCEL, (uint8_t*)&accel_frame, sizeof(accel_frame));
    }

    /* Small delay before reading gyro */
    tx_thread_sleep(5);

    /* === GIROSCÓPIO: ImuGyro_t (0x401) === */
    status_gyro = ISM330DHCX_ReadGyro(&gyro_data);

    if (status_gyro == HAL_OK)
    {
      ImuGyro_t gyro_frame;

      /* Conversão para 0.1 °/s (ex: 123.4 dps -> 1234) */
      gyro_frame.gyro_x = (int16_t)(gyro_data.x * 10.0f);
      gyro_frame.gyro_y = (int16_t)(gyro_data.y * 10.0f);
      gyro_frame.gyro_z = (int16_t)(gyro_data.z * 10.0f);

      gyro_frame.reserved = 0;
      gyro_frame.status = (init_status_ism != HAL_OK) ? (1 << 0) : 0;

      /* Enviar frame (8 bytes) */
      mcp_send_message(CAN_ID_IMU_GYRO, (uint8_t*)&gyro_frame, sizeof(gyro_frame));
    }

    /* Small delay before reading magnetometer */
    tx_thread_sleep(5);

    /* === MAGNETÔMETRO: ImuMag_t (0x402) === */
    status_mag = IIS2MDC_ReadMag(&mag_data);

    if (status_mag == HAL_OK)
    {
      ImuMag_t mag_frame;

      /* Dados já em milli-Gauss */
      mag_frame.mag_x = (int16_t)(mag_data.x);
      mag_frame.mag_y = (int16_t)(mag_data.y);
      mag_frame.mag_z = (int16_t)(mag_data.z);

      mag_frame.reserved = 0;
      mag_frame.status = (init_status_iis != HAL_OK) ? (1 << 0) : 0;

      /* Enviar frame (8 bytes) */
      mcp_send_message(CAN_ID_IMU_MAG, (uint8_t*)&mag_frame, sizeof(mag_frame));

      tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
      printf("[IMU] Accel(%.2f, %.2f, %.2f)g | Gyro(%.1f, %.1f, %.1f)dps | Mag(%.0f, %.0f, %.0f)mG\r\n",
             accel_data.x, accel_data.y, accel_data.z,
             gyro_data.x, gyro_data.y, gyro_data.z,
             mag_data.x, mag_data.y, mag_data.z);
      tx_mutex_put(&printf_mutex);
    }

    /* Sleep ajustado: CAN_PERIOD_IMU_FAST_MS menos os delays internos (5+5=10ms) */
    uint32_t imu_sleep = (CAN_PERIOD_IMU_FAST_MS > 10) ? (CAN_PERIOD_IMU_FAST_MS - 10) : 0;
    tx_thread_sleep(imu_sleep);
  }
}


/**
  * @brief  ToF thread entry function
  * @param  thread_input: thread input parameter (not used)
  * @retval None
  */

/**
  * @brief  ToF thread entry function - 8x8 mode with Gesture Detection
  * @param  thread_input: thread input parameter (not used)
  * @retval None
  * @note   Emergency stop has been moved to SRF08 thread
  * @note   Uses movement accumulation algorithm (tested and working)
  */
static void ToF_Thread_Entry(ULONG thread_input)
{
  (void)thread_input;

  uint8_t status;
  uint8_t isAlive;
  uint8_t isReady;
  VL53L5CX_ResultsData Results;

  /* Gesture detection variables - Movement Accumulation Algorithm */
  static GestureType_t last_gesture = GESTURE_NONE;
  GestureType_t detected_gesture = GESTURE_NONE;

  /* Tracking state for movement accumulation */
  float start_x = -1.0f;
  float start_z = -1.0f;
  uint32_t start_time_gesture = 0;
  float prev_center_x = -1.0f;
  uint32_t hand_lost_time = 0;

  /* Current hand position */
  float center_x = 0.0f;
  float center_y = 0.0f;
  uint16_t min_z = 4000;
  uint8_t hand_detected = 0;

  /* Gesture thresholds */
  #define GESTURE_SWIPE_THRESHOLD   2.0f    /* zones of movement */
  #define GESTURE_TAP_Z_THRESHOLD   100     /* mm approach */
  #define GESTURE_TAP_MIN_DIST      150     /* mm minimum distance for TAP */
  #define GESTURE_MAX_DETECTION_MM  500     /* mm maximum detection range */
  #define GESTURE_HAND_LOST_MS      300     /* ms before considering hand lost */
  #define GESTURE_COOLDOWN_MS       500     /* ms between gestures */

  uint32_t last_gesture_time = 0;

  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
  printf("\r\n[ToF] Thread iniciada - Modo 8x8 + Gestos (Acumulacao)!\r\n");
  tx_mutex_put(&printf_mutex);

  /* Delay inicial para deixar outras threads I2C2 estabilizarem */
  tx_thread_sleep(500);

  /* 1. Configurar I2C Address */
  Dev.platform.address = 0x52;

  /* 2. Reset via XSHUT (Hardware Reset) */
  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
  printf("[ToF] Reset XSHUT...\r\n");
  tx_mutex_put(&printf_mutex);

  HAL_GPIO_WritePin(Mems_VL53_xshut_GPIO_Port, Mems_VL53_xshut_Pin, GPIO_PIN_RESET);
  tx_thread_sleep(20);  /* 20ms com XSHUT low */
  HAL_GPIO_WritePin(Mems_VL53_xshut_GPIO_Port, Mems_VL53_xshut_Pin, GPIO_PIN_SET);
  tx_thread_sleep(100); /* 100ms para o sensor arrancar apos XSHUT high */

  /* 3. Verificar se está vivo */
  status = vl53l5cx_is_alive(&Dev, &isAlive);

  if (!isAlive || status) {
      tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
      printf("[ToF] ERRO: Sensor nao detetado!\r\n");
      tx_mutex_put(&printf_mutex);
  }

  /* 4. Inicializar (Upload de Firmware) */
  status = vl53l5cx_init(&Dev);

  if (status) {
      tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
      printf("[ToF] ERRO: Init falhou!\r\n");
      tx_mutex_put(&printf_mutex);
  } else {
      tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
      printf("[ToF] VL53L5CX inicializado (8x8)!\r\n");
      tx_mutex_put(&printf_mutex);
  }

  /* 5. Configurar para 8x8 resolucao (64 zonas) para gestos */
  status = vl53l5cx_set_resolution(&Dev, VL53L5CX_RESOLUTION_8X8);
  if (status) {
      tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
      printf("[ToF] ERRO: Falha ao configurar 8x8!\r\n");
      tx_mutex_put(&printf_mutex);
  }

  /* 6. Configurar frequencia mais alta para gestos (15Hz) */
  status = vl53l5cx_set_ranging_frequency_hz(&Dev, 15);
  if (status) {
      tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
      printf("[ToF] AVISO: Falha ao configurar frequencia\r\n");
      tx_mutex_put(&printf_mutex);
  }

  /* 7. Iniciar Leitura (Ranging) */
  status = vl53l5cx_start_ranging(&Dev);

  if (status) {
      tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
      printf("[ToF] ERRO: Start ranging falhou!\r\n");
      tx_mutex_put(&printf_mutex);
  } else {
      tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
      printf("[ToF] Ranging iniciado (8x8 @ 15Hz)!\r\n");
      tx_mutex_put(&printf_mutex);
  }

  /* --- LOOP PRINCIPAL DA THREAD --- */
  while (1)
  {
      /* Verificar se ha dados prontos */
      status = vl53l5cx_check_data_ready(&Dev, &isReady);

      if (isReady)
      {
          vl53l5cx_get_ranging_data(&Dev, &Results);

          uint32_t current_time = HAL_GetTick();
          detected_gesture = GESTURE_NONE;

          /* === FIND HAND CENTROID IN 8x8 GRID === */
          float sum_x = 0.0f, sum_y = 0.0f;
          int valid_count = 0;
          min_z = 4000;
          uint8_t nearest_zone = 255;

          for (int i = 0; i < 64; i++) {
              /* Check if valid target within gesture range */
              if ((Results.target_status[i] == 5 || Results.target_status[i] == 9) &&
                  Results.distance_mm[i] > 0 && Results.distance_mm[i] < GESTURE_MAX_DETECTION_MM) {

                  int row = i / 8;  /* 0-7 */
                  int col = i % 8;  /* 0-7 */

                  sum_x += col;
                  sum_y += row;
                  valid_count++;

                  if (Results.distance_mm[i] < min_z) {
                      min_z = Results.distance_mm[i];
                      nearest_zone = (uint8_t)i;
                  }
              }
          }

          /* Need at least 3 valid zones to detect hand */
          if (valid_count >= 3) {
              center_x = sum_x / valid_count;
              center_y = sum_y / valid_count;
              hand_detected = 1;
              hand_lost_time = 0;
          } else {
              hand_detected = 0;
              if (hand_lost_time == 0) {
                  hand_lost_time = current_time;
              }
          }

          /* === GESTURE DETECTION WITH MOVEMENT ACCUMULATION === */
          if (hand_detected) {
              /* Check cooldown */
              if ((current_time - last_gesture_time) < GESTURE_COOLDOWN_MS) {
                  /* Still in cooldown, don't detect */
              }
              else if (start_x < 0) {
                  /* First detection - save start position */
                  start_x = center_x;
                  start_z = min_z;
                  start_time_gesture = current_time;
                  prev_center_x = center_x;
              }
              else {
                  /* Calculate total movement from start */
                  float total_dx = center_x - start_x;
                  float total_dz = (float)min_z - start_z;

                  /* SWIPE LEFT: moved left more than threshold */
                  if (total_dx < -GESTURE_SWIPE_THRESHOLD) {
                      detected_gesture = GESTURE_SWIPE_LEFT;
                      start_x = -1.0f;  /* Reset tracking */
                      last_gesture_time = current_time;
                  }
                  /* SWIPE RIGHT: moved right more than threshold */
                  else if (total_dx > GESTURE_SWIPE_THRESHOLD) {
                      detected_gesture = GESTURE_SWIPE_RIGHT;
                      start_x = -1.0f;  /* Reset tracking */
                      last_gesture_time = current_time;
                  }
                  /* TAP: approached sensor quickly */
                  else if (total_dz < -GESTURE_TAP_Z_THRESHOLD && min_z < GESTURE_TAP_MIN_DIST) {
                      detected_gesture = GESTURE_TAP;
                      start_x = -1.0f;  /* Reset tracking */
                      last_gesture_time = current_time;
                  }

                  prev_center_x = center_x;
              }
          }
          else {
              /* Hand lost - reset after timeout */
              if (hand_lost_time > 0 && (current_time - hand_lost_time) > GESTURE_HAND_LOST_MS) {
                  start_x = -1.0f;
                  start_z = -1.0f;
                  prev_center_x = -1.0f;
              }
          }

          /* === ENVIAR GESTURE FRAME (0x424) === */
          GestureFrame_t gesture_frame;

          gesture_frame.gesture_type = (uint8_t)detected_gesture;
          gesture_frame.hand_detected = hand_detected;

          /* Convert hand position (0-7) to (-100 to +100) */
          if (hand_detected) {
              gesture_frame.hand_x = (int8_t)((center_x - 3.5f) * 28.57f);
              gesture_frame.hand_y = (int8_t)((center_y - 3.5f) * 28.57f);
              gesture_frame.hand_z_mm = min_z;
              gesture_frame.confidence = 80;
          } else {
              gesture_frame.hand_x = 0;
              gesture_frame.hand_y = 0;
              gesture_frame.hand_z_mm = 0;
              gesture_frame.confidence = 0;
          }

          gesture_frame.status = (status != 0) ? (1 << 0) : 0;

          /* Enviar frame de gesto */
          mcp_send_message(CAN_ID_GESTURE, (uint8_t*)&gesture_frame, sizeof(gesture_frame));

          /* Log apenas quando detecta um gesto novo */
          if (detected_gesture != GESTURE_NONE && detected_gesture != last_gesture) {
              const char* gesture_name = "";
              switch(detected_gesture) {
                  case GESTURE_SWIPE_LEFT:  gesture_name = "SWIPE_LEFT"; break;
                  case GESTURE_SWIPE_RIGHT: gesture_name = "SWIPE_RIGHT"; break;
                  case GESTURE_TAP:         gesture_name = "TAP"; break;
                  default:                  gesture_name = "UNKNOWN"; break;
              }

              tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
              printf("\033[1;35m[GESTURE] %s detectado! Hand(%.1f, %.1f, %u mm)\033[0m\r\n",
                     gesture_name, center_x, center_y, min_z);
              tx_mutex_put(&printf_mutex);

              /* Gestos removidos do LCD - linha 2 reservada para SRF08 + velocidade */
          }
          last_gesture = detected_gesture;

          /* === ENVIAR ToF DISTANCE FRAME (0x422) === */
          /* Encontrar distancia minima global (para qualquer distancia, nao so gestos) */
          uint16_t global_min_dist = 4000;
          uint8_t global_nearest_zone = 255;
          uint8_t valid_targets = 0;

          for (int i = 0; i < 64; i++) {
              if (Results.target_status[i] == 5 || Results.target_status[i] == 9) {
                  valid_targets++;
                  if (Results.distance_mm[i] > 0 && Results.distance_mm[i] < global_min_dist) {
                      global_min_dist = Results.distance_mm[i];
                      global_nearest_zone = (uint8_t)i;
                  }
              }
          }

          ToFDistance_t tof_frame;
          tof_frame.min_distance_mm = global_min_dist;
          tof_frame.nearest_zone = global_nearest_zone;
          tof_frame.target_status = (global_nearest_zone != 255) ? Results.target_status[global_nearest_zone] : 0;
          tof_frame.detection_count = valid_targets;
          tof_frame.reserved[0] = 0;
          tof_frame.reserved[1] = 0;
          tof_frame.status = (status != 0) ? (1 << 0) : 0;

          /* Enviar frame ToF (8 bytes) */
          mcp_send_message(CAN_ID_TOF_DISTANCE, (uint8_t*)&tof_frame, sizeof(tof_frame));

          #if ENABLE_LCD
          /* Atualizar variavel partilhada para o LCD */
          shared_tof_distance = global_min_dist;
          #endif

          /* Log periodico (a cada 15 iteracoes ~1 segundo) */
          static uint8_t log_counter = 0;
          if (++log_counter >= 15) {
              log_counter = 0;
              tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
              printf("[ToF] Dist=%u mm | Zone=%u | Targets=%u | Hand=%s (%.1f,%.1f)\r\n",
                     global_min_dist, global_nearest_zone, valid_targets,
                     hand_detected ? "YES" : "NO", center_x, center_y);
              tx_mutex_put(&printf_mutex);
          }
      }

      /* Sleep mais curto para gestos (66ms = ~15Hz) */
      tx_thread_sleep(66);
  }
}




/**
  * @brief  SRF08 thread entry function - OPTIMIZED FOR MINIMUM LAG
  * @param  thread_input: thread input parameter (not used)
  * @retval None
  */
static void SRF08_Thread_Entry(ULONG thread_input)
{
  (void)thread_input;

  SRF08_HandleTypeDef hsrf08;
  HAL_StatusTypeDef init_status;
  uint32_t can_send_counter = 0;  // Contador para envios CAN periódicos

  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
  printf("\r\n[SRF08] Thread iniciada (PRIORIDADE ALTA - SAFETY CRITICAL)!\r\n");
  tx_mutex_put(&printf_mutex);

  /* Inicializar SRF08 */
  init_status = SRF08_Init(&hsrf08, &hi2c1, SRF08_DEFAULT_ADDR);

  if (init_status == HAL_OK) {
    uint8_t version = SRF08_GetVersion(&hsrf08);

    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[SRF08] Sensor OK! Versao: %d | Emergency @ %u mm\r\n",
           version, SRF08_EMERGENCY_THRESHOLD_MM);
    printf("[SRF08] Configurado: Gain=12 (medio, melhor <200mm), Range=140 (~6m)\r\n");
    printf("[SRF08] Sleep fixo 70ms (sensor nao suporta polling)\r\n");
    tx_mutex_put(&printf_mutex);
  } else {
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[SRF08] ERRO init! Status: %d\r\n", init_status);
    tx_mutex_put(&printf_mutex);
  }

  /* Loop principal - OTIMIZADO PARA LAG MÍNIMO */
  while (1)
  {
    /* Iniciar medição */
    HAL_StatusTypeDef ranging_status = SRF08_StartRanging(&hsrf08);

    /* DEBUG: Verificar se comando foi aceite */
    static uint8_t err_log_counter = 0;
    if (ranging_status != HAL_OK && ++err_log_counter >= 15) {
        err_log_counter = 0;
        tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
        printf("\033[1;31m[SRF08] ERRO ao enviar comando ranging! Status: %d\033[0m\r\n", ranging_status);
        tx_mutex_put(&printf_mutex);
    }

    /* === AGUARDAR MEDIÇÃO COMPLETAR === */
    uint8_t ready = 0;
    uint8_t poll_attempts = 0;

#if SRF08_DISABLE_POLLING
    /* Modo sleep fixo (sem polling) - Mais confiável se sensor não suporta */
    tx_thread_sleep(70);  // 70ms fixo (datasheet: 65ms min)
    ready = 1;
    poll_attempts = 14;   // Para logging
#else
    /* Modo polling inteligente */
    const uint8_t MAX_POLLS = 14;  // 14 * 5ms = 70ms

    while (!ready && poll_attempts < MAX_POLLS) {
        tx_thread_sleep(5);  // Sleep 5ms entre polls
        ready = SRF08_IsReady(&hsrf08);
        poll_attempts++;
    }
    /* Após timeout, assumir que terminou */
#endif

    /* Verificar se deu timeout */
    static uint8_t timeout_log_counter = 0;
    if (!ready && ++timeout_log_counter >= 15) {
        timeout_log_counter = 0;

        // DEBUG: Ler registo COMMAND para ver o que está a retornar
        uint8_t cmd_reg = 0xFF;
        HAL_I2C_Mem_Read(hsrf08.hi2c, hsrf08.addr, SRF08_REG_COMMAND, 1, &cmd_reg, 1, 100);

        tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
        printf("\033[1;33m[SRF08] WARNING: Timeout! Polls=%u | CMD_REG=0x%02X (esperado 0x00)\033[0m\r\n",
               poll_attempts, cmd_reg);
        tx_mutex_put(&printf_mutex);
    }

    /* Ler distância e luz - APENAS se pronto ou timeout */
    uint16_t distance_cm = SRF08_GetDistanceCm(&hsrf08);
    uint8_t light = SRF08_GetLight(&hsrf08);
    uint16_t distance_mm_raw = (distance_cm == 0xFFFF) ? 0 : distance_cm * 10;

    /* === FILTRO DE MÉDIA MÓVEL (5 amostras) === */
    #define SRF08_FILTER_SIZE 5
    #define SRF08_LIGHT_THRESHOLD 2  // Threshold baixo - aceitar mais leituras
    static uint16_t distance_buffer[SRF08_FILTER_SIZE] = {0};
    static uint8_t buffer_index = 0;
    static uint8_t buffer_filled = 0;
    static uint16_t distance_mm_filtered = 0;

    /* Aceitar leitura se:
     * 1. Light >= threshold (eco detectado com boa intensidade)
     * 2. OU distância válida (mesmo com light baixo)
     * 3. Rejeitar apenas se light=0 E distance=0 (sem eco nenhum)
     */
    if (light >= SRF08_LIGHT_THRESHOLD || distance_mm_raw > 0) {
        /* Adicionar ao buffer circular */
        distance_buffer[buffer_index] = distance_mm_raw;
        buffer_index = (buffer_index + 1) % SRF08_FILTER_SIZE;
        if (!buffer_filled && buffer_index == 0) buffer_filled = 1;

        /* Calcular média */
        uint32_t sum = 0;
        uint8_t count = buffer_filled ? SRF08_FILTER_SIZE : buffer_index;
        for (uint8_t i = 0; i < count; i++) {
            sum += distance_buffer[i];
        }
        distance_mm_filtered = (count > 0) ? (sum / count) : 0;
    }
    /* Senão (light=0 E distance=0), manter valor filtrado anterior */

    uint16_t distance_mm = distance_mm_filtered;  // Usar valor filtrado

    /* DEBUG: Log valores brutos do SRF08 */
    static uint8_t srf08_log_counter = 0;
    if (++srf08_log_counter >= 15) {  // Log a cada ~1 segundo
        srf08_log_counter = 0;
        tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
        if (distance_cm == 0xFFFF) {
            printf("\033[1;31m[SRF08] ERRO I2C ao ler distancia!\033[0m\r\n");
        } else {
            printf("\033[1;36m[SRF08] %u mm | L=%u | SpeedLimit=%u%%",
                   distance_mm, light, srf08_speed_limit);
            if (srf08_speed_limit < 100) {
                printf(" \033[1;33m[SLOWDOWN!]\033[1;36m");
            }
            if (light == 0 && distance_cm == 0) {
                printf(" <- SEM ECO");
            }
            printf("\033[0m\r\n");
        }
        tx_mutex_put(&printf_mutex);
    }

    /* Atualizar variável partilhada para o LCD */
    shared_srf08_distance = distance_mm;

    #if ENABLE_LCD
    /* Atualizar LCD em tempo real (65ms update rate) */
    if (!emergency_stop_active) {
        LCD1602_UpdateLine2(shared_srf08_distance, shared_speed);
    }
    #endif

    /* === SPEED CONTROL BASED ON DISTANCE === */
    /* Calcular limite de velocidade baseado na distância */
    if (distance_mm >= SRF08_SLOWDOWN_THRESHOLD_MM || light == 0) {
        /* Distância segura (>500mm) ou sem objeto: velocidade total */
        srf08_speed_limit = 100;
    }
    else if (distance_mm <= SRF08_EMERGENCY_THRESHOLD_MM && light > 0) {
        /* Muito perto (<300mm): PARAR */
        srf08_speed_limit = 0;
    }
    else if (light > 0) {
        /* Zona de slowdown (300-500mm): Reduzir para 25% */
        srf08_speed_limit = SRF08_SLOWDOWN_SPEED_PERCENT;
    }

    /* === EMERGENCY STOP LOGIC - SEMPRE ATIVO (independente de light) === */
    EmergencyStopState_t new_state = srf08_emergency_state;

    /* CRITICAL: Verificar emergency SEMPRE (mesmo com light=0) */
    if (distance_mm < SRF08_EMERGENCY_THRESHOLD_MM && light > 0) {
      /* Entrar em EMERGENCY apenas se light > 0 (objeto detectado) */
      if (srf08_emergency_state != ESTOP_STATE_EMERGENCY) {
        /* 1. PARAR MOTORES IMEDIATAMENTE */
        Motor_Stop();

        /* 2. Atualizar estados */
        new_state = ESTOP_STATE_EMERGENCY;
        emergency_stop_active = 1;

        /* 3. Enviar Emergency Stop CAN */
        EmergencyStop_t estop_frame;
        estop_frame.active = 1;
        estop_frame.source = 1;  /* SRF08 */
        estop_frame.distance_mm = distance_mm;
        estop_frame.reason = 0x01;
        estop_frame.reserved[0] = 0;
        estop_frame.reserved[1] = 0;
        estop_frame.crc = calculate_crc8((uint8_t*)&estop_frame, sizeof(estop_frame) - 1);
        mcp_send_message(CAN_ID_EMERGENCY_STOP, (uint8_t*)&estop_frame, sizeof(estop_frame));

        tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
        printf("\033[1;31m[SRF08 ESTOP!] %u mm < %u mm (L=%u)\033[0m\r\n",
               distance_mm, SRF08_EMERGENCY_THRESHOLD_MM, light);
        tx_mutex_put(&printf_mutex);
      }
    }
    else if (srf08_emergency_state == ESTOP_STATE_EMERGENCY) {
      /* RECOVERY: Se estava em EMERGENCY, verificar se pode sair */
      /* IMPORTANTE: Faz recovery SEMPRE que não há objeto perto, independente de light */
      if (distance_mm >= SRF08_RECOVERY_THRESHOLD_MM || light == 0) {
        /* Recuperar: distância segura OU sem objeto detectado */
        new_state = ESTOP_STATE_NORMAL;
        emergency_stop_active = 0;

        /* Enviar CLEAR CAN */
        EmergencyStop_t estop_frame;
        estop_frame.active = 0;
        estop_frame.source = 1;
        estop_frame.distance_mm = distance_mm;
        estop_frame.reason = 0x00;
        estop_frame.reserved[0] = 0;
        estop_frame.reserved[1] = 0;
        estop_frame.crc = calculate_crc8((uint8_t*)&estop_frame, sizeof(estop_frame) - 1);
        mcp_send_message(CAN_ID_EMERGENCY_STOP, (uint8_t*)&estop_frame, sizeof(estop_frame));

        tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
        printf("\033[1;32m[SRF08] RECOVERY! Dist=%u mm L=%u (CLEAR ENVIADO)\033[0m\r\n",
               distance_mm, light);
        tx_mutex_put(&printf_mutex);
      }
    }
    else if (distance_mm >= SRF08_RECOVERY_THRESHOLD_MM && srf08_emergency_state == ESTOP_STATE_WARNING) {
      /* Sair de WARNING para NORMAL */
      new_state = ESTOP_STATE_NORMAL;
    }
    else if (distance_mm < SRF08_RECOVERY_THRESHOLD_MM && distance_mm >= SRF08_EMERGENCY_THRESHOLD_MM && light > 0) {
      /* Zona de histerese: WARNING */
      if (srf08_emergency_state == ESTOP_STATE_NORMAL) {
        new_state = ESTOP_STATE_WARNING;
      }
    }

    srf08_emergency_state = new_state;
    /* === END EMERGENCY STOP LOGIC === */

    /* === ENVIAR DADOS CAN PERIODICAMENTE === */
    can_send_counter++;
    if (light > 0 && srf08_emergency_state != ESTOP_STATE_EMERGENCY && (can_send_counter % 1) == 0) {
      SRF08Distance_t srf08_frame;
      srf08_frame.distance_mm = distance_mm;
      srf08_frame.light_level = light;
      srf08_frame.gain = 0;
      srf08_frame.range_setting = 0;
      srf08_frame.reserved[0] = 0;
      srf08_frame.reserved[1] = 0;
      srf08_frame.status = 0x01;

      if (init_status != HAL_OK) {
        srf08_frame.status |= (1 << 1);
      }

      mcp_send_message(CAN_ID_SRF08_DISTANCE, (uint8_t*)&srf08_frame, sizeof(srf08_frame));

      /* Printf reduzido - apenas a cada 10 iterações */
      if ((can_send_counter % 10) == 0) {
        tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
        printf("[SRF08] %u mm | L=%u | State=%u\r\n", distance_mm, light, srf08_emergency_state);
        tx_mutex_put(&printf_mutex);
      }
    }

    /* DELAY ZERO - Loop contínuo para resposta máxima */
    tx_thread_relinquish();
  }
}


/**
  * @brief  Battery thread entry function - Uses INA226 for real measurements
  * @param  thread_input: thread input parameter (not used)
  * @retval None
  */
static void Battery_Thread_Entry(ULONG thread_input)
{
  (void)thread_input;

  HAL_StatusTypeDef ina226_status;
  INA226_Data_t ina226_data;

  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
  printf("\r\n[Battery] Thread iniciada!\r\n");
  tx_mutex_put(&printf_mutex);

  /* Initialize INA226 Current/Voltage Monitor on I2C1 */
  ina226_status = INA226_Init(&hi2c1);

  if (ina226_status == HAL_OK) {
    uint16_t mfg_id = INA226_GetManufacturerID();
    uint16_t die_id = INA226_GetDieID();

    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[Battery] INA226 inicializado! MFG=0x%04X DIE=0x%04X\r\n", mfg_id, die_id);
    tx_mutex_put(&printf_mutex);
  } else {
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[Battery] ERRO ao inicializar INA226! Status: %d\r\n", ina226_status);
    tx_mutex_put(&printf_mutex);
  }

  /* Variáveis de estado da bateria */
  uint16_t voltage_mv = 0;
  int16_t current_ma = 0;
  uint8_t soc = 100;                // SOC estimado (sem fuel gauge)
  int8_t temperature = 25;          // Temperatura estimada
  uint8_t cycles = 0;               // Ciclos de carga
  uint8_t battery_status = 0;

  /* Para estimativa de SOC baseada em tensão (LiPo 3S = 11.1V nominal) */
  #define BATTERY_FULL_MV     12600   /* 4.2V × 3 células */
  #define BATTERY_EMPTY_MV    9900    /* 3.3V × 3 células (cutoff seguro) */

  while (1)
  {
    /* === LEITURA REAL DO INA226 === */
    if (ina226_status == HAL_OK)
    {
      HAL_StatusTypeDef read_status = INA226_ReadAll(&ina226_data);

      if (read_status == HAL_OK && ina226_data.valid)
      {
        /* Converter para formato da frame CAN */
        voltage_mv = (uint16_t)(ina226_data.voltage_V * 1000.0f);
        current_ma = (int16_t)(ina226_data.current_A * 1000.0f);

        /* Atualizar variável partilhada para o LCD */
        shared_voltage = ina226_data.voltage_V;

        /* Estimativa de SOC baseada na tensão (método simples) */
        if (voltage_mv >= BATTERY_FULL_MV) {
          soc = 100;
        } else if (voltage_mv <= BATTERY_EMPTY_MV) {
          soc = 0;
        } else {
          /* Interpolação linear entre EMPTY e FULL */
          soc = (uint8_t)(((uint32_t)(voltage_mv - BATTERY_EMPTY_MV) * 100) /
                          (BATTERY_FULL_MV - BATTERY_EMPTY_MV));
        }

        /* Estimativa de temperatura (sem sensor dedicado) */
        /* Aumenta com corrente alta (simplificado) */
        if (abs(current_ma) > 5000) {
          temperature = 35;  /* Alta corrente = aquecimento */
        } else if (abs(current_ma) > 2000) {
          temperature = 30;
        } else {
          temperature = 25;  /* Temperatura ambiente */
        }

        /* Status flags baseados em medições reais */
        battery_status = 0;
        if (voltage_mv < 10500) battery_status |= (1 << 0);  /* Undervoltage warning */
        if (voltage_mv < BATTERY_EMPTY_MV) battery_status |= ERROR_FLAG_UNDERVOLTAGE;
        if (voltage_mv > 13000) battery_status |= ERROR_FLAG_OVERVOLTAGE;
        if (soc < 20) battery_status |= (1 << 2);            /* Low battery */
        if (abs(current_ma) > 15000) battery_status |= (1 << 4); /* Overcurrent */
      }
      else
      {
        /* Falha na leitura - marcar status como erro */
        battery_status = ERROR_FLAG_SENSOR_FAULT;
      }
    }
    else
    {
      /* INA226 não inicializado - manter valores anteriores com flag de erro */
      battery_status = ERROR_FLAG_SENSOR_FAULT;
    }

    /* === CONSTRUIR E ENVIAR BatteryStatus_t (0x421) === */
    BatteryStatus_t battery_frame;

    battery_frame.voltage_mv = voltage_mv;
    battery_frame.current_ma = current_ma;
    battery_frame.soc = soc;
    battery_frame.temperature = temperature;
    battery_frame.cycles = cycles;
    battery_frame.status = battery_status;

    /* Enviar frame (8 bytes) */
    mcp_send_message(CAN_ID_BATTERY, (uint8_t*)&battery_frame, sizeof(battery_frame));

    /* Log com dados reais */
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    if (ina226_status == HAL_OK && ina226_data.valid) {
      printf("[Battery] %.2fV | %.2fA | %.2fW | SOC=%u%% | Status=0x%02X\r\n",
             ina226_data.voltage_V, ina226_data.current_A, ina226_data.power_W,
             soc, battery_status);
    } else {
      printf("[Battery] SENSOR ERROR | Status=0x%02X\r\n", battery_status);
    }
    tx_mutex_put(&printf_mutex);

    /* Sleep configurável via CAN_PERIOD_BATTERY_MS (500ms = 2Hz) */
    tx_thread_sleep(CAN_PERIOD_BATTERY_MS);
  }
}


void thread_relay_entry(ULONG thread_input)
{
    uint8_t rx_data[8]; // Buffer para receber os dados do CAN (payload)
    UINT status;

    while(1)
    {
        /* * TX_WAIT_FOREVER: A thread "dorme" aqui e não gasta CPU
         * até chegar algo à queue vindo da interrupção CAN.
         */
        status = tx_queue_receive(&can_rx_queue, &rx_data, TX_WAIT_FOREVER);

        if (status == TX_SUCCESS)
        {
            // O byte 0 define o estado: 1 = Ligar, 0 = Desligar
            if (rx_data[0] == 1)
            {
                // LIGA O RELAY (PF13)
                HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
                // Opcional: printf("Relay ON\n");
            }
            else
            {
                // DESLIGA O RELAY (PF13)
                HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
                // Opcional: printf("Relay OFF\n");
            }
        }
    }
}


/* USER CODE END 1 */
