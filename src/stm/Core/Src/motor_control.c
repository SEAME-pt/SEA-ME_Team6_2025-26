/**
  ******************************************************************************
  * @file    motor_control.c
  * @brief   Driver implementation for Grove TB6612FNG Motor Driver (I2C)
  ******************************************************************************
  */

#include "motor_control.h"
#include "tx_api.h"
#include <stdio.h>

/* Private Variables */
static I2C_HandleTypeDef *motor_i2c;
static uint8_t current_speed = 0;

// Debug flag - set to 1 to see I2C writes (disabled now that it works)
#define MOTOR_DEBUG_ENABLED 0

// External mutex for printf protection
extern TX_MUTEX printf_mutex;

/**
  * @brief  Send command to single motor channel (Grove protocol)
  * @param  cmd: motor command (CW, CCW, BRAKE, STOP)
  * @param  channel: motor channel (MOTOR_CHA or MOTOR_CHB)
  * @param  speed: motor speed (0-255)
  * @retval HAL status
  */
static HAL_StatusTypeDef Motor_SendCommand(uint8_t cmd, uint8_t channel, uint8_t speed)
{
    uint8_t data[3] = {cmd, channel, speed};
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(motor_i2c, MOTOR_I2C_ADDR, data, 3, 100);

#if MOTOR_DEBUG_ENABLED
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[MOTOR_I2C] Cmd=0x%02X, Ch=%d, Speed=%d -> Status=%d\r\n",
           cmd, channel, speed, status);
    tx_mutex_put(&printf_mutex);
#endif

    HAL_Delay(10); // Increased delay for ATmega8 processing
    return status;
}

/**
  * @brief  Send simple command (STANDBY, NOT_STANDBY)
  * @param  cmd: command byte
  * @param  value: data byte (usually 0)
  * @retval HAL status
  */
static HAL_StatusTypeDef Motor_SendSimpleCommand(uint8_t cmd, uint8_t value)
{
    uint8_t data[2] = {cmd, value};
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(motor_i2c, MOTOR_I2C_ADDR, data, 2, 100);

#if MOTOR_DEBUG_ENABLED
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[MOTOR_I2C] SimpleCmd=0x%02X, Val=0x%02X -> Status=%d\r\n",
           cmd, value, status);
    tx_mutex_put(&printf_mutex);
#endif

    HAL_Delay(10); // Increased delay for ATmega8 processing
    return status;
}

/**
  * @brief  Initialize motor driver (Grove TB6612FNG protocol)
  * @param  hi2c: pointer to I2C handle
  * @retval HAL status
  * @note   Simplified init - just store I2C handle, no commands sent
  */
HAL_StatusTypeDef Motor_Init(I2C_HandleTypeDef *hi2c)
{
    motor_i2c = hi2c;
    current_speed = 0;

    HAL_Delay(100); // Wait for motor driver power-up

    // Note: Do NOT send NOT_STANDBY or initial BRAKE - they interfere!
    // Module is ready to receive motor commands directly

    return HAL_OK;
}

/**
  * @brief  Stop both motors (Grove protocol - BRAKE command)
  * @retval HAL status
  */
HAL_StatusTypeDef Motor_Stop(void)
{
    HAL_StatusTypeDef status;

    // Send BRAKE command to Motor A (immediate stop)
    status = Motor_SendCommand(MOTOR_CMD_BRAKE, MOTOR_CHA, 0);
    if (status != HAL_OK) return status;

    // Send BRAKE command to Motor B (immediate stop)
    status = Motor_SendCommand(MOTOR_CMD_BRAKE, MOTOR_CHB, 0);
    if (status != HAL_OK) return status;

    current_speed = 0;
    return HAL_OK;
}

/**
  * @brief  Move motors forward (Grove protocol: CW command)
  * @param  speed: motor speed (0-100%)
  * @retval HAL status
  */
HAL_StatusTypeDef Motor_Forward(uint8_t speed)
{
    HAL_StatusTypeDef status;

    // Limit speed to 100%
    if (speed > 100) speed = 100;

    // Convert percentage to 0-255
    uint8_t pwm_value = (uint8_t)((speed * 255) / 100);

    // Send CW (clockwise/forward) command to Motor A
    status = Motor_SendCommand(MOTOR_CMD_CW, MOTOR_CHA, pwm_value);
    if (status != HAL_OK) return status;

    // Send CW (clockwise/forward) command to Motor B
    status = Motor_SendCommand(MOTOR_CMD_CW, MOTOR_CHB, pwm_value);
    if (status != HAL_OK) return status;

    current_speed = speed;
    return HAL_OK;
}

/**
  * @brief  Move motors backward (Grove protocol: CCW command)
  * @param  speed: motor speed (0-100%)
  * @retval HAL status
  */
HAL_StatusTypeDef Motor_Backward(uint8_t speed)
{
    HAL_StatusTypeDef status;

    // Limit speed to 100%
    if (speed > 100) speed = 100;

    // Convert percentage to 0-255
    uint8_t pwm_value = (uint8_t)((speed * 255) / 100);

    // Send CCW (counter-clockwise/backward) command to Motor A
    status = Motor_SendCommand(MOTOR_CMD_CCW, MOTOR_CHA, pwm_value);
    if (status != HAL_OK) return status;

    // Send CCW (counter-clockwise/backward) command to Motor B
    status = Motor_SendCommand(MOTOR_CMD_CCW, MOTOR_CHB, pwm_value);
    if (status != HAL_OK) return status;

    current_speed = speed;
    return HAL_OK;
}

/**
  * @brief  Set motor speed (not used in Grove protocol - direction must be explicit)
  * @param  speed: motor speed (0-100%)
  * @retval HAL status
  * @note   This function is deprecated for Grove I2C Motor Driver.
  *         Use Motor_Forward() or Motor_Backward() instead.
  */
