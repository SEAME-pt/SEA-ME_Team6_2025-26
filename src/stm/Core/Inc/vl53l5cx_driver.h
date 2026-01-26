/**
  ******************************************************************************
  * @file    vl53l5cx_driver.h
  * @brief   VL53L5CX Time-of-Flight sensor driver wrapper
  * @details High-level API wrapping the official ST VL53L5CX ULD library
  *          Configured for 4x4 resolution (16 zones) with interrupt mode
  ******************************************************************************
  */

#ifndef VL53L5CX_DRIVER_H
#define VL53L5CX_DRIVER_H

#include "vl53l5cx_api.h"
#include "platform.h"
#include "stm32u5xx_hal.h"
#include <stdint.h>

/* Note: VL53L5CX_RESOLUTION_4X4 and VL53L5CX_RESOLUTION_8X8 are already defined in vl53l5cx_api.h */

/* Driver structure */
typedef struct {
	VL53L5CX_Configuration config;  /* Official API configuration (contains platform inside) */
	uint8_t resolution;             /* 4x4 or 8x8 */
	uint8_t ranging_mode;           /* Continuous or autonomous */
	uint8_t integration_time_ms;    /* Integration time */
	uint8_t is_initialized;         /* Initialization flag */
} VL53L5CX_Driver_t;

/* Global driver instance */
extern VL53L5CX_Driver_t vl53l5cx_driver;

/**
 * @brief Initialize VL53L5CX sensor
 * @return HAL_OK if successful, HAL_ERROR otherwise
 * @note This function takes 1-2 seconds due to firmware upload
 */
HAL_StatusTypeDef VL53L5CX_Init(void);

/**
 * @brief Configure interrupt mode for data ready
 * @return HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef VL53L5CX_ConfigureInterrupt(void);

/**
 * @brief Start continuous ranging
 * @return HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef VL53L5CX_StartRanging(void);

/**
 * @brief Stop ranging
 * @return HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef VL53L5CX_StopRanging(void);

/**
 * @brief Check if data is ready
 * @param is_ready Pointer to store result (0=not ready, 1=ready)
 * @return HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef VL53L5CX_GetDataReady(uint8_t *is_ready);

/**
 * @brief Get full ranging results (all zones)
 * @param results Pointer to VL53L5CX_ResultsData structure
 * @return HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef VL53L5CX_GetResults(VL53L5CX_ResultsData *results);

/**
 * @brief Read distance data for all 16 zones (4x4 mode)
 * @param distances Array of 16 uint16_t to store distances in mm
 * @return HAL_OK if successful, HAL_ERROR otherwise
 */
HAL_StatusTypeDef VL53L5CX_ReadDistanceData4x4(uint16_t distances[16]);

/**
 * @brief Read single distance value (average of center zones)
 * @param distance_mm Pointer to store distance in millimeters
 * @return HAL_OK if successful, HAL_ERROR otherwise
 * @note Returns average of center 4 zones (5,6,9,10) for best accuracy
 */
HAL_StatusTypeDef VL53L5CX_ReadDistance(uint16_t *distance_mm);

/**
 * @brief Clear interrupt (compatibility function)
 * @return HAL_OK
 * @note With official API, reading data automatically clears interrupt
 */
HAL_StatusTypeDef VL53L5CX_ClearInterrupt(void);

/**
 * @brief Test function for VL53L5CX sensor (debug only)
 * @return HAL_OK if all tests pass, HAL_ERROR otherwise
 * @note Call this from main() before ThreadX starts for isolated testing
 */
HAL_StatusTypeDef VL53L5CX_TestSensor(void);

#endif /* VL53L5CX_DRIVER_H */
