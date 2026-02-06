/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.h
  * @author  MCD Application Team
  * @brief   ThreadX applicative header file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_THREADX_H
#define __APP_THREADX_H
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "tx_api.h"

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

#include "system_ctx.h"
#include "sys_helpers.h"
#include "tasks/task_heartbeat.h"
#include "tasks/task_environment.h"
#include "tasks/task_speed.h"
#include "tasks/task_imu.h"
#include "tasks/task_tof.h"
#include "tasks/task_srf08.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Main thread defines -------------------------------------------------------*/
/* USER CODE BEGIN MTD */

/* USER CODE END MTD */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
UINT App_ThreadX_Init(VOID *memory_ptr);
void MX_ThreadX_Init(void);
void valueNotSetted(ULONG thread_input);

/* USER CODE BEGIN EFP */

/* External semaphore for VL53L5CX data ready interrupt */
extern TX_SEMAPHORE vl53l5cx_data_ready_semaphore;

/* USER CODE END EFP */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif /* __APP_THREADX_H */
