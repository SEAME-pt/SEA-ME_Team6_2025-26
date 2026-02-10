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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* LCD1602 Control - Set to 1 to enable LCD, 0 to disable */
#define ENABLE_LCD 0

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
//TX_MUTEX printf_mutex;

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
//static volatile uint16_t shared_srf08_distance = 0;
static volatile float shared_speed = 0.0f;

// DAVID - preciso de tirar isto daqui e colocar no system state
volatile uint8_t emergency_stop_active = 0; // Flag global de emergência (1=ativo, 0=normal)
volatile uint8_t srf08_speed_limit = 100; // Limite de velocidade imposto pelo SRF08 (0-100%)

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
static void Environment_Thread_Entry(ULONG thread_input);
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

  system_ctx_init();

  /* Create mutex for printf protection */
  //if (tx_mutex_create(&printf_mutex, "Printf Mutex", TX_NO_INHERIT) != TX_SUCCESS)
  //{
  //  ret = TX_MUTEX_ERROR;
  //}

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
                       Environment_Thread_Entry,
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
/**
  * @brief  HeartBeat thread entry function
  * @param  thread_input: thread input parameter (not used)
  * @retval None
  */
static void HeartBeat_Thread_Entry(ULONG thread_input)
{
  (void)thread_input;

  SystemCtx* ctx = system_ctx();

  task_heartbeat_init(ctx);

  while (1)
  {
      task_heartbeat_step(ctx);
      tx_thread_sleep(CAN_PERIOD_HEARTBEAT_MS);
  }
}


/**
  * @brief  Environment thread entry function
  * @param  thread_input: thread input parameter (not used)
  * @retval None
  */

static void Environment_Thread_Entry(ULONG thread_input)
{
  (void)thread_input;

    SystemCtx* ctx = system_ctx();

    task_environment_init(ctx);

    while (1) {
        task_environment_step(ctx);
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

    SystemCtx* ctx = system_ctx();
    task_speed_init(ctx);

    while (1) {
        task_speed_step(ctx);
        /* IMPORTANT:
           Since task_speed_step() already waits CAN_PERIOD_WHEEL_SPEED_MS * 1ms internally,
           do NOT sleep again here. */
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

  SystemCtx* ctx = system_ctx();
  task_can_rx_init(ctx);

  for (;;)
  {
    task_can_rx_step(ctx);
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

    SystemCtx* ctx = system_ctx();
    task_imu_init(ctx);

    while (1) {
        task_imu_step(ctx);
        /* NOTE: task_imu_step already sleeps to match CAN_PERIOD_IMU_FAST_MS */
    }
}

/**
  * @brief  ToF thread entry function
  * @param  thread_input: thread input parameter (not used)
  * @retval None
  * @note   Emergency stop has been moved to SRF08 thread
  */
static void ToF_Thread_Entry(ULONG thread_input)
{
  (void)thread_input;

    SystemCtx* ctx = system_ctx();
    task_tof_init(ctx);

    while (1) {
        task_tof_step(ctx);
        /* NOTE: task_tof_step already sleeps (~15Hz). Do not sleep here too. */
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

    SystemCtx* ctx = system_ctx();
    task_srf08_init(ctx);

    while (1) {
        task_srf08_step(ctx);
        /* No extra sleep here: task_srf08_step already waits SRF08_PERIOD_MS */
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

    SystemCtx* ctx = system_ctx();
    task_battery_init(ctx);

    while (1) {
        task_battery_step(ctx); /* step sleeps CAN_PERIOD_BATTERY_MS */
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
