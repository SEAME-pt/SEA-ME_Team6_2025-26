/**
  * Testes unitários para o Motor Control (TB6612FNG)
  * Usando Google Test Framework
  */

#include <gtest/gtest.h>

extern "C" {
    #include "motor_control_testable.h"
    #include "stm32u5xx_hal.h"
}

/**
 * Fixture para os testes do motor control
 */
class MotorControlTest : public ::testing::Test {
protected:
    I2C_HandleTypeDef mock_i2c;

    void SetUp() override {
        // Reset estado dos mocks antes de cada teste
        mock_set_i2c_status(HAL_OK);
        mock_clear_i2c_history();

        // Inicializar estrutura I2C mock
        mock_i2c.Instance = nullptr;
        mock_i2c.Init = 0;
    }

    void TearDown() override {
        // Cleanup após cada teste
        mock_clear_i2c_history();
    }

    // Helper para verificar comando I2C enviado
    void VerifyI2CCommand(uint8_t expected_cmd, uint8_t expected_channel,
                          uint8_t expected_speed) {
        uint8_t buffer[10];
        uint16_t size = 0;
        uint16_t address = 0;

        mock_get_i2c_last_tx(buffer, &size, &address);

        EXPECT_EQ(address, MOTOR_I2C_ADDR);
        EXPECT_EQ(size, 3);
        EXPECT_EQ(buffer[0], expected_cmd);
        EXPECT_EQ(buffer[1], expected_channel);
        EXPECT_EQ(buffer[2], expected_speed);
    }
};

/**
 * MOT-01: Inicialização do motor driver
 */
TEST_F(MotorControlTest, InitializeMotorDriver) {
    // Act
    HAL_StatusTypeDef status = Motor_Init(&mock_i2c);

    // Assert
    EXPECT_EQ(status, HAL_OK);
    EXPECT_EQ(Motor_GetCurrentSpeed(), 0);
}

/**
 * MOT-02: Motor para frente (velocidade 50%)
 */
TEST_F(MotorControlTest, MotorForward50Percent) {
    // Arrange
    Motor_Init(&mock_i2c);
    mock_clear_i2c_history();

    // Act
    HAL_StatusTypeDef status = Motor_Forward(50);

    // Assert
    EXPECT_EQ(status, HAL_OK);
    EXPECT_EQ(Motor_GetCurrentSpeed(), 50);

    // Verificar comandos I2C enviados
    uint8_t buffer[10];
    uint16_t size = 0;
    uint16_t address = 0;
    mock_get_i2c_last_tx(buffer, &size, &address);

    // Deve ter enviado 2 comandos (Motor A e Motor B)
    // Verificar o último comando (Motor B)
    EXPECT_EQ(address, MOTOR_I2C_ADDR);
    EXPECT_EQ(size, 3);
    EXPECT_EQ(buffer[0], MOTOR_CMD_CW);  // Comando CW (frente)
    EXPECT_EQ(buffer[1], MOTOR_CHB);     // Motor B
    uint8_t expected_pwm = (50 * 255) / 100;  // 127
    EXPECT_EQ(buffer[2], expected_pwm);  // PWM value
}

/**
 * MOT-03: Motor para trás (velocidade 50%)
 */
TEST_F(MotorControlTest, MotorBackward50Percent) {
    // Arrange
    Motor_Init(&mock_i2c);
    mock_clear_i2c_history();

    // Act
    HAL_StatusTypeDef status = Motor_Backward(50);

    // Assert
    EXPECT_EQ(status, HAL_OK);
    EXPECT_EQ(Motor_GetCurrentSpeed(), 50);

    // Verificar comando I2C (Motor B)
    uint8_t buffer[10];
    uint16_t size = 0;
    uint16_t address = 0;
    mock_get_i2c_last_tx(buffer, &size, &address);

    EXPECT_EQ(buffer[0], MOTOR_CMD_CCW);  // Comando CCW (trás)
    EXPECT_EQ(buffer[1], MOTOR_CHB);
}

/**
 * MOT-04: Parar motor (BRAKE)
 */
TEST_F(MotorControlTest, MotorStop) {
    // Arrange
    Motor_Init(&mock_i2c);
    Motor_Forward(50);  // Motor em movimento
    mock_clear_i2c_history();

    // Act
    HAL_StatusTypeDef status = Motor_Stop();

    // Assert
    EXPECT_EQ(status, HAL_OK);
    EXPECT_EQ(Motor_GetCurrentSpeed(), 0);

    // Verificar comando BRAKE enviado
    uint8_t buffer[10];
    uint16_t size = 0;
    uint16_t address = 0;
    mock_get_i2c_last_tx(buffer, &size, &address);

    EXPECT_EQ(buffer[0], MOTOR_CMD_BRAKE);  // Comando BRAKE
}

