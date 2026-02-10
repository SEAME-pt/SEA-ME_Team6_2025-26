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
