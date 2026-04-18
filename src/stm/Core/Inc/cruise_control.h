/*
 * cruise_control.h
 *
 * Basic Cruise Control (V1) - ThreadX Task
 * Maintains target speed via PI controller on DC motors.
 *
 * Created on: Mar 2026
 *     Author: rcosta-c
 */

#ifndef INC_CRUISE_CONTROL_H_
#define INC_CRUISE_CONTROL_H_

#include <stdint.h>
#include <stdbool.h>


/*============================================================================
 * TUNABLE PARAMETERS — adjust these on the real car
 *============================================================================*/

/* --- PI Controller Gains --- */
#define CC_KP                       5.0f   /* Proportional gain (start 5-15, raise if sluggish, lower if oscillating) */
#define CC_KI                       3.0f    /* Integral gain (start 0.5-2, raise if steady-state error persists) */

/* --- Anti-windup: caps the integral accumulator (in throttle %) --- */
#define CC_INTEGRAL_MAX             50.0f
#define CC_INTEGRAL_MIN             0.0f    /* No negative integral — CC is forward-only */

/* --- Throttle rate limiter (max change per cycle, in %) --- */
#define CC_THROTTLE_RAMP_UP         20.0f    /* Max throttle increase per tick (smooth accel) */
#define CC_THROTTLE_RAMP_DOWN       80.0f    /* Max throttle decrease per tick (slightly faster decel) */

/* --- Throttle output limits (%) --- */
#define CC_THROTTLE_MAX             80.0f   /* Never exceed this — protects gears/motor */
#define CC_THROTTLE_MIN             0.0f    /* CC never commands reverse — decel by reducing to 0 */

/* --- Motor deadzone compensation (%) --- */
#define CC_MOTOR_DEADZONE_PCT       15.0f   /* Min throttle that reliably spins the motor (tune on real car) */

/* --- Speed thresholds (km/h) --- */
#define CC_SPEED_MIN                0.0f    /* Below this, CC refuses to activate (Hall unreliable) */
#define CC_SPEED_MAX                15.0f   /* Above this, CC refuses to activate (PiRacer safety) */
#define CC_SPEED_TOLERANCE          0.3f    /* "Close enough" band — reduces jitter near target */

/* --- Safety --- */
#define CC_SPEED_DROP_THRESHOLD     2.0f    /* If actual speed drops this much below target, override (stall/blocked) */
#define CC_STALL_TIMEOUT_MS         2000    /* If stall condition persists this long, abort */
#define CC_STALL_GRACE_MS           3000    /* Grace period after activation before stall detection kicks in */

/* --- Task timing --- */
#define CC_TASK_PERIOD_MS           50      /* PI loop rate: 20 Hz */
#define CC_STATUS_PERIOD_MS         100     /* CAN status broadcast: 10 Hz */

/* --- Wheel geometry (for RPM -> km/h conversion, if needed) --- */
#define CC_WHEEL_DIAMETER_MM        68.0f   /* Wheel diameter in mm */
#define CC_ENCODER_HOLES            18      /* Hall encoder disk holes */


/*============================================================================
 * Enums & Types
 *============================================================================*/

typedef enum {
    CC_STATE_OFF = 0,       /* Inactive, throttle controlled by MotorCmd */
    CC_STATE_ACTIVE,        /* PI loop running, CC controls throttle */
    CC_STATE_OVERRIDE       /* Temporarily suspended (brake/error), resume possible */
} CruiseControlState_t;

typedef enum {
    CC_CMD_OFF = 0,         /* Deactivate and clear target */
    CC_CMD_ACTIVATE,        /* Activate with current target */
    CC_CMD_DEACTIVATE,      /* Deactivate, keep target for resume */
    CC_CMD_SET_SPEED,       /* Set new target speed */
    CC_CMD_RESUME,          /* Resume from OVERRIDE with previous target */
    CC_CMD_INCREMENT,       /* +0.5 km/h (future V2) */
    CC_CMD_DECREMENT        /* -0.5 km/h (future V2) */
} CruiseControlCommand_t;

/* CAN frame: CC Command (0x202) - AGL -> STM32 */
typedef struct __attribute__((packed)) {
    uint8_t  command;           /* CruiseControlCommand_t */
    uint16_t target_speed;      /* 0.01 km/h units (e.g., 400 = 4.00 km/h) */
    uint8_t  reserved[3];
    uint8_t  counter;           /* Rolling 0-14 */
    uint8_t  crc;               /* CRC-8 */
} CruiseControlCmd_t;

/* CAN frame: CC Status (0x213) - STM32 -> AGL */
typedef struct __attribute__((packed)) {
    uint8_t  state;             /* CruiseControlState_t */
    uint16_t target_speed;      /* 0.01 km/h */
    uint16_t current_speed;     /* 0.01 km/h */
    int8_t   applied_throttle;  /* -100 to +100 % */
    uint8_t  counter;
    uint8_t  crc;
} CruiseControlStatus_t;

/* Internal CC context — holds all runtime state */
typedef struct {
    /* State machine */
    CruiseControlState_t state;

    /* Speed */
    float target_speed_kmh;
    float current_speed_kmh;

    /* PI controller */
    float integral;
    float last_error;
    float output_throttle;          /* Raw PI output (before ramp) */
    float applied_throttle;         /* After rate limiting — this goes to motor */

    /* Stall detection */
    uint32_t stall_timer_ms;
    uint32_t activation_grace_ms;   /* Counts up from 0 after each ACTIVATE/RESUME */

    /* CAN counter */
    uint8_t tx_counter;
} CruiseControl_t;


/*============================================================================
 * Public API
 *============================================================================*/

/* Call once from main init */
void CruiseControl_Init(CruiseControl_t *cc);

/* Called from CAN RX handler when 0x202 arrives */
void CruiseControl_ProcessCommand(CruiseControl_t *cc, const CruiseControlCmd_t *cmd);

/* Called every CC_TASK_PERIOD_MS from the CC ThreadX task */
void CruiseControl_Update(CruiseControl_t *cc, float current_speed_kmh);

/* Returns the throttle to apply (or NAN if CC is OFF) */
float CruiseControl_GetThrottle(const CruiseControl_t *cc);

/* Returns true if CC is controlling throttle */
bool CruiseControl_IsActive(const CruiseControl_t *cc);

/* Force override (called from AEB, emergency stop, brake detection) */
void CruiseControl_ForceOverride(CruiseControl_t *cc);

/* Utility: convert wheel RPM to km/h */
float CruiseControl_RpmToKmh(int16_t rpm);


#endif /* INC_CRUISE_CONTROL_H_ */
