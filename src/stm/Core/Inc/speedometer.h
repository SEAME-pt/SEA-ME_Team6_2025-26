/*
 * speedometer.h
 *
 *  Created on: Dec 9, 2025
 *      Author: seame
 *  Modified for ThreadX integration
 */

#ifndef INC_SPEEDOMETER_H_
#define INC_SPEEDOMETER_H_

#include <stdio.h>
#include <stdint.h>
#include "main.h"
#include "system_ctx.h"
#include "sys_helpers.h"

// Speedometer constants
#define PI 3.14159265
#define WHEEL_DIAMETER 0.06675f  // Em metros (66.75mm)
#define ENCODER_HOLES 18         // Furos no disco do encoder
#define WHEEL_CIRC        (PI * WHEEL_DIAMETER)
#define SPEED_TIMEOUT_MS  500

extern volatile uint32_t last_capture;  // TIM4 CCR1 value at last edge
extern volatile uint32_t capture_delta;  // ticks between last two edges
extern volatile uint32_t last_pulse_ms;  // HAL_GetTick() at last edge
extern volatile uint8_t  capture_valid;

// External variables
// extern volatile uint32_t pulse_count;
//extern float current_speed_kmh;
//extern float current_rpm;

// Function prototypes
void Speedometer_Init(SystemCtx* ctx);
void Speedometer_PulseISR(void);
void Speedometer_CountPulse(void);
void Speedometer_CalculateSpeed(SystemCtx* ctx);
float Speedometer_GetSpeed(void);
float Speedometer_GetRPM(void);

#endif /* INC_SPEEDOMETER_H_ */
