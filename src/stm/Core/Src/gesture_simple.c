/*
 * gesture_simple.c
 * Simple gesture detection for VL53L5CX 8x8 ToF sensor
 * Created on: Jan 27, 2026
 * Author: rcosta-c
 */

#include "gesture_simple.h"
#include <string.h>
#include <math.h>

/* Valid target status values (from VL53L5CX datasheet) */
#define STATUS_VALID_1  5
#define STATUS_VALID_2  9

/* 8x8 Grid layout:
 *   0  1  2  3  4  5  6  7
 *   8  9 10 11 12 13 14 15
 *  16 17 18 19 20 21 22 23
 *  24 25 26 27 28 29 30 31
 *  32 33 34 35 36 37 38 39
 *  40 41 42 43 44 45 46 47
 *  48 49 50 51 52 53 54 55
 *  56 57 58 59 60 61 62 63
 */

/**
 * @brief Initialize gesture detector with default values
 */
void Gesture_Init(GestureDetector_t *detector)
{
    if (detector == NULL) return;

    memset(detector, 0, sizeof(GestureDetector_t));

    detector->max_detection_mm = GESTURE_MAX_DETECTION_MM;
    detector->min_swipe_speed = GESTURE_MIN_SWIPE_SPEED;
    detector->min_tap_speed_z = GESTURE_MIN_TAP_SPEED_Z;

    detector->gesture.type = GESTURE_NONE;
    detector->gesture.ready = false;
}

/**
 * @brief Find hand centroid in the 8x8 grid
 */
static void find_hand_centroid(int16_t *distance_mm, uint8_t *status,
                                uint16_t max_dist, HandPosition_t *hand)
{
    float sum_x = 0, sum_y = 0, sum_z = 0;
    int valid_count = 0;
    uint16_t min_dist = 4000;

    /* Scan all 64 zones */
    for (int i = 0; i < 64; i++) {
        /* Check if valid target within range */
        if ((status[i] == STATUS_VALID_1 || status[i] == STATUS_VALID_2) &&
            distance_mm[i] > 0 && distance_mm[i] < max_dist) {

            int row = i / 8;  /* 0-7 */
            int col = i % 8;  /* 0-7 */

            /* Weight by inverse distance (closer = more weight) */
            float weight = 1.0f / (float)distance_mm[i];

            sum_x += col * weight;
            sum_y += row * weight;
            sum_z += distance_mm[i] * weight;
            valid_count++;

            if (distance_mm[i] < min_dist) {
                min_dist = distance_mm[i];
            }
        }
    }

    if (valid_count >= 3) {  /* Need at least 3 valid zones to detect hand */
        float total_weight = valid_count;  /* Simplified weighting */
        hand->x = sum_x / (sum_z / 1000.0f);  /* Normalize by total weight */
        hand->y = sum_y / (sum_z / 1000.0f);
        hand->z = min_dist;  /* Use minimum distance as Z */
        hand->found = true;
    } else {
        hand->found = false;
        hand->z = 4000;
    }
}

/**
 * @brief Detect gesture based on hand movement
 */
