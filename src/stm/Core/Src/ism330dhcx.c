/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ism330dhcx.c
  * @brief   This file provides driver for the ISM330DHCX IMU sensor
  *          (Accelerometer + Gyroscope)
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
#include "ism330dhcx.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef ISM330DHCX_WriteReg(uint8_t reg, uint8_t value);
static HAL_StatusTypeDef ISM330DHCX_ReadReg(uint8_t reg, uint8_t *value);
static HAL_StatusTypeDef ISM330DHCX_ReadRegMulti(uint8_t reg, uint8_t *buffer, uint16_t length);

/* USER CODE BEGIN 1 */

/**
  * @brief  Write a value to an ISM330DHCX register
  * @param  reg: register address
  * @param  value: value to write
  * @retval HAL status
  */
static HAL_StatusTypeDef ISM330DHCX_WriteReg(uint8_t reg, uint8_t value)
{
  return HAL_I2C_Mem_Write(&hi2c2, ISM330DHCX_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 100);
}

/**
  * @brief  Read a value from an ISM330DHCX register
  * @param  reg: register address
  * @param  value: pointer to store the read value
  * @retval HAL status
  */
static HAL_StatusTypeDef ISM330DHCX_ReadReg(uint8_t reg, uint8_t *value)
{
  return HAL_I2C_Mem_Read(&hi2c2, ISM330DHCX_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, value, 1, 100);
}

/**
  * @brief  Read multiple bytes from ISM330DHCX registers
  * @param  reg: starting register address
  * @param  buffer: pointer to buffer to store the read values
  * @param  length: number of bytes to read
  * @retval HAL status
  */
static HAL_StatusTypeDef ISM330DHCX_ReadRegMulti(uint8_t reg, uint8_t *buffer, uint16_t length)
{
  return HAL_I2C_Mem_Read(&hi2c2, ISM330DHCX_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, buffer, length, 100);
}

/**
  * @brief  Initialize the ISM330DHCX sensor
  * @retval HAL status
  */
HAL_StatusTypeDef ISM330DHCX_Init(void)
{
  HAL_StatusTypeDef status;
  uint8_t whoami;

  /* Check WHO_AM_I register */
  status = ISM330DHCX_ReadReg(ISM330DHCX_WHO_AM_I, &whoami);
  if (status != HAL_OK)
  {
    return status;
  }

  if (whoami != ISM330DHCX_ID)
  {
    return HAL_ERROR;  /* Wrong device ID */
  }

  /* Configure CTRL3_C: Enable BDU (Block Data Update) and auto-increment */
  status = ISM330DHCX_WriteReg(ISM330DHCX_CTRL3_C, 0x44);
  if (status != HAL_OK)
  {
    return status;
  }

  /* Configure CTRL1_XL: Accelerometer
   * ODR = 208 Hz (0110), FS = ±4g (10), LPF2_XL_EN = 0
   * Binary: 01101000 = 0x68
   */
  status = ISM330DHCX_WriteReg(ISM330DHCX_CTRL1_XL, 0x68);
  if (status != HAL_OK)
  {
    return status;
  }

  /* Configure CTRL2_G: Gyroscope
   * ODR = 208 Hz (0110), FS = ±500 dps (0100)
   * Binary: 01100100 = 0x64
   */
  status = ISM330DHCX_WriteReg(ISM330DHCX_CTRL2_G, 0x64);
  if (status != HAL_OK)
  {
    return status;
  }

  return HAL_OK;
}

/**
  * @brief  Read accelerometer data from ISM330DHCX
  * @param  accel: pointer to store accelerometer data (in g)
  * @retval HAL status
  */
HAL_StatusTypeDef ISM330DHCX_ReadAccel(ISM330DHCX_AxesRaw_t *accel)
{
  HAL_StatusTypeDef status;
  uint8_t buffer[6];
  int16_t raw_x, raw_y, raw_z;

  /* Read 6 bytes starting from OUTX_L_A (auto-increment enabled) */
  status = ISM330DHCX_ReadRegMulti(ISM330DHCX_OUTX_L_A, buffer, 6);
  if (status != HAL_OK)
  {
    return status;
  }

  /* Combine low and high bytes */
  raw_x = (int16_t)((buffer[1] << 8) | buffer[0]);
  raw_y = (int16_t)((buffer[3] << 8) | buffer[2]);
  raw_z = (int16_t)((buffer[5] << 8) | buffer[4]);

  /* Convert to g (±4g full scale)
   * Sensitivity = 0.122 mg/LSB = 0.000122 g/LSB
   */
  accel->x = (float)raw_x * 0.000122f;
  accel->y = (float)raw_y * 0.000122f;
  accel->z = (float)raw_z * 0.000122f;

  return HAL_OK;
}

/**
  * @brief  Read gyroscope data from ISM330DHCX
  * @param  gyro: pointer to store gyroscope data (in dps)
  * @retval HAL status
  */
HAL_StatusTypeDef ISM330DHCX_ReadGyro(ISM330DHCX_AxesRaw_t *gyro)
{
  HAL_StatusTypeDef status;
  uint8_t buffer[6];
  int16_t raw_x, raw_y, raw_z;

  /* Read 6 bytes starting from OUTX_L_G (auto-increment enabled) */
  status = ISM330DHCX_ReadRegMulti(ISM330DHCX_OUTX_L_G, buffer, 6);
  if (status != HAL_OK)
  {
    return status;
  }

  /* Combine low and high bytes */
  raw_x = (int16_t)((buffer[1] << 8) | buffer[0]);
  raw_y = (int16_t)((buffer[3] << 8) | buffer[2]);
  raw_z = (int16_t)((buffer[5] << 8) | buffer[4]);

  /* Convert to dps (±500 dps full scale)
   * Sensitivity = 17.50 mdps/LSB = 0.0175 dps/LSB
   */
  gyro->x = (float)raw_x * 0.0175f;
  gyro->y = (float)raw_y * 0.0175f;
  gyro->z = (float)raw_z * 0.0175f;

  return HAL_OK;
}

/* USER CODE END 1 */