HAL_StatusTypeDef Motor_SetSpeed(uint8_t speed)
{
    // Grove protocol requires explicit direction (CW or CCW)
    // Cannot change speed without knowing direction
    // For now, just update the current_speed variable
    if (speed > 100) speed = 100;
    current_speed = speed;
    return HAL_OK;
}

/**
  * @brief  Set standby mode (Grove protocol)
  * @param  standby: 1 to enable standby, 0 to disable
  * @retval HAL status
  */
HAL_StatusTypeDef Motor_SetStandby(uint8_t standby)
{
    if (standby) {
        // Enter standby mode
        return Motor_SendSimpleCommand(MOTOR_CMD_STANDBY, 0);
    } else {
        // Exit standby mode (NOT_STANDBY command)
        return Motor_SendSimpleCommand(MOTOR_CMD_NOT_STANDBY, 0);
    }
}

/**
  * @brief  Hardware test function (call before ThreadX starts)
  * @param  hi2c: pointer to I2C handle
  * @retval None
  */
void Motor_HardwareTest(I2C_HandleTypeDef *hi2c)
{
    HAL_StatusTypeDef status;

    printf("\r\n");
    printf("========================================\r\n");
    printf("   MOTOR HARDWARE TEST\r\n");
    printf("========================================\r\n");

    // Step 1: Reset motor driver (Relay is normally closed)
    printf("1. RESET do motor driver via RELAY (normally closed)...\r\n");
    printf("   a) Desligando alimentacao (Relay ON -> corta corrente)...\r\n");
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);  // HIGH = Relay ON = Corta
    HAL_Delay(500); // Wait for power to drain
    printf("   b) Ligando alimentacao (Relay OFF -> passa corrente)...\r\n");
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET); // LOW = Relay OFF = Passa
    HAL_Delay(1000); // Wait for motor driver to power up
    printf("   Motor driver alimentado e pronto!\r\n");

    // Step 2: I2C Scan (find device address)
    printf("2. I2C SCAN - Procurando dispositivos...\r\n");
    uint8_t found_count = 0;

    for (uint8_t addr = 0x08; addr < 0x78; addr++) {
        if (HAL_I2C_IsDeviceReady(hi2c, addr << 1, 1, 10) == HAL_OK) {
            printf("   Dispositivo encontrado em: 0x%02X\r\n", addr << 1);
            found_count++;
            if (found_count >= 10) break;
        }
    }

    if (found_count == 0) {
        printf("   >>> NENHUM dispositivo I2C encontrado! <<<\r\n");
        printf("   Verifique cabos I2C (SDA/SCL)\r\n");
        printf("========================================\r\n\r\n");
        return;
    }

    printf("   Total encontrados: %d\r\n", found_count);
    printf("\r\n");

    // Step 3: Initialize motor driver (simplified - no commands sent)
    printf("3. Inicializando TB6612FNG (I2C Addr=0x%02X)...\r\n", MOTOR_I2C_ADDR);
    status = Motor_Init(hi2c);
    printf("   Init Status: %d %s\r\n", status, (status == HAL_OK) ? "[OK]" : "[ERRO!]");
    HAL_Delay(100);

    // Step 4: Test FORWARD at 100% (Grove command-based protocol)
    printf("4. Testando FRENTE (100%% - protocolo Grove correto)...\r\n");
    printf("   Usando comando CW (0x02) para ambos os motores\r\n");
    status = Motor_Forward(20);
    printf("   Forward Status: %d %s\r\n", status, (status == HAL_OK) ? "[OK]" : "[ERRO!]");
    printf("   >>> MOTORES DEVEM ANDAR PARA FRENTE! <<<\r\n");
    printf("   Se nao andarem, verifica:\r\n");
    printf("   - VM (motor power) = 12V?\r\n");
    printf("   - Motores ligados a A1/A2 e B1/B2?\r\n");
    printf("   - Cabos I2C ligados corretamente?\r\n");
    HAL_Delay(3000); // 3 seconds forward at full power

    // Step 5: STOP
    printf("5. PARANDO motores...\r\n");
    status = Motor_Stop();
    printf("   Stop Status: %d %s\r\n", status, (status == HAL_OK) ? "[OK]" : "[ERRO!]");
    HAL_Delay(1000); // 1 second stop

    // Step 6: Test BACKWARD at 100%
    printf("6. Testando TRAS (100%% - protocolo Grove correto)...\r\n");
    printf("   Usando comando CCW (0x03) para ambos os motores\r\n");
    status = Motor_Backward(20);
    printf("   Backward Status: %d %s\r\n", status, (status == HAL_OK) ? "[OK]" : "[ERRO!]");
    printf("   >>> MOTORES DEVEM ANDAR PARA TRAS! <<<\r\n");
    HAL_Delay(3000); // 3 seconds backward at full power

    // Step 7: Final STOP
    printf("7. PARANDO motores (final)...\r\n");
    status = Motor_Stop();
    printf("   Stop Status: %d %s\r\n", status, (status == HAL_OK) ? "[OK]" : "[ERRO!]");

    HAL_Delay(500);

    // Step 8: Turn OFF motor driver power (Relay ON to cut power)
    printf("8. Desligando motor driver (Relay ON -> corta corrente)...\r\n");
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);  // HIGH = Corta
    printf("   Motor driver desligado\r\n");

    printf("========================================\r\n");
    printf("   TESTE COMPLETO COM PROTOCOLO GROVE!\r\n");
    printf("   Se os motores andaram, o protocolo esta correto!\r\n");
    printf("========================================\r\n\r\n");
}
