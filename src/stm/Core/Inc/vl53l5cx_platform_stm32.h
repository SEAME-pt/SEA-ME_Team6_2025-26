/**
  ******************************************************************************
  * @file    vl53l5cx_platform_stm32.h
  * @brief   STM32-specific GPIO control functions for VL53L5CX sensor
  * @details Provides XSHUT (power enable) and interrupt pin control
  ******************************************************************************
  */

#ifndef VL53L5CX_PLATFORM_STM32_H
#define VL53L5CX_PLATFORM_STM32_H

#include "main.h"
#include "platform.h"
#include "i2c.h"
#include "gpio.h"
#include <stdio.h>
#include "vl53l5cx_api.h"
#include "stm32u5xx_hal.h"
/**
 * @brief Set XSHUT pin state
 * @param state 0=LOW (sensor in shutdown), 1=HIGH (sensor powered)
 */
void VL53L5CX_SetXSHUT(uint8_t state);

/**
 * @brief Read interrupt pin state
 * @return 0=LOW, 1=HIGH
 */
uint8_t VL53L5CX_GetInterruptState(void);

/**
 * @brief Power on sensor with proper sequencing
 * @note Performs XSHUT toggle with appropriate delays
 */
void VL53L5CX_PowerOn(void);

/**
 * @brief Power off sensor (low power mode)
 */
void VL53L5CX_PowerOff(void);

#endif /* VL53L5CX_PLATFORM_STM32_H */
