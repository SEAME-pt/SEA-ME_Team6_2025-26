/*
 * emergency_stop.h
 *
 *  Created on: Jan 3, 2026
 *      Author: rcosta-c
 *
 *  Description: Emergency stop configuration for ToF-based automatic braking
 */

#ifndef INC_EMERGENCY_STOP_H_
#define INC_EMERGENCY_STOP_H_

#include <stdint.h>

/* ===========================================================================
 * EMERGENCY STOP CONFIGURATION
 * ===========================================================================
 */

/* Distance thresholds (mm) */
#define TOF_EMERGENCY_THRESHOLD_MM    100    /* Trigger emergency stop */
#define TOF_HYSTERESIS_MM             20     /* Prevent oscillation */
#define TOF_RECOVERY_THRESHOLD_MM     (TOF_EMERGENCY_THRESHOLD_MM + TOF_HYSTERESIS_MM)  /* 120mm */

/* ===========================================================================
 * EMERGENCY STOP STATES
 * ===========================================================================
 */

/**
 * @brief Emergency stop state machine states
 */
typedef enum {
    ESTOP_STATE_NORMAL = 0,      /* Normal operation (distance >= 120mm) */
    ESTOP_STATE_WARNING = 1,     /* Proximity warning (100-120mm) */
    ESTOP_STATE_EMERGENCY = 2    /* Emergency stop active (distance < 100mm) */
} EmergencyStopState_t;

/* ===========================================================================
 * LCD RGB COLORS FOR EACH STATE
 * ===========================================================================
 */

/* Normal state - White (same as LCD initialization) */
#define LCD_COLOR_NORMAL_R      255
#define LCD_COLOR_NORMAL_G      255
#define LCD_COLOR_NORMAL_B      255

/* Warning state - Orange/Yellow */
#define LCD_COLOR_WARNING_R     255
#define LCD_COLOR_WARNING_G     128
#define LCD_COLOR_WARNING_B     0

/* Emergency state - Red */
#define LCD_COLOR_EMERGENCY_R   255
#define LCD_COLOR_EMERGENCY_G   0
#define LCD_COLOR_EMERGENCY_B   0

/* ===========================================================================
 * GLOBAL STATE VARIABLE
 * ===========================================================================
 */

/**
 * @brief Global flag indicating emergency stop is active
 *
 * This variable is accessed by:
 * - ToF Thread: writes (sets/clears emergency state)
 * - CAN_RX Thread: reads (blocks joystick commands during emergency)
 *
 * Thread-safe: uint8_t is atomic on ARM Cortex-M33
 */
extern volatile uint8_t emergency_stop_active;

#endif /* INC_EMERGENCY_STOP_H_ */
