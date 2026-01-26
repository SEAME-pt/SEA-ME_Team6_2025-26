/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    veml6030.c
  * @brief   This file provides driver for the VEML6030 ambient light sensor
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
#include "veml6030.h"
#include <stdio.h>  /* For printf debugging */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* Private variables */
static float veml6030_resolution = 0.0288f;  /* lux/count for gain=1, IT=100ms */

/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef VEML6030_WriteReg(uint8_t reg, uint16_t value);
static HAL_StatusTypeDef VEML6030_ReadReg(uint8_t reg, uint16_t *value);

/* USER CODE BEGIN 1 */

/**
  * @brief  Write a 16-bit value to a VEML6030 register
  * @param  reg: register address
  * @param  value: 16-bit value to write
  * @retval HAL status
  */
static HAL_StatusTypeDef VEML6030_WriteReg(uint8_t reg, uint16_t value)
{
  uint8_t data[2];
  data[0] = (uint8_t)(value & 0xFF);        /* LSB first */
  data[1] = (uint8_t)((value >> 8) & 0xFF); /* MSB second */

  return HAL_I2C_Mem_Write(&hi2c2, VEML6030_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 2, 100);
}

/**
  * @brief  Read a 16-bit value from a VEML6030 register
  * @param  reg: register address
  * @param  value: pointer to store the 16-bit read value
  * @retval HAL status
  */
static HAL_StatusTypeDef VEML6030_ReadReg(uint8_t reg, uint16_t *value)
{
  HAL_StatusTypeDef status;
  uint8_t data[2];

  status = HAL_I2C_Mem_Read(&hi2c2, VEML6030_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 2, 100);
  if (status == HAL_OK)
  {
    *value = (uint16_t)(data[0] | (data[1] << 8));  /* LSB first */
  }

  return status;
}

/**
  * @brief  Initialize the VEML6030 ambient light sensor
  * @retval HAL status
  */
HAL_StatusTypeDef VEML6030_Init(void)
{
  HAL_StatusTypeDef status;
  uint16_t config;

  /* Configure ALS_CONF register */
  config = VEML6030_ALS_IT_100ms | VEML6030_ALS_GAIN_1 | VEML6030_ALS_PERS_1;

  status = VEML6030_WriteReg(VEML6030_ALS_CONF, config);
  if (status != HAL_OK)
  {
    return status;
  }

  /* Power saving mode: disabled (0x0000) */
  status = VEML6030_WriteReg(VEML6030_POWER_SAVING, 0x0000);
  if (status != HAL_OK)
  {
    return status;
  }

  veml6030_resolution = 0.0288f;  /* Gain=1x, IT=100ms */

  /* Wait for sensor to stabilize - REMOVED: HAL_Delay causes issues in RTOS context */
  /* HAL_Delay(10); */

  return HAL_OK;
}

/**
  * @brief  Read ambient light sensor data from VEML6030
  * @param  lux: pointer to store ambient light level (in lux)
  * @retval HAL status
  */
HAL_StatusTypeDef VEML6030_ReadALS(uint16_t *lux)
{
  HAL_StatusTypeDef status;
  uint16_t als_counts;

  /* Read ALS data register */
  status = VEML6030_ReadReg(VEML6030_ALS, &als_counts);
  if (status != HAL_OK)
  {
    return status;
  }

  /* Convert counts to lux using resolution
   * lux = counts × resolution
   */
  *lux = (uint16_t)((float)als_counts * veml6030_resolution);

  return HAL_OK;
}

/**
  * @brief  Read white channel data from VEML6030
  * @param  white: pointer to store white channel counts
  * @retval HAL status
  */
HAL_StatusTypeDef VEML6030_ReadWhite(uint16_t *white)
{
  HAL_StatusTypeDef status;

  /* Read WHITE data register */
  status = VEML6030_ReadReg(VEML6030_WHITE, white);

  return status;
}

/* USER CODE END 1 */
