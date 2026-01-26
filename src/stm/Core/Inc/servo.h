/*
 * servo.h
 *
 *  Created on: Dec 28, 2025
 *      Author: rcosta-c
 *
 *  Description: Driver for MG996R servo motor control via PWM
 *               - Model: WS-MG996R (Metal gear, 55g, ~10kg.cm torque)
 *               - PWM: 50Hz (20ms period)
 *               - Pulse width: 1ms (0°) to 2ms (180°)
 *               - Connected to: CN13 Pin 2
 */

#ifndef INC_SERVO_H_
#define INC_SERVO_H_

#include "stm32u5xx_hal.h"
#include <stdint.h>

/* Servo Configuration */
#define SERVO_MIN_ANGLE      0      /* Minimum angle (degrees) */
#define SERVO_MAX_ANGLE      180    /* Maximum angle (degrees) */
#define SERVO_CENTER_ANGLE   90     /* Center position */

/* PWM Timing (for 50Hz = 20ms period) */
#define SERVO_PWM_FREQ_HZ    50     /* 50Hz */
#define SERVO_PWM_PERIOD_MS  20     /* 20ms period */

/* Pulse width timing */
#define SERVO_PULSE_MIN_US   1000   /* 1ms = 0° */
#define SERVO_PULSE_MAX_US   2000   /* 2ms = 180° */
#define SERVO_PULSE_CENTER_US 1500  /* 1.5ms = 90° */

/* Function Prototypes */

/**
 * @brief  Initialize servo motor PWM
 * @param  htim: Pointer to TIM handle (e.g., &htim3)
 * @param  channel: TIM channel (e.g., TIM_CHANNEL_1)
 * @retval HAL_StatusTypeDef
 */
HAL_StatusTypeDef Servo_Init(TIM_HandleTypeDef *htim, uint32_t channel);

/**
 * @brief  Set servo angle
 * @param  angle: Desired angle in degrees (0-180)
 * @retval HAL_StatusTypeDef
 */
HAL_StatusTypeDef Servo_SetAngle(uint8_t angle);

/**
 * @brief  Set servo to center position (90°)
 * @retval HAL_StatusTypeDef
 */
HAL_StatusTypeDef Servo_Center(void);

/**
 * @brief  Disable servo (stop PWM)
 * @retval HAL_StatusTypeDef
 */
HAL_StatusTypeDef Servo_Disable(void);

/**
 * @brief  Get current servo angle
 * @retval Current angle (0-180)
 */
uint8_t Servo_GetAngle(void);

#endif /* INC_SERVO_H_ */
