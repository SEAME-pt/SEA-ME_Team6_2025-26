/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    iis2mdc.h
  * @brief   This file contains all the function prototypes for
  *          the IIS2MDC magnetometer sensor driver (3-axis compass)
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
#ifndef __IIS2MDC_H__
#define __IIS2MDC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* IIS2MDC I2C Address */
#define IIS2MDC_I2C_ADDR        (0x1E << 1)  /* 0x1E with R/W bit = 0x3C */

/* IIS2MDC Register addresses */
#define IIS2MDC_WHO_AM_I        0x4F
#define IIS2MDC_CFG_REG_A       0x60  /* Configuration register A */
#define IIS2MDC_CFG_REG_B       0x61  /* Configuration register B */
#define IIS2MDC_CFG_REG_C       0x62  /* Configuration register C */
#define IIS2MDC_STATUS_REG      0x67  /* Status register */
#define IIS2MDC_OUTX_L          0x68  /* X-axis low byte */
#define IIS2MDC_OUTX_H          0x69  /* X-axis high byte */
#define IIS2MDC_OUTY_L          0x6A  /* Y-axis low byte */
#define IIS2MDC_OUTY_H          0x6B  /* Y-axis high byte */
#define IIS2MDC_OUTZ_L          0x6C  /* Z-axis low byte */
#define IIS2MDC_OUTZ_H          0x6D  /* Z-axis high byte */
#define IIS2MDC_TEMP_OUT_L      0x6E  /* Temperature low byte */
#define IIS2MDC_TEMP_OUT_H      0x6F  /* Temperature high byte */

/* IIS2MDC WHO_AM_I value */
#define IIS2MDC_ID              0x40

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/* Data structures */
typedef struct {
    float x;  /* Magnetic field X-axis (mGauss) */
    float y;  /* Magnetic field Y-axis (mGauss) */
    float z;  /* Magnetic field Z-axis (mGauss) */
} IIS2MDC_MagData_t;

/* Function prototypes */
HAL_StatusTypeDef IIS2MDC_Init(void);
HAL_StatusTypeDef IIS2MDC_ReadMag(IIS2MDC_MagData_t *mag);
HAL_StatusTypeDef IIS2MDC_ReadTemp(float *temp);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __IIS2MDC_H__ */