/**
 * MOT-05: Velocidade máxima (100%)
 */
TEST_F(MotorControlTest, MotorForward100Percent) {
    // Arrange
    Motor_Init(&mock_i2c);
    mock_clear_i2c_history();

    // Act
    HAL_StatusTypeDef status = Motor_Forward(100);

    // Assert
    EXPECT_EQ(status, HAL_OK);
    EXPECT_EQ(Motor_GetCurrentSpeed(), 100);

    // Verificar PWM = 255 (máximo)
    uint8_t buffer[10];
    uint16_t size = 0;
    uint16_t address = 0;
    mock_get_i2c_last_tx(buffer, &size, &address);

    EXPECT_EQ(buffer[2], 255);  // PWM máximo
}

/**
 * MOT-06: Velocidade mínima (0%)
 */
TEST_F(MotorControlTest, MotorForward0Percent) {
    // Arrange
    Motor_Init(&mock_i2c);
    mock_clear_i2c_history();

    // Act
    HAL_StatusTypeDef status = Motor_Forward(0);

    // Assert
    EXPECT_EQ(status, HAL_OK);
    EXPECT_EQ(Motor_GetCurrentSpeed(), 0);

    // Verificar PWM = 0
    uint8_t buffer[10];
    uint16_t size = 0;
    uint16_t address = 0;
    mock_get_i2c_last_tx(buffer, &size, &address);

    EXPECT_EQ(buffer[2], 0);  // PWM zero
}

/**
 * MOT-07: Mudança de direção instantânea
 */
TEST_F(MotorControlTest, DirectionChangeForwardToBackward) {
    // Arrange
    Motor_Init(&mock_i2c);
    Motor_Forward(50);

    // Act
    HAL_StatusTypeDef status = Motor_Backward(50);

    // Assert
    EXPECT_EQ(status, HAL_OK);
    EXPECT_EQ(Motor_GetCurrentSpeed(), 50);

    // Verificar que o último comando foi CCW
    uint8_t buffer[10];
    uint16_t size = 0;
    uint16_t address = 0;
    mock_get_i2c_last_tx(buffer, &size, &address);

    EXPECT_EQ(buffer[0], MOTOR_CMD_CCW);
}

/**
 * MOT-08: Comando I2C (Grove protocol)
 */
TEST_F(MotorControlTest, I2CProtocolCorrect) {
    // Arrange
    Motor_Init(&mock_i2c);
    mock_clear_i2c_history();

    // Act - Enviar comando Forward a 75%
    Motor_Forward(75);

    // Assert - Verificar formato completo do protocolo Grove
    uint8_t buffer[10];
    uint16_t size = 0;
    uint16_t address = 0;
    mock_get_i2c_last_tx(buffer, &size, &address);

    // Endereço I2C correto
    EXPECT_EQ(address, MOTOR_I2C_ADDR);

    // Tamanho correto (3 bytes: comando, canal, velocidade)
    EXPECT_EQ(size, 3);

    // Comando CW
    EXPECT_EQ(buffer[0], MOTOR_CMD_CW);

    // Canal B
    EXPECT_EQ(buffer[1], MOTOR_CHB);

    // PWM = 75% de 255 = 191
    uint8_t expected_pwm = (75 * 255) / 100;
    EXPECT_EQ(buffer[2], expected_pwm);
}

/**
 * MOT-09: Proteção contra valores inválidos (speed > 100)
 */
TEST_F(MotorControlTest, SpeedLimitedTo100) {
    // Arrange
    Motor_Init(&mock_i2c);
    mock_clear_i2c_history();

    // Act - Tentar enviar velocidade > 100
    HAL_StatusTypeDef status = Motor_Forward(150);

    // Assert - Velocidade deve ser limitada a 100
    EXPECT_EQ(status, HAL_OK);
    EXPECT_EQ(Motor_GetCurrentSpeed(), 100);

    // Verificar que PWM é 255 (máximo)
    uint8_t buffer[10];
    uint16_t size = 0;
    uint16_t address = 0;
    mock_get_i2c_last_tx(buffer, &size, &address);

    EXPECT_EQ(buffer[2], 255);
}

/**
 * MOT-10: Timeout de comando I2C (falha de comunicação)
 */
