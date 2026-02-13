/**
  * Header testável do servo control
  */

#ifndef SERVO_TESTABLE_H_
#define SERVO_TESTABLE_H_

#include "stm32u5xx_hal.h"
#include <stdint.h>

/* Servo Configuration */
#define SERVO_MIN_ANGLE      0
#define SERVO_MAX_ANGLE      180
#define SERVO_CENTER_ANGLE   90

/* PWM Timing */
#define SERVO_PWM_FREQ_HZ    50
#define SERVO_PWM_PERIOD_MS  20

/* Pulse width timing */
#define SERVO_PULSE_MIN_US   1000
#define SERVO_PULSE_MAX_US   2000
#define SERVO_PULSE_CENTER_US 1500

/* Function Prototypes */
HAL_StatusTypeDef Servo_Init(TIM_HandleTypeDef *htim, uint32_t channel);
HAL_StatusTypeDef Servo_SetAngle(uint8_t angle);
HAL_StatusTypeDef Servo_Center(void);
HAL_StatusTypeDef Servo_Disable(void);
uint8_t Servo_GetAngle(void);

/* Test Helper - Reset internal state */
#ifdef UNIT_TESTING
void Servo_Reset(void);
#endif

#endif /* SERVO_TESTABLE_H_ */
