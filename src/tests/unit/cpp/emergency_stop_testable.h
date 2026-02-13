/**
  * Header testável do Emergency Stop
  */

#ifndef EMERGENCY_STOP_TESTABLE_H_
#define EMERGENCY_STOP_TESTABLE_H_

#include <stdint.h>

/**
 * @brief Emergency stop state machine states
 */
typedef enum {
    ESTOP_STATE_NORMAL = 0,      /* Normal operation (distance >= 120mm) */
    ESTOP_STATE_WARNING = 1,     /* Proximity warning (100-120mm) */
    ESTOP_STATE_EMERGENCY = 2    /* Emergency stop active (distance < 100mm) */
} EmergencyStopState_t;

/**
 * @brief Callback function type for emergency actions
 */
typedef void (*emergency_callback_t)(void);

// Global state
extern volatile uint8_t emergency_stop_active;

// Function prototypes
void Emergency_SetMotorStopCallback(emergency_callback_t callback);
void Emergency_SetEmergencyFrameCallback(emergency_callback_t callback);
void Emergency_SetClearFrameCallback(emergency_callback_t callback);

void Emergency_Init(void);  // Alias for Emergency_Reset
EmergencyStopState_t Emergency_GetState(void);
uint8_t Emergency_IsActive(void);
void Emergency_Reset(void);

EmergencyStopState_t Emergency_ProcessDistance(uint16_t distance_mm);
int Emergency_ProcessCANCommand(uint8_t active);
int Emergency_ShouldBlockMotorCommand(uint8_t has_emergency_flag);

#endif /* EMERGENCY_STOP_TESTABLE_H_ */
