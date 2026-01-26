/**
  ******************************************************************************
  * @file    vl53l5cx_platform_stm32.c
  * @brief   STM32-specific GPIO control implementation for VL53L5CX sensor
  ******************************************************************************
  */

#include "vl53l5cx_platform_stm32.h"
VL53L5CX_Configuration Dev;

/**
 * @brief Set XSHUT pin state
 * @param state 0=LOW (sensor in shutdown), 1=HIGH (sensor powered)
 */
void VL53L5CX_SetXSHUT(uint8_t state)
{
	HAL_GPIO_WritePin(Mems_VL53_xshut_GPIO_Port, Mems_VL53_xshut_Pin,
	                  state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * @brief Read interrupt pin state
 * @return 0=LOW, 1=HIGH
 */
uint8_t VL53L5CX_GetInterruptState(void)
{
	return HAL_GPIO_ReadPin(Mems_VLX_GPIO_GPIO_Port, Mems_VLX_GPIO_Pin);
}

/**
 * @brief Power on sensor with proper sequencing
 * @note Datasheet recommends: XSHUT LOW for 10ms, then HIGH, then wait 1.2ms+ for boot
 */
void VL53L5CX_PowerOn(void)
{
	/* Ensure sensor is in shutdown */
	VL53L5CX_SetXSHUT(0);
	HAL_Delay(10);

	/* Bring sensor out of shutdown */
	VL53L5CX_SetXSHUT(1);
	HAL_Delay(10);  /* Wait for sensor boot (datasheet: 1.2ms typ, using 10ms for margin) */
}

/**
 * @brief Power off sensor (low power mode)
 */
void VL53L5CX_PowerOff(void)
{
	VL53L5CX_SetXSHUT(0);  /* Put sensor in shutdown (low power) */
}