static GestureType_t detect_gesture(GestureDetector_t *detector, uint32_t timestamp)
{
    GestureType_t detected = GESTURE_NONE;

    /* Need both previous and current hand positions */
    if (!detector->curr_hand.found) {
        /* Hand lost - check if it was a TAP (approach then retreat) */
        if (detector->tap_approaching && detector->tap_min_distance < 150) {
            uint32_t tap_duration = timestamp - detector->tap_start_time;
            if (tap_duration < GESTURE_TAP_MAX_TIME_MS) {
                detected = GESTURE_TAP;
                detector->gesture.end_time = timestamp;
            }
        }
        detector->tap_approaching = false;
        return detected;
    }

    if (!detector->prev_hand.found) {
        /* Hand just appeared */
        detector->tap_approaching = true;
        detector->tap_start_time = timestamp;
        detector->tap_min_distance = detector->curr_hand.z;
        return GESTURE_NONE;
    }

    /* Calculate time delta */
    uint32_t dt = timestamp - detector->prev_hand.timestamp;
    if (dt == 0) dt = 1;  /* Prevent division by zero */

    float dt_sec = dt / 1000.0f;

    /* Calculate velocities */
    float dx = detector->curr_hand.x - detector->prev_hand.x;
    float dy = detector->curr_hand.y - detector->prev_hand.y;
    float dz = detector->curr_hand.z - detector->prev_hand.z;

    float speed_x = dx / dt_sec;  /* zones/sec */
    float speed_y = dy / dt_sec;  /* zones/sec */
    float speed_z = dz / dt_sec;  /* mm/sec */

    detector->gesture.speed_x = speed_x;
    detector->gesture.speed_y = speed_y;
    detector->gesture.speed_z = speed_z;

    /* Track minimum distance for TAP detection */
    if (detector->curr_hand.z < detector->tap_min_distance) {
        detector->tap_min_distance = detector->curr_hand.z;
    }

    /* Detect TAP: fast approach (negative Z velocity) */
    if (speed_z < -detector->min_tap_speed_z) {
        detector->tap_approaching = true;
        detector->tap_start_time = timestamp;
        detector->tap_min_distance = detector->curr_hand.z;
    }

    /* Detect TAP completion: was approaching, now retreating */
    if (detector->tap_approaching && speed_z > detector->min_tap_speed_z) {
        uint32_t tap_duration = timestamp - detector->tap_start_time;
        if (tap_duration < GESTURE_TAP_MAX_TIME_MS && detector->tap_min_distance < 200) {
            detected = GESTURE_TAP;
            detector->tap_approaching = false;
            detector->gesture.start_time = detector->tap_start_time;
            detector->gesture.end_time = timestamp;
            return detected;
        }
    }

    /* Detect horizontal swipes */
    float abs_speed_x = fabsf(speed_x);
    float abs_speed_y = fabsf(speed_y);

    if (abs_speed_x > detector->min_swipe_speed && abs_speed_x > abs_speed_y * 1.5f) {
        /* Horizontal swipe detected */
        if (speed_x < 0) {
            detected = GESTURE_SWIPE_LEFT;
        } else {
            detected = GESTURE_SWIPE_RIGHT;
        }
        detector->gesture.start_time = detector->prev_hand.timestamp;
        detector->gesture.end_time = timestamp;
    }
    /* Detect vertical swipes */
    else if (abs_speed_y > detector->min_swipe_speed && abs_speed_y > abs_speed_x * 1.5f) {
        /* Vertical swipe detected */
        if (speed_y < 0) {
            detected = GESTURE_SWIPE_UP;
        } else {
            detected = GESTURE_SWIPE_DOWN;
        }
        detector->gesture.start_time = detector->prev_hand.timestamp;
        detector->gesture.end_time = timestamp;
    }

    return detected;
}

/**
 * @brief Process new ToF data and detect gestures
 */
GestureType_t Gesture_Process(GestureDetector_t *detector,
                               int16_t *distance_mm,
                               uint8_t *status,
                               uint32_t timestamp)
{
    if (detector == NULL || distance_mm == NULL || status == NULL) {
        return GESTURE_NONE;
    }

    /* Save previous hand position */
    detector->prev_hand = detector->curr_hand;

    /* Find current hand position */
    find_hand_centroid(distance_mm, status, detector->max_detection_mm, &detector->curr_hand);
    detector->curr_hand.timestamp = timestamp;

    /* Detect gesture based on movement */
    GestureType_t gesture = detect_gesture(detector, timestamp);

    /* Update result */
    if (gesture != GESTURE_NONE) {
        detector->gesture.type = gesture;
        detector->gesture.ready = true;
    } else {
        detector->gesture.ready = false;
    }

    return gesture;
}

/**
 * @brief Get current gesture result
 */
GestureResult_t* Gesture_GetResult(GestureDetector_t *detector)
{
    if (detector == NULL) return NULL;
    return &detector->gesture;
}

/**
 * @brief Get current hand position
 */
HandPosition_t* Gesture_GetHand(GestureDetector_t *detector)
{
    if (detector == NULL) return NULL;
    return &detector->curr_hand;
}

/**
 * @brief Convert gesture type to string
 */
const char* Gesture_TypeToString(GestureType_t type)
{
    switch (type) {
        case GESTURE_NONE:        return "NONE";
        case GESTURE_SWIPE_LEFT:  return "SWIPE_LEFT";
        case GESTURE_SWIPE_RIGHT: return "SWIPE_RIGHT";
        case GESTURE_SWIPE_UP:    return "SWIPE_UP";
        case GESTURE_SWIPE_DOWN:  return "SWIPE_DOWN";
        case GESTURE_TAP:         return "TAP";
        case GESTURE_DOUBLE_TAP:  return "DOUBLE_TAP";
        case GESTURE_AWAY:        return "AWAY";
        default:                  return "UNKNOWN";
    }
}
