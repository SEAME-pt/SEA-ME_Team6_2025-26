/**
 * Testable implementation of ToF sensor wrapper
 */

#include "tof_testable.h"
#include <string.h>

/* Private variables */
static uint8_t tof_initialized = 0;
static uint8_t tof_ranging_active = 0;
static ToF_ResultsData simulated_results;

/**
 * @brief  Initialize ToF sensor
 */
HAL_StatusTypeDef ToF_Init(void)
{
    if (tof_initialized) {
        return HAL_ERROR;  // Already initialized
    }

    // Simulate initialization
    tof_initialized = 1;
    tof_ranging_active = 0;

    // Clear simulated data
    memset(&simulated_results, 0, sizeof(simulated_results));
    simulated_results.nb_zones = TOF_RESOLUTION_8X8;

    return HAL_OK;
}

/**
 * @brief  Start ranging measurements
 */
HAL_StatusTypeDef ToF_StartRanging(void)
{
    if (!tof_initialized) {
        return HAL_ERROR;
    }

    if (tof_ranging_active) {
        return HAL_ERROR;  // Already ranging
    }

    tof_ranging_active = 1;
    return HAL_OK;
}

/**
 * @brief  Stop ranging measurements
 */
HAL_StatusTypeDef ToF_StopRanging(void)
{
    if (!tof_initialized) {
        return HAL_ERROR;
    }

    if (!tof_ranging_active) {
        return HAL_ERROR;  // Not ranging
    }

    tof_ranging_active = 0;
    return HAL_OK;
}

/**
 * @brief  Get ranging data
 */
HAL_StatusTypeDef ToF_GetData(ToF_ResultsData *results)
{
    if (!tof_initialized || !tof_ranging_active) {
        return HAL_ERROR;
    }

    if (results == NULL) {
        return HAL_ERROR;
    }

    // Copy simulated data to output
    memcpy(results, &simulated_results, sizeof(ToF_ResultsData));

    return HAL_OK;
}

/**
 * @brief  Find nearest valid distance in all zones
 * @param  results: Pointer to results data
 * @param  nearest_zone: Pointer to store the zone index of nearest object (can be NULL)
 * @return Minimum distance in mm (or TOF_MAX_DISTANCE_MM if no valid object)
 */
uint16_t ToF_FindNearestDistance(const ToF_ResultsData *results, int *nearest_zone)
{
    if (results == NULL) {
        return TOF_MAX_DISTANCE_MM;
    }

    uint16_t min_dist = TOF_MAX_DISTANCE_MM;
    int min_zone = -1;

    for (int i = 0; i < results->nb_zones; i++) {
        // Check if status is valid (5 or 9)
        if (!ToF_IsStatusValid(results->target_status[i])) {
            continue;
        }

        // Check if distance is positive and closer
        if (results->distance_mm[i] > 0 && results->distance_mm[i] < min_dist) {
            min_dist = results->distance_mm[i];
            min_zone = i;
        }
    }

    if (nearest_zone != NULL) {
        *nearest_zone = min_zone;
    }

    return (min_dist == TOF_MAX_DISTANCE_MM) ? 0 : min_dist;
}

/**
 * @brief  Check if target status is valid
 * @param  status: Target status value
 * @return 1 if valid (5 or 9), 0 otherwise
 */
uint8_t ToF_IsStatusValid(uint8_t status)
{
    return (status == TOF_STATUS_VALID_5) || (status == TOF_STATUS_VALID_9);
}

#ifdef UNIT_TESTING
/**
 * @brief  Reset internal state (for testing)
 */
void ToF_Reset(void)
{
    tof_initialized = 0;
    tof_ranging_active = 0;
    memset(&simulated_results, 0, sizeof(simulated_results));
    simulated_results.nb_zones = TOF_RESOLUTION_8X8;
}

/**
 * @brief  Simulate ToF data (for testing)
 */
void ToF_SimulateData(const ToF_ResultsData *simulated_data)
{
    if (simulated_data != NULL) {
        memcpy(&simulated_results, simulated_data, sizeof(ToF_ResultsData));
    }
}
#endif
