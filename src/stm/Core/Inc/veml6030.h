/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    veml6030.h
  * @brief   This file contains all the function prototypes for
  *          the VEML6030 ambient light sensor driver
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
#ifndef __VEML6030_H__
#define __VEML6030_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* VEML6030 I2C Address */
/* Note: VEML6030 can be at 0x10 (ADDR=LOW) or 0x48 (ADDR=HIGH) */
#define VEML6030_I2C_ADDR       (0x10 << 1)  /* 0x10 = 0x20 shifted (CONFIRMED by I2C scan!) */

/* VEML6030 Register addresses */
#define VEML6030_ALS_CONF       0x00  /* ALS configuration register */
#define VEML6030_ALS_WH         0x01  /* ALS high threshold window setting */
#define VEML6030_ALS_WL         0x02  /* ALS low threshold window setting */
#define VEML6030_POWER_SAVING   0x03  /* Power saving mode */
#define VEML6030_ALS            0x04  /* ALS high resolution output data */
#define VEML6030_WHITE          0x05  /* White channel output data */
#define VEML6030_ALS_INT        0x06  /* ALS interrupt status */

/* ALS_CONF register bit definitions */
#define VEML6030_ALS_SD         (1 << 0)   /* ALS shut down (1=shutdown, 0=power on) */
#define VEML6030_ALS_INT_EN     (1 << 1)   /* ALS interrupt enable */
#define VEML6030_ALS_PERS_1     (0 << 4)   /* ALS persistence: 1 */
#define VEML6030_ALS_PERS_2     (1 << 4)   /* ALS persistence: 2 */
#define VEML6030_ALS_PERS_4     (2 << 4)   /* ALS persistence: 4 */
#define VEML6030_ALS_PERS_8     (3 << 4)   /* ALS persistence: 8 */
#define VEML6030_ALS_IT_25ms    (12 << 6)  /* Integration time: 25ms */
#define VEML6030_ALS_IT_50ms    (8 << 6)   /* Integration time: 50ms */
#define VEML6030_ALS_IT_100ms   (0 << 6)   /* Integration time: 100ms */
#define VEML6030_ALS_IT_200ms   (1 << 6)   /* Integration time: 200ms */
#define VEML6030_ALS_IT_400ms   (2 << 6)   /* Integration time: 400ms */
#define VEML6030_ALS_IT_800ms   (3 << 6)   /* Integration time: 800ms */
#define VEML6030_ALS_GAIN_1     (0 << 11)  /* ALS gain: 1x */
#define VEML6030_ALS_GAIN_2     (1 << 11)  /* ALS gain: 2x */
#define VEML6030_ALS_GAIN_1_8   (2 << 11)  /* ALS gain: 1/8 */
#define VEML6030_ALS_GAIN_1_4   (3 << 11)  /* ALS gain: 1/4 */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/* Function prototypes */
HAL_StatusTypeDef VEML6030_Init(void);
HAL_StatusTypeDef VEML6030_ReadALS(uint16_t *lux);
HAL_StatusTypeDef VEML6030_ReadWhite(uint16_t *white);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __VEML6030_H__ */
