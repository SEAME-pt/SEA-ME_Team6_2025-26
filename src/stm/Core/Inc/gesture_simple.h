/*
 * gesture_simple.h
 * Simple gesture detection for VL53L5CX 8x8 ToF sensor
 * Created on: Jan 27, 2026
 * Author: rcosta-c
 *
 * Detects basic gestures:
 * - SWIPE_LEFT, SWIPE_RIGHT
 * - SWIPE_UP, SWIPE_DOWN
 * - TAP (approach + retreat)
 */

#ifndef GESTURE_SIMPLE_H
#define GESTURE_SIMPLE_H

#include <stdint.h>
#include <stdbool.h>

/* Gesture types */
typedef enum {
    GESTURE_NONE       = 0,
    GESTURE_SWIPE_LEFT = 1,
    GESTURE_SWIPE_RIGHT= 2,
    GESTURE_SWIPE_UP   = 3,
    GESTURE_SWIPE_DOWN = 4,
    GESTURE_TAP        = 5,
    GESTURE_DOUBLE_TAP = 6,
    GESTURE_AWAY       = 7
} GestureType_t;

/* Hand position in the 8x8 grid */
typedef struct {
    float x;           /* X position (0-7, left to right) */
    float y;           /* Y position (0-7, top to bottom) */
    float z;           /* Z distance in mm */
    bool found;        /* Hand was detected */
    uint32_t timestamp;/* Detection timestamp */
} HandPosition_t;

/* Gesture detection result */
typedef struct {
    GestureType_t type;     /* Detected gesture */
    bool ready;             /* Gesture is ready to be used */
    float speed_x;          /* Speed in X direction (zones/sec) */
    float speed_y;          /* Speed in Y direction (zones/sec) */
    float speed_z;          /* Speed in Z direction (mm/sec) */
    uint32_t start_time;    /* When gesture started */
    uint32_t end_time;      /* When gesture ended */
} GestureResult_t;

/* Gesture detector state */
typedef struct {
    HandPosition_t prev_hand;       /* Previous hand position */
    HandPosition_t curr_hand;       /* Current hand position */
    GestureResult_t gesture;        /* Current gesture result */

    /* Tracking for TAP detection */
    bool tap_approaching;           /* Hand is approaching */
    uint16_t tap_min_distance;      /* Minimum distance during tap */
    uint32_t tap_start_time;        /* When tap started */

    /* Configuration */
    uint16_t max_detection_mm;      /* Max distance to detect hand (default 400mm) */
    uint16_t min_swipe_speed;       /* Min speed for swipe (zones/sec) */
    uint16_t min_tap_speed_z;       /* Min Z speed for tap (mm/sec) */
} GestureDetector_t;

/* Configuration defaults */
#define GESTURE_MAX_DETECTION_MM    600
#define GESTURE_MIN_SWIPE_SPEED     2.0f    /* zones per second */
#define GESTURE_MIN_TAP_SPEED_Z     200.0f  /* mm per second */
#define GESTURE_TAP_MAX_TIME_MS     500     /* max tap duration */

/**
 * @brief Initialize gesture detector
 * @param detector Pointer to detector structure
 */
void Gesture_Init(GestureDetector_t *detector);

/**
 * @brief Process new ToF data and detect gestures
 * @param detector Pointer to detector structure
 * @param distance_mm Array of 64 distances (8x8 grid)
 * @param status Array of 64 target status values
 * @param timestamp Current timestamp in ms
 * @return Detected gesture type (GESTURE_NONE if no gesture)
 */
GestureType_t Gesture_Process(GestureDetector_t *detector,
                               int16_t *distance_mm,
                               uint8_t *status,
                               uint32_t timestamp);

/**
 * @brief Get current gesture result
 * @param detector Pointer to detector structure
 * @return Pointer to gesture result
 */
GestureResult_t* Gesture_GetResult(GestureDetector_t *detector);

/**
 * @brief Get current hand position
 * @param detector Pointer to detector structure
 * @return Pointer to hand position
 */
HandPosition_t* Gesture_GetHand(GestureDetector_t *detector);

/**
 * @brief Convert gesture type to string
 * @param type Gesture type
 * @return String representation
 */
const char* Gesture_TypeToString(GestureType_t type);

#endif /* GESTURE_SIMPLE_H */
