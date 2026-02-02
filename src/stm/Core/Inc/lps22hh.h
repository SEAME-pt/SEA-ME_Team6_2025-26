/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    lps22hh.h
  * @brief   This file contains all the function prototypes for
  *          the LPS22HH temperature and pressure sensor driver
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
#ifndef __LPS22HH_H__
#define __LPS22HH_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#ifdef UNIT_TESTING
/* In unit tests include the lightweight HAL mock so we don't pull platform headers */
#include "stm32u5xx_hal.h"
#else
#include "main.h"
#endif
#include "i2c.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* LPS22HH I2C Address */
#define LPS22HH_I2C_ADDR        (0x5D << 1)  /* 0x5D with R/W bit */

/* LPS22HH Register addresses */
#define LPS22HH_WHO_AM_I        0x0F
#define LPS22HH_CTRL_REG1       0x10
#define LPS22HH_CTRL_REG2       0x11
#define LPS22HH_CTRL_REG3       0x12
#define LPS22HH_STATUS          0x27
#define LPS22HH_PRESS_OUT_XL    0x28
#define LPS22HH_PRESS_OUT_L     0x29
#define LPS22HH_PRESS_OUT_H     0x2A
#define LPS22HH_TEMP_OUT_L      0x2B
#define LPS22HH_TEMP_OUT_H      0x2C

/* LPS22HH WHO_AM_I value */
#define LPS22HH_ID              0xB3

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/* Function prototypes */
HAL_StatusTypeDef LPS22HH_Init(void);
HAL_StatusTypeDef LPS22HH_ReadTemperature(float *temperature);
HAL_StatusTypeDef LPS22HH_ReadPressure(float *pressure);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __LPS22HH_H__ */
