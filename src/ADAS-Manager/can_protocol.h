/*
 * can_protocol.h
 *
 * Unified CAN protocol — shared header for STM32 and AGL (ADAS Manager).
 * Byte-for-byte identical on both sides.
 *
 * Project: SEA:ME — Team 6
 *
 * Design:
 *   - 0x202 carries intent (mode + steering + throttle + target_speed + headway)
 *     from the Manager to the STM32 at 50 Hz.
 *   - STM32 is the final arbiter of throttle/steering based on `mode`.
 *   - Steering is always applied (independent of mode).
 *   - Throttle is used only in MANUAL/LKA modes.
 *   - target_speed_cms is used only in CC/ACC modes.
 *   - AEB on the STM32 has absolute override regardless of mode.
 */

#ifndef INC_CAN_PROTOCOL_H_
#define INC_CAN_PROTOCOL_H_

#include <stdint.h>

/*============================================================================
 * CAN IDs — longitudinal/lateral control
 *============================================================================*/

#define CAN_ID_CTRL_CMD       0x202   /* Manager -> STM32: unified control */
#define CAN_ID_CTRL_STATUS    0x213   /* STM32 -> Manager: actual state + ACC feedback */

/*============================================================================
 * Control Modes
 *============================================================================*/

typedef enum {
    CTRL_MODE_DISABLED = 0,
    CTRL_MODE_MANUAL   = 1,
    CTRL_MODE_LKA      = 2,
    CTRL_MODE_CC       = 3,
    CTRL_MODE_ACC      = 4,
} CtrlMode_t;

/*============================================================================
 * Headway settings (ACC only)
 *  Maps to seconds of time-headway: gap_target = headway_time * v_current
 *============================================================================*/

typedef enum {
    HEADWAY_CLOSE  = 1,   /* ~0.8s */
    HEADWAY_MEDIUM = 2,   /* ~1.5s */
    HEADWAY_FAR    = 3,   /* ~2.2s */
} HeadwaySetting_t;

/*============================================================================
 * 0x202 — Unified Control Command (Manager -> STM32)
 *  Sent every 20ms (50 Hz). CRC-8 (poly=0x07, init=0x00) over bytes 0..6.
 *============================================================================*/

typedef struct __attribute__((packed)) {
    uint8_t  mode;              /* CtrlMode_t */
    int8_t   steering;          /* -100..+100 — ALWAYS applied */
    int8_t   throttle;          /* -100..+100 — used in MANUAL/LKA only */
    uint16_t target_speed_cms;  /* cm/s, used in CC/ACC only (range 0..655) */
    uint8_t  headway       : 4; /* HeadwaySetting_t, used in ACC only */
    uint8_t  aeb_request   : 1; /* Manager requests emergency stop */
    uint8_t  reserved      : 3;
    uint8_t  counter;           /* rolling 0..14 */
    uint8_t  crc;               /* CRC-8 over bytes 0..6 */
} CtrlCmd_t;

/*============================================================================
 * 0x213 — Control Status (STM32 -> Manager)
 *  Sent every 100ms (10 Hz). 8 bytes — counter_echo dropped to fit classic CAN.
 *============================================================================*/

typedef enum {
    OVERRIDE_NONE           = 0,
    OVERRIDE_AEB            = 1,
    OVERRIDE_MANUAL_BRAKE   = 2,
    OVERRIDE_LEAD_TOO_CLOSE = 3,
    OVERRIDE_CMD_TIMEOUT    = 4,
    OVERRIDE_LIMIT_HIT      = 5,
} OverrideReason_t;

typedef struct __attribute__((packed)) {
    uint8_t  active_mode;          /* CtrlMode_t — what STM32 is actually running */
    int8_t   actual_steering;      /* -100..+100 — applied to servo */
    int8_t   actual_throttle;      /* -100..+100 — applied to TB6612 */
    uint16_t current_speed_cms;    /* cm/s, from Hall PB6 */
    uint16_t gap_cm;               /* cm, from SRF08 (0xFFFF = no lead) */
    uint8_t  override_reason;      /* OverrideReason_t */
} CtrlStatus_t;

/*============================================================================
 * Compile-time size checks — wire format must be exactly 8 bytes on both sides
 *============================================================================*/

#ifdef __cplusplus
static_assert(sizeof(CtrlCmd_t)    == 8, "CtrlCmd_t must be 8 bytes on the wire");
static_assert(sizeof(CtrlStatus_t) == 8, "CtrlStatus_t must be 8 bytes on the wire");
#else
_Static_assert(sizeof(CtrlCmd_t)    == 8, "CtrlCmd_t must be 8 bytes on the wire");
_Static_assert(sizeof(CtrlStatus_t) == 8, "CtrlStatus_t must be 8 bytes on the wire");
#endif

#endif /* INC_CAN_PROTOCOL_H_ */
