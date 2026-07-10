/*
 * speedometer.c
 *
 *  Created on: Dec 9, 2025
 *      Author: seame
 *  Modified for ThreadX integration
 */

#include "speedometer.h"
#include "tx_api.h"
#include <stdio.h>
#include <string.h>

// Pulse counting variables
volatile uint32_t last_capture   = 0;  // TIM4 CCR1 value at last edge
volatile uint32_t capture_delta  = 0;  // ticks between last two edges
volatile uint32_t last_pulse_ms  = 0;  // HAL_GetTick() at last edge
volatile uint8_t  capture_valid  = 0;  // have we seen at least 2 edges?

// TIM4 runs at 1MHz with prescaler=159 → 1 tick = 1us
#define TIM4_TICK_HZ   1000000UL

// Speed data
static float current_speed_kmh = 0.0f;
static float current_rpm = 0.0f;
static uint32_t last_calculation_tick = 0;
static uint32_t last_ms = 0;

uint32_t last_calculation_time = 0;

/**
  * @brief  Initialize speedometer
  * @retval None
  */
void Speedometer_Init(SystemCtx* ctx)
{
    current_speed_kmh = 0.0f;
    current_rpm = 0.0f;
    last_calculation_tick = HAL_GetTick();

    sys_log(ctx, "[Speedometer] Init - Furos: %d, Diametro: %.1fmm, Circ: %.1fmm",
            ENCODER_HOLES,
            WHEEL_DIAMETER * 1000.0f,
            PI * WHEEL_DIAMETER * 1000.0f);
}

/**
 * @brief  Called from HAL_GPIO_EXTI_Callback when PB6 triggers
 *         (configure PB6 as EXTI with rising edge in CubeMX)
 */
void Speedometer_PulseISR(void)
{
	//uint32_t now = HAL_GetTick();
	//if (now - last_ms < 3) return;
	//last_ms = now;
}

void Speedometer_CalculateSpeed(SystemCtx* ctx)
{
    // Timeout check — wheel has stopped
    if ((HAL_GetTick() - last_pulse_ms) > SPEED_TIMEOUT_MS)
    {
        current_speed_kmh = 0.0f;
        current_rpm       = 0.0f;
        capture_valid     = 0;
        return;
    }

    if (!capture_valid)
        return;

    // Atomically read and clear the delta
    __disable_irq();
    uint32_t delta = capture_delta;
    capture_delta = 0;
    __enable_irq();

    if (delta == 0) return;

    // Time between pulses in seconds
    // delta is in TIM4 ticks; TIM4 runs at (SystemCoreClock / (Prescaler+1))
    float tick_period_s = 1.0f / (float)TIM4_TICK_HZ;
    float pulse_period_s = (float)delta * tick_period_s;

    // One pulse = 1/ENCODER_HOLES of a rotation
    float rps         = 1.0f / (pulse_period_s * (float)ENCODER_HOLES);
    current_rpm       = rps * 60.0f;
    current_speed_kmh = rps * WHEEL_CIRC * 3.6f;

    sys_log(ctx, "[Speedometer] delta=%lu ticks | RPM=%.1f | Speed=%.2f km/h",
            delta, current_rpm, current_speed_kmh);
}

float Speedometer_GetSpeed(void)
{
    return current_speed_kmh;
}

float Speedometer_GetRPM(void)
{
    return current_rpm;
}