TEST_F(MotorControlTest, I2CTimeoutHandled) {
    // Arrange
    Motor_Init(&mock_i2c);
    mock_set_i2c_status(HAL_TIMEOUT);  // Simular timeout

    // Act
    HAL_StatusTypeDef status = Motor_Forward(50);

    // Assert - Função deve retornar erro
    EXPECT_EQ(status, HAL_TIMEOUT);
}

/**
 * TESTE ADICIONAL: Conversão correta de percentagem para PWM
 */
TEST_F(MotorControlTest, PercentageToPWMConversion) {
    // Arrange
    Motor_Init(&mock_i2c);

    struct TestCase {
        uint8_t speed_percent;
        uint8_t expected_pwm;
    };

    TestCase cases[] = {
        {0,   0},
        {10,  25},
        {25,  63},
        {50,  127},
        {75,  191},
        {100, 255}
    };

    for (const auto& test : cases) {
        mock_clear_i2c_history();

        // Act
        Motor_Forward(test.speed_percent);

        // Assert
        uint8_t buffer[10];
        uint16_t size = 0;
        uint16_t address = 0;
        mock_get_i2c_last_tx(buffer, &size, &address);

        EXPECT_EQ(buffer[2], test.expected_pwm)
            << "Speed " << (int)test.speed_percent << "% should give PWM "
            << (int)test.expected_pwm;
    }
}

/**
 * TESTE ADICIONAL: Stop após Forward
 */
TEST_F(MotorControlTest, StopAfterForward) {
    // Arrange
    Motor_Init(&mock_i2c);
    Motor_Forward(75);
    EXPECT_EQ(Motor_GetCurrentSpeed(), 75);

    // Act
    Motor_Stop();

    // Assert
    EXPECT_EQ(Motor_GetCurrentSpeed(), 0);
}

/**
 * TESTE ADICIONAL: Stop após Backward
 */
TEST_F(MotorControlTest, StopAfterBackward) {
    // Arrange
    Motor_Init(&mock_i2c);
    Motor_Backward(75);
    EXPECT_EQ(Motor_GetCurrentSpeed(), 75);

    // Act
    Motor_Stop();

    // Assert
    EXPECT_EQ(Motor_GetCurrentSpeed(), 0);
}

/**
 * TESTE ADICIONAL: Comandos sequenciais
 */
TEST_F(MotorControlTest, SequentialCommands) {
    // Arrange
    Motor_Init(&mock_i2c);

    // Act & Assert - Sequência de comandos
    EXPECT_EQ(Motor_Forward(25), HAL_OK);
    EXPECT_EQ(Motor_GetCurrentSpeed(), 25);

    EXPECT_EQ(Motor_Forward(50), HAL_OK);
    EXPECT_EQ(Motor_GetCurrentSpeed(), 50);

    EXPECT_EQ(Motor_Stop(), HAL_OK);
    EXPECT_EQ(Motor_GetCurrentSpeed(), 0);

    EXPECT_EQ(Motor_Backward(75), HAL_OK);
    EXPECT_EQ(Motor_GetCurrentSpeed(), 75);

    EXPECT_EQ(Motor_Stop(), HAL_OK);
    EXPECT_EQ(Motor_GetCurrentSpeed(), 0);
}

/**
 * TESTE ADICIONAL: I2C Error durante Forward
 */
TEST_F(MotorControlTest, I2CErrorDuringForward) {
    // Arrange
    Motor_Init(&mock_i2c);
    mock_set_i2c_status(HAL_ERROR);

    // Act
    HAL_StatusTypeDef status = Motor_Forward(50);

    // Assert
    EXPECT_EQ(status, HAL_ERROR);
}

/**
 * TESTE ADICIONAL: I2C Error durante Backward
 */
TEST_F(MotorControlTest, I2CErrorDuringBackward) {
    // Arrange
    Motor_Init(&mock_i2c);
    mock_set_i2c_status(HAL_ERROR);

    // Act
    HAL_StatusTypeDef status = Motor_Backward(50);

    // Assert
    EXPECT_EQ(status, HAL_ERROR);
}

/**
 * TESTE ADICIONAL: I2C Error durante Stop
 */
TEST_F(MotorControlTest, I2CErrorDuringStop) {
    // Arrange
    Motor_Init(&mock_i2c);
    Motor_Forward(50);
    mock_set_i2c_status(HAL_ERROR);

    // Act
    HAL_StatusTypeDef status = Motor_Stop();

    // Assert
    EXPECT_EQ(status, HAL_ERROR);
}
