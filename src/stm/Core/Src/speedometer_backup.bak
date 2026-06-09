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
static volatile uint32_t pulse_count = 0;
GPIO_PinState estado_anterior = GPIO_PIN_RESET;

// Speed data
static float current_speed_kmh = 0.0f;
static float current_rpm = 0.0f;
static uint32_t last_calculation_tick = 0;

uint32_t last_calculation_time = 0;

/**
  * @brief  Initialize speedometer
  * @retval None
  */
void Speedometer_Init(SystemCtx* ctx)
{
    pulse_count = 0;
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
    pulse_count++;
}

/**
  * @brief  Count pulses from encoder (call this frequently!)
  * @retval None
  */
void Speedometer_CountPulse(void)
{
    GPIO_PinState estado_atual = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);

    // Rising Edge (0 -> 1)
    if (estado_atual == GPIO_PIN_SET && estado_anterior == GPIO_PIN_RESET)
    {
        pulse_count++;
    }

    estado_anterior = estado_atual;
}


/**
 * @brief  Calculate speed — call periodically (e.g., every 500ms–1s)
 */
void Speedometer_CalculateSpeed(void)
{
    uint32_t now = HAL_GetTick();
    uint32_t elapsed_ms = now - last_calculation_tick;

    // Avoid division by zero or too-short intervals
    if (elapsed_ms < 100)
        return;

    // Atomic read and reset of pulse count
    __disable_irq();
    uint32_t pulses = pulse_count;
    pulse_count = 0;
    __enable_irq();

    float elapsed_s = (float)elapsed_ms / 1000.0f;
    float wheel_circ = PI * WHEEL_DIAMETER;  // ~0.2136m

    // pulses / 18 = rotações neste intervalo
    float rotations = (float)pulses / (float)ENCODER_HOLES;

    // RPM
    float rps = rotations / elapsed_s;
    current_rpm = rps * 60.0f;

    // Velocidade
    float speed_ms = rps * wheel_circ;       // m/s
    current_speed_kmh = speed_ms * 3.6f;     // km/h

    last_calculation_tick = now;
}

float Speedometer_GetSpeed(void)
{
    return current_speed_kmh;
}

float Speedometer_GetRPM(void)
{
    return current_rpm;
}
