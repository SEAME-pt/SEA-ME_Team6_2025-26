/**
  ******************************************************************************
  * @file    motor_control.h
  * @brief   Driver for Grove TB6612FNG Motor Driver (I2C)
  ******************************************************************************
  */

#ifndef INC_MOTOR_CONTROL_H_
#define INC_MOTOR_CONTROL_H_

#include "stm32u5xx_hal.h"
#include <stdint.h>

/* TB6612FNG I2C Address - CONFIRMED by I2C scan! */
#define MOTOR_I2C_ADDR          0x28  // 0x14 << 1 (7-bit addr shifted for HAL)

/* Grove I2C Motor Driver Commands (TB6612FNG)
 * Protocol: Command-based I2C (not register-based!)
 * Format: [I2C_ADDR] [COMMAND] [CHANNEL] [SPEED]
 *
 * Example: Motor A forward at speed 255
 *   uint8_t data[] = {0x02, 0x00, 255};  // CW, MOTOR_CHA, speed
 *   HAL_I2C_Master_Transmit(i2c, 0x28, data, 3, 100);
 */
#define MOTOR_CMD_BRAKE         0x00    // Brake motor (immediate stop)
#define MOTOR_CMD_STOP          0x01    // Stop motor (slow decel)
#define MOTOR_CMD_CW            0x02    // Clockwise (forward)
#define MOTOR_CMD_CCW           0x03    // Counter-clockwise (backward)
#define MOTOR_CMD_STANDBY       0x04    // Enter standby mode
#define MOTOR_CMD_NOT_STANDBY   0x05    // Exit standby mode

/* Motor Channels */
#define MOTOR_CHA               0x00    // Motor channel A
#define MOTOR_CHB               0x01    // Motor channel B

/* Motor Direction Commands (for CAN interface) */
typedef enum {
    MOTOR_DIR_STOP = 0,
    MOTOR_DIR_FORWARD = 1,
    MOTOR_DIR_BACKWARD = 2
} MotorDirection_t;

/* Function Prototypes */
HAL_StatusTypeDef Motor_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef Motor_Stop(void);
HAL_StatusTypeDef Motor_Forward(uint8_t speed);
HAL_StatusTypeDef Motor_Backward(uint8_t speed);
HAL_StatusTypeDef Motor_SetSpeed(uint8_t speed);
HAL_StatusTypeDef Motor_SetStandby(uint8_t standby);

/* Hardware Test Function */
void Motor_HardwareTest(I2C_HandleTypeDef *hi2c);

#endif /* INC_MOTOR_CONTROL_H_ */
