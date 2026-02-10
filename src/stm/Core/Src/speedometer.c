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
volatile uint32_t pulse_count = 0;
GPIO_PinState estado_anterior = GPIO_PIN_RESET;

// Speed data
float current_speed_kmh = 0.0f;
float current_rpm = 0.0f;
uint32_t last_calculation_time = 0;

/**
  * @brief  Initialize speedometer
  * @retval None
  */
void Speedometer_Init(SystemCtx* ctx)
{
    pulse_count = 0;
    estado_anterior = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);
    last_calculation_time = HAL_GetTick();

    sys_log(ctx, "[Speedometer] Inicializado - Encoder: %d furos, Roda: %.2f mm\r\n",
           ENCODER_HOLES, WHEEL_DIAMETER * 1000);
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
  * @brief  Calculate speed based on pulses (call every 1 second)
  * @retval None
  */
void Speedometer_CalculateSpeed(void)
{
    // Constants
    float wheel_perimeter = PI * WHEEL_DIAMETER;

    // Calculations
    float rps = (float)pulse_count / ENCODER_HOLES;  // Rotations per second
    current_rpm = rps * 60.0f;                       // RPM
    float speed_ms = rps * wheel_perimeter;          // Speed in m/s
    current_speed_kmh = speed_ms * 3.6f;             // Speed in km/h

    // Reset pulse count for next second
    pulse_count = 0;
    last_calculation_time = HAL_GetTick();
}

/**
  * @brief  Get current speed in km/h
  * @retval Speed in km/h
  */
float Speedometer_GetSpeed(void)
{
    return current_speed_kmh;
}

/**
  * @brief  Get current RPM
  * @retval RPM
  */
float Speedometer_GetRPM(void)
{
    return current_rpm;
}
