/**
 * Testable wrapper for VL53L5CX ToF Sensor
 * Simplified interface for unit testing
 */

#ifndef TOF_TESTABLE_H_
#define TOF_TESTABLE_H_

#include "stm32u5xx_hal.h"
#include <stdint.h>

/* Configuration */
#define TOF_RESOLUTION_8X8       64
#define TOF_MAX_ZONES            64
#define TOF_MAX_DISTANCE_MM      4000

/* Target Status Values */
#define TOF_STATUS_VALID_5       5
#define TOF_STATUS_VALID_9       9
#define TOF_STATUS_INVALID       255

/* Emergency Thresholds (from emergency_stop.h) */
#define TOF_THRESHOLD_EMERGENCY  100   // < 100mm = EMERGENCY
#define TOF_THRESHOLD_WARNING    200   // < 200mm = WARNING

/* Results Structure (simplified from VL53L5CX_ResultsData) */
typedef struct {
    int16_t  distance_mm[TOF_MAX_ZONES];      // Distance for each zone
    uint8_t  target_status[TOF_MAX_ZONES];    // Status for each zone
    uint8_t  nb_zones;                         // Number of zones (64 for 8x8)
} ToF_ResultsData;

/* Function Prototypes */
HAL_StatusTypeDef ToF_Init(void);
HAL_StatusTypeDef ToF_StartRanging(void);
HAL_StatusTypeDef ToF_StopRanging(void);
HAL_StatusTypeDef ToF_GetData(ToF_ResultsData *results);
uint16_t ToF_FindNearestDistance(const ToF_ResultsData *results, int *nearest_zone);
uint8_t ToF_IsStatusValid(uint8_t status);

/* Test Helper - Reset internal state */
#ifdef UNIT_TESTING
void ToF_Reset(void);
void ToF_SimulateData(const ToF_ResultsData *simulated_data);
#endif

#endif /* TOF_TESTABLE_H_ */
