/**
  * Header testável do motor_control
  */

#ifndef MOTOR_CONTROL_TESTABLE_H_
#define MOTOR_CONTROL_TESTABLE_H_

#include "stm32u5xx_hal.h"
#include <stdint.h>

/* TB6612FNG I2C Address */
#define MOTOR_I2C_ADDR          0x28

/* Grove I2C Motor Driver Commands */
#define MOTOR_CMD_BRAKE         0x00
#define MOTOR_CMD_STOP          0x01
#define MOTOR_CMD_CW            0x02
#define MOTOR_CMD_CCW           0x03
#define MOTOR_CMD_STANDBY       0x04
#define MOTOR_CMD_NOT_STANDBY   0x05

/* Motor Channels */
#define MOTOR_CHA               0x00
#define MOTOR_CHB               0x01

/* Function Prototypes */
HAL_StatusTypeDef Motor_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef Motor_Stop(void);
HAL_StatusTypeDef Motor_Forward(uint8_t speed);
HAL_StatusTypeDef Motor_Backward(uint8_t speed);
HAL_StatusTypeDef Motor_SetStandby(uint8_t standby);
uint8_t Motor_GetCurrentSpeed(void);
uint8_t motor_is_running(void);

/* Test Helper - Reset internal state */
#ifdef UNIT_TESTING
void Motor_Reset(void);
#endif

#endif /* MOTOR_CONTROL_TESTABLE_H_ */
