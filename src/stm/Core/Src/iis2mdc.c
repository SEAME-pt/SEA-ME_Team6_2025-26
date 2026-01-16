/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    iis2mdc.c
  * @brief   This file provides driver for the IIS2MDC magnetometer sensor
  *          (3-axis compass)
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
#include "iis2mdc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef IIS2MDC_WriteReg(uint8_t reg, uint8_t value);
static HAL_StatusTypeDef IIS2MDC_ReadReg(uint8_t reg, uint8_t *value);
static HAL_StatusTypeDef IIS2MDC_ReadRegMulti(uint8_t reg, uint8_t *buffer, uint16_t length);

/* USER CODE BEGIN 1 */

/**
  * @brief  Write a value to an IIS2MDC register
  * @param  reg: register address
  * @param  value: value to write
  * @retval HAL status
  */
static HAL_StatusTypeDef IIS2MDC_WriteReg(uint8_t reg, uint8_t value)
{
  return HAL_I2C_Mem_Write(&hi2c2, IIS2MDC_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 100);
}

/**
  * @brief  Read a value from an IIS2MDC register
  * @param  reg: register address
  * @param  value: pointer to store the read value
  * @retval HAL status
  */
static HAL_StatusTypeDef IIS2MDC_ReadReg(uint8_t reg, uint8_t *value)
{
  return HAL_I2C_Mem_Read(&hi2c2, IIS2MDC_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, value, 1, 100);
}

/**
  * @brief  Read multiple bytes from IIS2MDC registers
  * @param  reg: starting register address
  * @param  buffer: pointer to buffer to store the read values
  * @param  length: number of bytes to read
  * @retval HAL status
  */
static HAL_StatusTypeDef IIS2MDC_ReadRegMulti(uint8_t reg, uint8_t *buffer, uint16_t length)
{
  return HAL_I2C_Mem_Read(&hi2c2, IIS2MDC_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, buffer, length, 100);
}

/**
  * @brief  Initialize the IIS2MDC magnetometer
  * @retval HAL status
  */
HAL_StatusTypeDef IIS2MDC_Init(void)
{
  HAL_StatusTypeDef status;
  uint8_t whoami;

  /* Check WHO_AM_I register */
  status = IIS2MDC_ReadReg(IIS2MDC_WHO_AM_I, &whoami);
  if (status != HAL_OK)
  {
    return status;
  }

  if (whoami != IIS2MDC_ID)
  {
    return HAL_ERROR;  /* Wrong device ID */
  }

  /* Configure CFG_REG_A:
   * MD[1:0] = 00 (continuous mode)
   * ODR[1:0] = 11 (100 Hz output data rate)
   * LP = 0 (high-resolution mode)
   * COMP_TEMP_EN = 1 (temperature compensation enabled)
   * Binary: 10001100 = 0x8C
   */
  status = IIS2MDC_WriteReg(IIS2MDC_CFG_REG_A, 0x8C);
  if (status != HAL_OK)
  {
    return status;
  }

  /* Configure CFG_REG_B:
   * OFF_CANC = 0 (offset cancellation disabled)
   * INT_on_DataOFF = 0
   * Set_FREQ = 0
   * OFF_CANC_ONE_SHOT = 0
   * Binary: 00000000 = 0x00
   */
  status = IIS2MDC_WriteReg(IIS2MDC_CFG_REG_B, 0x00);
  if (status != HAL_OK)
  {
    return status;
  }

  /* Configure CFG_REG_C:
   * BDU = 1 (Block Data Update enabled)
   * BLE = 0 (data LSB @ lower address)
   * I2C_DIS = 0 (I2C enabled)
   * INT_on_PIN = 0
   * Self_test = 0
   * Binary: 00010000 = 0x10
   */
  status = IIS2MDC_WriteReg(IIS2MDC_CFG_REG_C, 0x10);
  if (status != HAL_OK)
  {
    return status;
  }

  return HAL_OK;
}

/**
  * @brief  Read magnetometer data from IIS2MDC
  * @param  mag: pointer to store magnetic field data (in mGauss)
  * @retval HAL status
  */
HAL_StatusTypeDef IIS2MDC_ReadMag(IIS2MDC_MagData_t *mag)
{
  HAL_StatusTypeDef status;
  uint8_t buffer[6];
  int16_t raw_x, raw_y, raw_z;

  /* Read 6 bytes starting from OUTX_L (registers auto-increment) */
  status = IIS2MDC_ReadRegMulti(IIS2MDC_OUTX_L, buffer, 6);
  if (status != HAL_OK)
  {
    return status;
  }

  /* Combine low and high bytes */
  raw_x = (int16_t)((buffer[1] << 8) | buffer[0]);
  raw_y = (int16_t)((buffer[3] << 8) | buffer[2]);
  raw_z = (int16_t)((buffer[5] << 8) | buffer[4]);

  /* Convert to mGauss (±50 gauss full scale)
   * Sensitivity = 1.5 mGauss/LSB
   */
  mag->x = (float)raw_x * 1.5f;
  mag->y = (float)raw_y * 1.5f;
  mag->z = (float)raw_z * 1.5f;

  return HAL_OK;
}

/**
  * @brief  Read temperature from IIS2MDC
  * @param  temp: pointer to store temperature (in °C)
  * @retval HAL status
  * @note   Temperature sensor is used for compensation, not accurate absolute measurement
  */
HAL_StatusTypeDef IIS2MDC_ReadTemp(float *temp)
{
  HAL_StatusTypeDef status;
  uint8_t buffer[2];
  int16_t raw_temp;

  /* Read 2 bytes starting from TEMP_OUT_L */
  status = IIS2MDC_ReadRegMulti(IIS2MDC_TEMP_OUT_L, buffer, 2);
  if (status != HAL_OK)
  {
    return status;
  }

  /* Combine low and high bytes */
  raw_temp = (int16_t)((buffer[1] << 8) | buffer[0]);

  /* Convert to °C
   * Sensitivity = 8 LSB/°C
   * Offset is not specified in datasheet - using typical room temp calibration
   */
  *temp = ((float)raw_temp / 8.0f) + 25.0f;

  return HAL_OK;
}

/* USER CODE END 1 */
