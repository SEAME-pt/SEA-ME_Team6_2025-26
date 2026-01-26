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
static volatile float shared_temperature = 0.0f;
static volatile float shared_humidity = 0.0f;
static volatile uint16_t shared_tof_distance = 0;
static volatile float shared_speed = 0.0f;

/* Emergency stop state management */
volatile uint8_t emergency_stop_active = 0;
static EmergencyStopState_t emergency_state = ESTOP_STATE_NORMAL;

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

    /* Atualizar apenas Linha 1 do LCD (Temp/Hum) */
    /* Não atualizar durante emergency stop */
    if (!emergency_stop_active) {
        LCD1602_UpdateLine1(shared_temperature, shared_humidity);
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
                LCD1602_UpdateLine2(shared_tof_distance, shared_speed);
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

              /* Bloquear durante emergency stop */
              if (emergency_stop_active && !(cmd->flags & CMD_FLAG_EMERGENCY_STOP))
              {
                tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
                printf("\033[1;31m[CAN_RX] MotorCmd BLOCKED - Emergency Active!\r\n\033[0m");
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

              /* Atualizar modo de condução */
              current_drive_mode = (DriveMode_t)cmd->mode;

              /* --- CONTROLO DO SERVO (Steering: -100 a +100) --- */
              int8_t steering = cmd->steering;
              if (steering < -100) steering = -100;
              if (steering > 100) steering = 100;

              uint8_t servo_angle = (uint8_t)((steering + 100) * 180 / 200);
              Servo_SetAngle(servo_angle);
              actual_steering_applied = steering;

              /* --- CONTROLO DO MOTOR (Throttle: -100 a +100) --- */
              int8_t throttle = cmd->throttle;
              if (throttle < -100) throttle = -100;
              if (throttle > 100) throttle = 100;

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
            /* Bloquear comandos de joystick durante emergency stop */
            if (emergency_stop_active)
            {
              tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
              printf("\033[1;31m[CAN_RX] Joystick BLOCKED - Emergency Active!\r\n\033[0m");
              tx_mutex_put(&printf_mutex);
              break;
            }

            if (rx_msg.dlc >= 4)
            {
              /* Extrair steering e throttle (little-endian int16) */
              int16_t steering = (int16_t)(rx_msg.data[0] | (rx_msg.data[1] << 8));
              int16_t throttle = (int16_t)(rx_msg.data[2] | (rx_msg.data[3] << 8));

              /* Limitar valores */
              if (steering < -100) steering = -100;
              if (steering > 100) steering = 100;
              if (throttle < -100) throttle = -100;
              if (throttle > 100) throttle = 100;

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
  * @brief  ToF thread entry function (Simple Implementation)
  * @param  thread_input: thread input parameter (not used)
  * @retval None
  */
static void ToF_Thread_Entry(ULONG thread_input)
{
  (void)thread_input;

  uint8_t status;
  uint8_t isAlive;
  uint8_t isReady;
  VL53L5CX_ResultsData Results;

  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
  printf("\r\n[ToF] Thread iniciada!\r\n");
  tx_mutex_put(&printf_mutex);

  /* 1. Configurar I2C Address */
  Dev.platform.address = 0x52;

  /* 2. Reset via XSHUT (Hardware Reset) */
  HAL_GPIO_WritePin(Mems_VL53_xshut_GPIO_Port, Mems_VL53_xshut_Pin, GPIO_PIN_RESET);
  tx_thread_sleep(2);
  HAL_GPIO_WritePin(Mems_VL53_xshut_GPIO_Port, Mems_VL53_xshut_Pin, GPIO_PIN_SET);
  tx_thread_sleep(2);

  /* 3. Verificar se está vivo */
  status = vl53l5cx_is_alive(&Dev, &isAlive);

  if (!isAlive || status) {
      tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
      printf("[ToF] ERRO: Sensor não detetado!\r\n");
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
      printf("[ToF] VL53L5CX inicializado!\r\n");
      tx_mutex_put(&printf_mutex);
  }

  /* 5. Iniciar Leitura (Ranging) */
  status = vl53l5cx_start_ranging(&Dev);

  if (status) {
      tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
      printf("[ToF] ERRO: Start ranging falhou!\r\n");
      tx_mutex_put(&printf_mutex);
  } else {
      tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
      printf("[ToF] Ranging iniciado!\r\n");
      tx_mutex_put(&printf_mutex);
  }

  /* --- LOOP PRINCIPAL DA THREAD --- */
  while (1)
  {
      /* Verificar se há dados prontos */
      status = vl53l5cx_check_data_ready(&Dev, &isReady);

      if (isReady)
      {
          vl53l5cx_get_ranging_data(&Dev, &Results);


          /* Lógica de deteção de gestos/mão: Encontrar o ponto mais próximo */
          uint16_t min_dist = 4000; // Max range
          uint8_t nearest_zone = 255;
          uint8_t valid_targets = 0; // Contador de alvos válidos detectados

          /* Varrer as zonas (assumindo 8x8 = 64 zonas, ou 4x4 = 16) */
          /* O loop até 64 é seguro pois o array tem tamanho fixo */
          for (int i = 0; i < 64; i++) {
              if (Results.distance_mm[i] > 0 && Results.distance_mm[i] < min_dist) {
                  // Filtrar por status (5 e 9 são válidos)
                  if(Results.target_status[i] == 5 || Results.target_status[i] == 9) {
                      min_dist = Results.distance_mm[i];
                      nearest_zone = (uint8_t)i;
                      valid_targets++;
                  }
              } else if (Results.distance_mm[i] > 0) {
                  // Conta alvos válidos mesmo que não sejam o mais próximo
                  if(Results.target_status[i] == 5 || Results.target_status[i] == 9) {
                      valid_targets++;
                  }
              }
          }

          /* === EMERGENCY STOP LOGIC === */
          EmergencyStopState_t new_state = emergency_state;

          if (min_dist < TOF_EMERGENCY_THRESHOLD_MM) {
              // Entrar em EMERGENCY
              if (emergency_state != ESTOP_STATE_EMERGENCY) {
                  new_state = ESTOP_STATE_EMERGENCY;
                  emergency_stop_active = 1;

                  Motor_Stop();  // Parar motores imediatamente

                  #if ENABLE_LCD
                  LCD1602_SetRGB(LCD_COLOR_EMERGENCY_R, LCD_COLOR_EMERGENCY_G, LCD_COLOR_EMERGENCY_B);
                  LCD1602_SetCursor(0, 0);
                  LCD1602_Print("!EMERGENCY STOP!");
                  LCD1602_SetCursor(0, 1);
                  char buf[17];
                  snprintf(buf, sizeof(buf), "Dist: %u mm    ", min_dist);
                  LCD1602_Print(buf);
                  #endif

                  /* Enviar Emergency Stop frame (0x001) */
                  EmergencyStop_t estop_frame;
                  estop_frame.active = 1;                    /* Emergency active */
                  estop_frame.source = 0;                    /* 0 = ToF sensor */
                  estop_frame.distance_mm = min_dist;        /* Distance to obstacle */
                  estop_frame.reason = 0x01;                 /* Reason: Proximity alert */
                  estop_frame.reserved[0] = 0;
                  estop_frame.reserved[1] = 0;
                  estop_frame.crc = calculate_crc8((uint8_t*)&estop_frame, sizeof(estop_frame) - 1);

                  mcp_send_message(CAN_ID_EMERGENCY_STOP, (uint8_t*)&estop_frame, sizeof(estop_frame));

                  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
                  printf("\033[1;31m[EMERGENCY] Dist %u mm < %u mm - MOTORS STOPPED!\033[0m\r\n",
                         min_dist, TOF_EMERGENCY_THRESHOLD_MM);
                  tx_mutex_put(&printf_mutex);
              }
          }
          else if (min_dist >= TOF_RECOVERY_THRESHOLD_MM) {
              // Recuperar para NORMAL
              if (emergency_state == ESTOP_STATE_EMERGENCY) {
                  new_state = ESTOP_STATE_NORMAL;
                  emergency_stop_active = 0;

                  #if ENABLE_LCD
                  LCD1602_SetRGB(LCD_COLOR_NORMAL_R, LCD_COLOR_NORMAL_G, LCD_COLOR_NORMAL_B);
                  LCD1602_UpdateLine2(shared_tof_distance, shared_speed);
                  LCD1602_SetCursor(0, 0);
                  LCD1602_Print("T:--C H:--%    ");
                  #endif

                  /* Enviar Emergency Stop CLEAR frame (0x001) */
                  EmergencyStop_t estop_frame;
                  estop_frame.active = 0;                    /* Emergency cleared */
                  estop_frame.source = 0;                    /* 0 = ToF sensor */
                  estop_frame.distance_mm = min_dist;        /* Current safe distance */
                  estop_frame.reason = 0x00;                 /* Reason: Cleared */
                  estop_frame.reserved[0] = 0;
                  estop_frame.reserved[1] = 0;
                  estop_frame.crc = calculate_crc8((uint8_t*)&estop_frame, sizeof(estop_frame) - 1);

                  mcp_send_message(CAN_ID_EMERGENCY_STOP, (uint8_t*)&estop_frame, sizeof(estop_frame));

                  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
                  printf("\033[1;32m[EMERGENCY] Cleared - dist %u mm > %u mm\033[0m\r\n",
                         min_dist, TOF_RECOVERY_THRESHOLD_MM);
                  tx_mutex_put(&printf_mutex);
              }
              else if (emergency_state == ESTOP_STATE_WARNING) {
                  new_state = ESTOP_STATE_NORMAL;
                  #if ENABLE_LCD
                  LCD1602_SetRGB(LCD_COLOR_NORMAL_R, LCD_COLOR_NORMAL_G, LCD_COLOR_NORMAL_B);
                  LCD1602_UpdateLine2(shared_tof_distance, shared_speed);
                  #endif
              }
          }
          else {
              // Zona de histerese (100-120mm): WARNING
              if (emergency_state == ESTOP_STATE_NORMAL) {
                  new_state = ESTOP_STATE_WARNING;
                  #if ENABLE_LCD
                  LCD1602_SetRGB(LCD_COLOR_WARNING_R, LCD_COLOR_WARNING_G, LCD_COLOR_WARNING_B);
                  #endif
                  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
                  printf("\033[1;33m[WARNING] Proximity: %u mm\033[0m\r\n", min_dist);
                  tx_mutex_put(&printf_mutex);
              }
          }

          emergency_state = new_state;
          /* === END EMERGENCY STOP LOGIC === */

          /* === NOVA IMPLEMENTAÇÃO: ToFDistance_t (0x422) === */
          ToFDistance_t tof_frame;

          if (nearest_zone != 255) // Se encontrou algo válido
          {
              tof_frame.min_distance_mm = min_dist;
              tof_frame.nearest_zone = nearest_zone;
              tof_frame.target_status = Results.target_status[nearest_zone];
              tof_frame.detection_count = valid_targets;
          }
          else
          {
              // Nada detetado ou fora de alcance
              tof_frame.min_distance_mm = 4000;
              tof_frame.nearest_zone = 255;
              tof_frame.target_status = 0;
              tof_frame.detection_count = 0;
          }

          /* Reserved bytes */
          tof_frame.reserved[0] = 0;
          tof_frame.reserved[1] = 0;

          /* Status: sensor health */
          tof_frame.status = (status != 0) ? (1 << 0) : 0;
          if (emergency_stop_active) {
              tof_frame.status |= (1 << 1); // Bit 1: Emergency stop active
          }

          /* Enviar dados normais apenas se NÃO estiver em emergency */
          if (emergency_state != ESTOP_STATE_EMERGENCY) {
              /* Enviar frame ToF (8 bytes) */
              mcp_send_message(CAN_ID_TOF_DISTANCE, (uint8_t*)&tof_frame, sizeof(tof_frame));

              #if ENABLE_LCD
              /* Atualizar variável partilhada para o LCD */
              shared_tof_distance = min_dist;

              /* Atualizar Linha 2 do LCD (ToF/Speed) em tempo real */
              LCD1602_UpdateLine2(shared_tof_distance, shared_speed);
              #endif

              tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
              printf("[ToF] Dist=%u mm | Zona=%u | Targets=%u | Status=0x%02X\r\n",
                     tof_frame.min_distance_mm, tof_frame.nearest_zone,
                     tof_frame.detection_count, tof_frame.status);
              tx_mutex_put(&printf_mutex);
          }
      }

      /* Sleep configurável via CAN_PERIOD_TOF_MS */
      tx_thread_sleep(CAN_PERIOD_TOF_MS);
  }
}




/**
  * @brief  Battery thread entry function
  * @param  thread_input: thread input parameter (not used)
  * @retval None
  */
static void Battery_Thread_Entry(ULONG thread_input)
{
  (void)thread_input;

  tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
  printf("\r\n[Battery] Thread iniciada!\r\n");
  tx_mutex_put(&printf_mutex);

  /* TODO: Inicializar sensor de bateria (INA219, MAX17043, etc.)
   * Por enquanto, vamos usar valores simulados
   */

  /* Variáveis de estado da bateria */
  uint16_t voltage_mv = 12000;      // 12.0V inicial
  int16_t current_ma = 0;           // 0mA inicial
  uint8_t soc = 100;                // 100% carga inicial
  int8_t temperature = 25;          // 25°C
  uint8_t cycles = 0;               // 0 ciclos
  uint8_t battery_status = 0;       // Status OK

  while (1)
  {
    /* === SIMULAÇÃO DE DADOS DE BATERIA === */
    /* TODO: Substituir por leitura real de sensor I2C (INA219, MAX17043, etc.) */

    /* Simular descarga lenta baseada no throttle aplicado */
    if (actual_throttle_applied != 0)
    {
      /* Motor ativo - consumo de corrente */
      current_ma = -((int16_t)abs(actual_throttle_applied) * 30); // ~3A @ 100%

      /* Pequena queda de tensão sob carga */
      voltage_mv = 12000 - (abs(actual_throttle_applied) * 5); // Drop de ~0.5V @ 100%

      /* Descarga do SOC (muito lenta para simulação) */
      if (soc > 0) {
        static uint32_t discharge_counter = 0;
        if (++discharge_counter >= 100) { // Descarrega 1% a cada 100 iterações (50s)
          discharge_counter = 0;
          soc--;
        }
      }
    }
    else
    {
      /* Motor parado - corrente mínima (consumo do sistema) */
      current_ma = -150; // 150mA (sistema)
      voltage_mv = 12000;
    }

    /* Temperatura aumenta ligeiramente com uso */
    if (abs(actual_throttle_applied) > 50)
    {
      temperature = 30; // Aquece para 30°C sob carga
    }
    else
    {
      temperature = 25; // Volta ao normal
    }

    /* Status flags */
    battery_status = 0;
    if (voltage_mv < 11000) battery_status |= (1 << 0); // Undervoltage
    if (voltage_mv > 14000) battery_status |= (1 << 1); // Overvoltage
    if (soc < 20) battery_status |= (1 << 2);           // Low battery
    if (temperature > 40) battery_status |= (1 << 3);   // High temperature

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

    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[Battery] %u.%03uV | %dmA | SOC=%u%% | Temp=%d°C | Status=0x%02X\r\n",
           voltage_mv / 1000, voltage_mv % 1000,
           current_ma, soc, temperature, battery_status);
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
