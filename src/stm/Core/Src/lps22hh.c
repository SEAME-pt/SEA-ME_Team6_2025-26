/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    lps22hh.c
  * @brief   This file provides driver for the LPS22HH temperature and
  *          pressure sensor
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
#include "lps22hh.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef LPS22HH_WriteReg(uint8_t reg, uint8_t value);
static HAL_StatusTypeDef LPS22HH_ReadReg(uint8_t reg, uint8_t *value);
static HAL_StatusTypeDef LPS22HH_ReadRegMulti(uint8_t reg, uint8_t *buffer, uint16_t length);

/* USER CODE BEGIN 1 */

/**
  * @brief  Write a value to a LPS22HH register
  * @param  reg: register address
  * @param  value: value to write
  * @retval HAL status
  */
static HAL_StatusTypeDef LPS22HH_WriteReg(uint8_t reg, uint8_t value)
{
  return HAL_I2C_Mem_Write(&hi2c2, LPS22HH_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 100);
}

/**
  * @brief  Read a value from a LPS22HH register
  * @param  reg: register address
  * @param  value: pointer to store the read value
  * @retval HAL status
  */
static HAL_StatusTypeDef LPS22HH_ReadReg(uint8_t reg, uint8_t *value)
{
  return HAL_I2C_Mem_Read(&hi2c2, LPS22HH_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, value, 1, 100);
}

/**
  * @brief  Read multiple bytes from LPS22HH registers
  * @param  reg: starting register address
  * @param  buffer: pointer to buffer to store the read values
  * @param  length: number of bytes to read
  * @retval HAL status
  */
static HAL_StatusTypeDef LPS22HH_ReadRegMulti(uint8_t reg, uint8_t *buffer, uint16_t length)
{
  return HAL_I2C_Mem_Read(&hi2c2, LPS22HH_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, buffer, length, 100);
}

/**
  * @brief  Initialize the LPS22HH sensor
  * @retval HAL status
  */
HAL_StatusTypeDef LPS22HH_Init(void)
{
  HAL_StatusTypeDef status;
  uint8_t whoami;

  /* Check WHO_AM_I register */
  status = LPS22HH_ReadReg(LPS22HH_WHO_AM_I, &whoami);
  if (status != HAL_OK)
  {
    return status;
  }

  if (whoami != LPS22HH_ID)
  {
    return HAL_ERROR;  /* Wrong device ID */
  }

  /* Configure CTRL_REG1: ODR = 10 Hz, Low-pass filter enabled */
  /* ODR[2:0] = 011 (10 Hz), EN_LPFP = 1 */
  status = LPS22HH_WriteReg(LPS22HH_CTRL_REG1, 0x3A);
  if (status != HAL_OK)
  {
    return status;
  }

  /* Configure CTRL_REG2: One-shot disabled, I2C enabled */
  status = LPS22HH_WriteReg(LPS22HH_CTRL_REG2, 0x10);
  if (status != HAL_OK)
  {
    return status;
  }

  return HAL_OK;
}

/**
  * @brief  Read temperature from LPS22HH
  * @param  temperature: pointer to store temperature in degrees Celsius
  * @retval HAL status
  */
HAL_StatusTypeDef LPS22HH_ReadTemperature(float *temperature)
{
  HAL_StatusTypeDef status;
  uint8_t buffer[2];
  int16_t temp_raw;

  /* Read temperature registers (TEMP_OUT_L and TEMP_OUT_H) */
  status = LPS22HH_ReadRegMulti(LPS22HH_TEMP_OUT_L, buffer, 2);
  if (status != HAL_OK)
  {
    return status;
  }

  /* Combine low and high bytes */
  temp_raw = (int16_t)((buffer[1] << 8) | buffer[0]);

  /* Convert to degrees Celsius */
  /* Temperature in °C = raw_value / 100 */
  *temperature = (float)temp_raw / 100.0f;

  return HAL_OK;
}

/**
  * @brief  Read pressure from LPS22HH
  * @param  pressure: pointer to store pressure in hPa (mbar)
  * @retval HAL status
  */
HAL_StatusTypeDef LPS22HH_ReadPressure(float *pressure)
{
  HAL_StatusTypeDef status;
  uint8_t buffer[3];
  int32_t press_raw;

  /* Read pressure registers (PRESS_OUT_XL, PRESS_OUT_L, PRESS_OUT_H) */
  status = LPS22HH_ReadRegMulti(LPS22HH_PRESS_OUT_XL, buffer, 3);
  if (status != HAL_OK)
  {
    return status;
  }

  /* Combine three bytes */
  press_raw = (int32_t)((buffer[2] << 16) | (buffer[1] << 8) | buffer[0]);

  /* Sign extend if negative (24-bit to 32-bit) */
  if (press_raw & 0x00800000)
  {
    press_raw |= 0xFF000000;
  }

  /* Convert to hPa (mbar) */
  /* Pressure in hPa = raw_value / 4096 */
  *pressure = (float)press_raw / 4096.0f;

  return HAL_OK;
}

/* USER CODE END 1 */
