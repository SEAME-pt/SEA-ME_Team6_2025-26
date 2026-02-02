/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ism330dhcx.h
  * @brief   This file contains all the function prototypes for
  *          the ISM330DHCX IMU sensor driver (Accelerometer + Gyroscope)
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
#ifndef __ISM330DHCX_H__
#define __ISM330DHCX_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#ifdef UNIT_TESTING
/* In unit tests we use a lightweight HAL/header mock provided by tests/mocks */
#include "stm32u5xx_hal.h"
#else
#include "main.h"
#endif
#include "i2c.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* ISM330DHCX I2C Address */
#define ISM330DHCX_I2C_ADDR        (0x6B << 1)  /* 0x6B with R/W bit (SDO/SA0 = 1) */

/* ISM330DHCX Register addresses */
#define ISM330DHCX_WHO_AM_I        0x0F
#define ISM330DHCX_CTRL1_XL        0x10  /* Accelerometer control register */
#define ISM330DHCX_CTRL2_G         0x11  /* Gyroscope control register */
#define ISM330DHCX_CTRL3_C         0x12  /* Control register 3 */
#define ISM330DHCX_STATUS_REG      0x1E
#define ISM330DHCX_OUT_TEMP_L      0x20
#define ISM330DHCX_OUT_TEMP_H      0x21
#define ISM330DHCX_OUTX_L_G        0x22  /* Gyro X-axis low byte */
#define ISM330DHCX_OUTX_H_G        0x23  /* Gyro X-axis high byte */
#define ISM330DHCX_OUTY_L_G        0x24
#define ISM330DHCX_OUTY_H_G        0x25
#define ISM330DHCX_OUTZ_L_G        0x26
#define ISM330DHCX_OUTZ_H_G        0x27
#define ISM330DHCX_OUTX_L_A        0x28  /* Accel X-axis low byte */
#define ISM330DHCX_OUTX_H_A        0x29  /* Accel X-axis high byte */
#define ISM330DHCX_OUTY_L_A        0x2A
#define ISM330DHCX_OUTY_H_A        0x2B
#define ISM330DHCX_OUTZ_L_A        0x2C
#define ISM330DHCX_OUTZ_H_A        0x2D

/* ISM330DHCX WHO_AM_I value */
#define ISM330DHCX_ID              0x6B

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/* Data structures */
typedef struct {
    float x;
    float y;
    float z;
} ISM330DHCX_AxesRaw_t;

/* Function prototypes */
HAL_StatusTypeDef ISM330DHCX_Init(void);
HAL_StatusTypeDef ISM330DHCX_ReadAccel(ISM330DHCX_AxesRaw_t *accel);
HAL_StatusTypeDef ISM330DHCX_ReadGyro(ISM330DHCX_AxesRaw_t *gyro);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ISM330DHCX_H__ */
