/**
  * Versão testável do motor_control
  * Sem dependências do STM32 real
  */

#include "stm32u5xx_hal.h"
#include "motor_control_testable.h"
#include "tx_api.h"
#include <stdio.h>

/* Private Variables */
static I2C_HandleTypeDef *motor_i2c;
static uint8_t current_speed = 0;

// External mutex for printf protection
extern TX_MUTEX printf_mutex;

/**
  * @brief  Send command to single motor channel (Grove protocol)
  */
static HAL_StatusTypeDef Motor_SendCommand(uint8_t cmd, uint8_t channel, uint8_t speed)
{
    uint8_t data[3] = {cmd, channel, speed};
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(motor_i2c, MOTOR_I2C_ADDR, data, 3, 100);
    HAL_Delay(10);
    return status;
}

/**
  * @brief  Send simple command (STANDBY, NOT_STANDBY)
  */
static HAL_StatusTypeDef Motor_SendSimpleCommand(uint8_t cmd, uint8_t value)
{
    uint8_t data[2] = {cmd, value};
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(motor_i2c, MOTOR_I2C_ADDR, data, 2, 100);
    HAL_Delay(10);
    return status;
}

/**
  * @brief  Initialize motor driver
  */
HAL_StatusTypeDef Motor_Init(I2C_HandleTypeDef *hi2c)
{
    motor_i2c = hi2c;
    current_speed = 0;
    HAL_Delay(100);
    return HAL_OK;
}

/**
  * @brief  Stop both motors
  */
HAL_StatusTypeDef Motor_Stop(void)
{
    HAL_StatusTypeDef status;

    status = Motor_SendCommand(MOTOR_CMD_BRAKE, MOTOR_CHA, 0);
    if (status != HAL_OK) return status;

    status = Motor_SendCommand(MOTOR_CMD_BRAKE, MOTOR_CHB, 0);
    if (status != HAL_OK) return status;

    current_speed = 0;
    return HAL_OK;
}

/**
  * @brief  Move motors forward
  */
HAL_StatusTypeDef Motor_Forward(uint8_t speed)
{
    HAL_StatusTypeDef status;

    if (speed > 100) speed = 100;

    uint8_t pwm_value = (uint8_t)((speed * 255) / 100);

    status = Motor_SendCommand(MOTOR_CMD_CW, MOTOR_CHA, pwm_value);
    if (status != HAL_OK) return status;

    status = Motor_SendCommand(MOTOR_CMD_CW, MOTOR_CHB, pwm_value);
    if (status != HAL_OK) return status;

    current_speed = speed;
    return HAL_OK;
}

/**
  * @brief  Move motors backward
  */
HAL_StatusTypeDef Motor_Backward(uint8_t speed)
{
    HAL_StatusTypeDef status;

    if (speed > 100) speed = 100;

    uint8_t pwm_value = (uint8_t)((speed * 255) / 100);

    status = Motor_SendCommand(MOTOR_CMD_CCW, MOTOR_CHA, pwm_value);
    if (status != HAL_OK) return status;

    status = Motor_SendCommand(MOTOR_CMD_CCW, MOTOR_CHB, pwm_value);
    if (status != HAL_OK) return status;

    current_speed = speed;
    return HAL_OK;
}

/**
  * @brief  Get current speed (for testing)
  */
uint8_t Motor_GetCurrentSpeed(void)
{
    return current_speed;
}

/**
  * @brief  Set standby mode
  */
HAL_StatusTypeDef Motor_SetStandby(uint8_t standby)
{
    if (standby) {
        return Motor_SendSimpleCommand(MOTOR_CMD_STANDBY, 0);
    } else {
        return Motor_SendSimpleCommand(MOTOR_CMD_NOT_STANDBY, 0);
    }
}

/**
  * @brief  Check if motor is running
  */
uint8_t motor_is_running(void)
{
    return (current_speed > 0);
}

#ifdef UNIT_TESTING
/**
  * @brief  Reset internal state (for testing)
  */
void Motor_Reset(void)
{
    motor_i2c = NULL;
    current_speed = 0;
}
#endif
