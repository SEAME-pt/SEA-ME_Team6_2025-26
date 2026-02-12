/**
  * Versão testável do servo control
  */

#include "servo_testable.h"
#include <stdio.h>

/* Private variables */
static TIM_HandleTypeDef *servo_htim = NULL;
static uint32_t servo_channel = 0;
static uint8_t current_angle = SERVO_CENTER_ANGLE;
static uint32_t timer_period = 0;

/**
 * @brief  Initialize servo motor PWM
 */
HAL_StatusTypeDef Servo_Init(TIM_HandleTypeDef *htim, uint32_t channel)
{
  if (htim == NULL) {
    return HAL_ERROR;
  }

  servo_htim = htim;
  servo_channel = channel;

  /* Get timer period (ARR value) */
  timer_period = __HAL_TIM_GET_AUTORELOAD(servo_htim);

  /* CRITICAL: For Advanced Timers (TIM1, TIM8), enable Main Output Enable (MOE) */
  __HAL_TIM_MOE_ENABLE(servo_htim);

  /* Start PWM */
  if (HAL_TIM_PWM_Start(servo_htim, servo_channel) != HAL_OK) {
    return HAL_ERROR;
  }

  /* Set to center position */
  Servo_Center();

  return HAL_OK;
}

/**
 * @brief  Set servo angle
 */
HAL_StatusTypeDef Servo_SetAngle(uint8_t angle)
{
  if (servo_htim == NULL) {
    return HAL_ERROR;
  }

  /* Clamp angle to valid range */
  if (angle > SERVO_MAX_ANGLE) {
    angle = SERVO_MAX_ANGLE;
  }

  /* Calculate pulse width in microseconds */
  uint32_t pulse_us = SERVO_PULSE_MIN_US +
                      ((uint32_t)angle * (SERVO_PULSE_MAX_US - SERVO_PULSE_MIN_US)) / SERVO_MAX_ANGLE;

  /* Convert to timer compare value */
  uint32_t ccr_value = pulse_us;

  /* Safety check: ensure CCR doesn't exceed period */
  if (ccr_value > timer_period) {
    ccr_value = timer_period;
  }

  /* Set PWM duty cycle */
  __HAL_TIM_SET_COMPARE(servo_htim, servo_channel, ccr_value);

  current_angle = angle;

  return HAL_OK;
}

/**
 * @brief  Set servo to center position (90°)
 */
HAL_StatusTypeDef Servo_Center(void)
{
  return Servo_SetAngle(SERVO_CENTER_ANGLE);
}

/**
 * @brief  Disable servo (stop PWM)
 */
HAL_StatusTypeDef Servo_Disable(void)
{
  if (servo_htim == NULL) {
    return HAL_ERROR;
  }

  /* Stop PWM */
  if (HAL_TIM_PWM_Stop(servo_htim, servo_channel) != HAL_OK) {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
 * @brief  Get current servo angle
 */
uint8_t Servo_GetAngle(void)
{
  return current_angle;
}

#ifdef UNIT_TESTING
/**
 * @brief  Reset internal state (for testing)
 */
void Servo_Reset(void)
{
  servo_htim = NULL;
  servo_channel = 0;
  current_angle = SERVO_CENTER_ANGLE;
  timer_period = 0;
}
#endif
