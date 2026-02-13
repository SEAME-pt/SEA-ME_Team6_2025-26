/**
  * Testes unitários para o Servo Control (MG996R)
  * Usando Google Test Framework
  */

#include <gtest/gtest.h>

extern "C" {
    #include "servo_testable.h"
    #include "stm32u5xx_hal.h"
}

/**
 * Fixture para os testes do servo
 */
class ServoTest : public ::testing::Test {
protected:
    TIM_HandleTypeDef mock_tim;

    void SetUp() override {
        // Reset mocks
        mock_set_tim_pwm_status(HAL_OK);
        mock_tim_reset();
        mock_tim_set_autoreload(20000);  // 20ms @ 1MHz

        // Initialize mock TIM
        mock_tim.Instance = nullptr;
        mock_tim.Init = 0;
        mock_tim.Channel = TIM_CHANNEL_1;

        // Reset servo state
        Servo_Reset();
    }

    void TearDown() override {
        mock_tim_reset();
    }
};

/**
 * SRV-01: Inicialização do servo (TIM1_CH1)
 */
TEST_F(ServoTest, InitializeServo) {
    // Act
    HAL_StatusTypeDef status = Servo_Init(&mock_tim, TIM_CHANNEL_1);

    // Assert
    EXPECT_EQ(status, HAL_OK);
    EXPECT_TRUE(mock_tim_is_moe_enabled()) << "MOE deve ser habilitado (Advanced Timer)";
    EXPECT_TRUE(mock_tim_is_pwm_started()) << "PWM deve ser iniciado";
    EXPECT_EQ(Servo_GetAngle(), SERVO_CENTER_ANGLE) << "Deve iniciar em 90°";
}

/**
 * SRV-02: Posição central (90°)
 */
TEST_F(ServoTest, SetCenterPosition) {
    // Arrange
    Servo_Init(&mock_tim, TIM_CHANNEL_1);

    // Act
    HAL_StatusTypeDef status = Servo_Center();

    // Assert
    EXPECT_EQ(status, HAL_OK);
    EXPECT_EQ(Servo_GetAngle(), 90);

    // Verificar PWM = 1500us (centro)
    uint32_t ccr = mock_tim_get_compare();
    EXPECT_EQ(ccr, SERVO_PULSE_CENTER_US);
}

/**
 * SRV-03: Posição mínima (0°)
 */
TEST_F(ServoTest, SetMinimumAngle) {
    // Arrange
    Servo_Init(&mock_tim, TIM_CHANNEL_1);

    // Act
    HAL_StatusTypeDef status = Servo_SetAngle(0);

    // Assert
    EXPECT_EQ(status, HAL_OK);
    EXPECT_EQ(Servo_GetAngle(), 0);

    // Verificar PWM = 1000us (mínimo)
    uint32_t ccr = mock_tim_get_compare();
    EXPECT_EQ(ccr, SERVO_PULSE_MIN_US);
}

/**
 * SRV-04: Posição máxima (180°)
 */
TEST_F(ServoTest, SetMaximumAngle) {
    // Arrange
    Servo_Init(&mock_tim, TIM_CHANNEL_1);

    // Act
    HAL_StatusTypeDef status = Servo_SetAngle(180);

    // Assert
    EXPECT_EQ(status, HAL_OK);
    EXPECT_EQ(Servo_GetAngle(), 180);

    // Verificar PWM = 2000us (máximo)
    uint32_t ccr = mock_tim_get_compare();
    EXPECT_EQ(ccr, SERVO_PULSE_MAX_US);
}

/**
 * SRV-05: Ângulo intermédio (45°)
 */
TEST_F(ServoTest, SetIntermediateAngle45) {
    // Arrange
    Servo_Init(&mock_tim, TIM_CHANNEL_1);

    // Act
    Servo_SetAngle(45);

    // Assert
    EXPECT_EQ(Servo_GetAngle(), 45);

    // Calcular PWM esperado: 1000 + (45/180) * 1000 = 1250us
    uint32_t expected_pwm = 1000 + (45 * 1000) / 180;
    uint32_t ccr = mock_tim_get_compare();
    EXPECT_EQ(ccr, expected_pwm);
}

/**
 * SRV-06: Proteção contra ângulo inválido (>180°)
 */
TEST_F(ServoTest, ClampAngleAbove180) {
    // Arrange
    Servo_Init(&mock_tim, TIM_CHANNEL_1);

    // Act - Tentar 200°
    Servo_SetAngle(200);

    // Assert - Deve ser limitado a 180°
    EXPECT_EQ(Servo_GetAngle(), 180);

    uint32_t ccr = mock_tim_get_compare();
    EXPECT_EQ(ccr, SERVO_PULSE_MAX_US);
}

/**
 * SRV-07: Conversão de steering (-100 a +100) para ângulo
 */
TEST_F(ServoTest, ConvertSteeringZeroTo90Degrees) {
    // Arrange
    Servo_Init(&mock_tim, TIM_CHANNEL_1);
    int8_t steering = 0;  // Centro

    // Act - Converter steering para ângulo
    uint8_t angle = (uint8_t)((steering + 100) * 180 / 200);
    Servo_SetAngle(angle);

    // Assert
    EXPECT_EQ(angle, 90);
    EXPECT_EQ(Servo_GetAngle(), 90);
}

/**
 * SRV-08: Conversão de steering (-100)
 */
TEST_F(ServoTest, ConvertSteeringMinus100To0Degrees) {
    // Arrange
    Servo_Init(&mock_tim, TIM_CHANNEL_1);
    int8_t steering = -100;  // Esquerda máxima

    // Act
    uint8_t angle = (uint8_t)((steering + 100) * 180 / 200);
    Servo_SetAngle(angle);

    // Assert
    EXPECT_EQ(angle, 0);
    EXPECT_EQ(Servo_GetAngle(), 0);
}

/**
 * SRV-09: Conversão de steering (+100)
 */
TEST_F(ServoTest, ConvertSteeringPlus100To180Degrees) {
    // Arrange
    Servo_Init(&mock_tim, TIM_CHANNEL_1);
    int8_t steering = +100;  // Direita máxima

    // Act
    uint8_t angle = (uint8_t)((steering + 100) * 180 / 200);
    Servo_SetAngle(angle);

    // Assert
    EXPECT_EQ(angle, 180);
    EXPECT_EQ(Servo_GetAngle(), 180);
}

/**
 * SRV-10: MOE habilitado (Advanced Timer)
 */
TEST_F(ServoTest, MOEEnabledForAdvancedTimer) {
    // Act
    Servo_Init(&mock_tim, TIM_CHANNEL_1);

    // Assert
    EXPECT_TRUE(mock_tim_is_moe_enabled())
        << "MOE (Main Output Enable) deve ser habilitado para TIM1/TIM8";
}

/**
 * TESTE ADICIONAL: Init com NULL retorna erro
 */
TEST_F(ServoTest, InitWithNullReturnsError) {
    // Act
    HAL_StatusTypeDef status = Servo_Init(NULL, TIM_CHANNEL_1);

    // Assert
    EXPECT_EQ(status, HAL_ERROR);
}

/**
 * TESTE ADICIONAL: SetAngle sem init retorna erro
 */
TEST_F(ServoTest, SetAngleWithoutInitReturnsError) {
    // Act (sem chamar Init)
    HAL_StatusTypeDef status = Servo_SetAngle(90);

    // Assert
    EXPECT_EQ(status, HAL_ERROR);
}

/**
 * TESTE ADICIONAL: Disable servo
 */
TEST_F(ServoTest, DisableServo) {
    // Arrange
    Servo_Init(&mock_tim, TIM_CHANNEL_1);
    EXPECT_TRUE(mock_tim_is_pwm_started());

    // Act
    HAL_StatusTypeDef status = Servo_Disable();

    // Assert
    EXPECT_EQ(status, HAL_OK);
    EXPECT_FALSE(mock_tim_is_pwm_started()) << "PWM deve ser parado";
}

/**
 * TESTE ADICIONAL: Múltiplas mudanças de ângulo
 */
TEST_F(ServoTest, MultipleAngleChanges) {
    // Arrange
    Servo_Init(&mock_tim, TIM_CHANNEL_1);

    // Act & Assert - Sequência de ângulos
    Servo_SetAngle(0);
    EXPECT_EQ(Servo_GetAngle(), 0);
    EXPECT_EQ(mock_tim_get_compare(), 1000u);

    Servo_SetAngle(45);
    EXPECT_EQ(Servo_GetAngle(), 45);

    Servo_SetAngle(90);
    EXPECT_EQ(Servo_GetAngle(), 90);
    EXPECT_EQ(mock_tim_get_compare(), 1500u);

    Servo_SetAngle(135);
    EXPECT_EQ(Servo_GetAngle(), 135);

    Servo_SetAngle(180);
    EXPECT_EQ(Servo_GetAngle(), 180);
    EXPECT_EQ(mock_tim_get_compare(), 2000u);
}

/**
 * TESTE ADICIONAL: Conversão linear de ângulo para PWM
 */
TEST_F(ServoTest, LinearAngleToPWMConversion) {
    // Arrange
    Servo_Init(&mock_tim, TIM_CHANNEL_1);

    struct TestCase {
        uint8_t angle;
        uint32_t expected_pwm;
    };

    TestCase cases[] = {
        {0,   1000},
        {30,  1166},
        {45,  1250},
        {60,  1333},
        {90,  1500},
        {120, 1666},
        {135, 1750},
        {150, 1833},
        {180, 2000}
    };

    for (const auto& test : cases) {
        // Act
        Servo_SetAngle(test.angle);

        // Assert
        uint32_t ccr = mock_tim_get_compare();
        EXPECT_NEAR(ccr, test.expected_pwm, 1)
            << "Angle " << (int)test.angle << "° should give PWM "
            << test.expected_pwm << "us";
    }
}

/**
 * TESTE ADICIONAL: PWM não excede período do timer
 */
TEST_F(ServoTest, PWMDoesNotExceedTimerPeriod) {
    // Arrange
    mock_tim_set_autoreload(1900);  // Período menor que 2000us (anormal mas testável)
    Servo_Init(&mock_tim, TIM_CHANNEL_1);

    // Act - Tentar 180° (que seria 2000us)
    Servo_SetAngle(180);

    // Assert - PWM deve ser limitado ao período
    uint32_t ccr = mock_tim_get_compare();
    EXPECT_LE(ccr, 1900u) << "PWM não deve exceder período do timer";
}

/**
 * TESTE ADICIONAL: TIM PWM Start falha
 */
TEST_F(ServoTest, PWMStartFailure) {
    // Arrange
    mock_set_tim_pwm_status(HAL_ERROR);

    // Act
    HAL_StatusTypeDef status = Servo_Init(&mock_tim, TIM_CHANNEL_1);

    // Assert
    EXPECT_EQ(status, HAL_ERROR);
}

/**
 * TESTE ADICIONAL: GetAngle retorna ângulo correto
 */
TEST_F(ServoTest, GetAngleReturnsCorrectValue) {
    // Arrange
    Servo_Init(&mock_tim, TIM_CHANNEL_1);

    // Act & Assert
    Servo_SetAngle(75);
    EXPECT_EQ(Servo_GetAngle(), 75);

    Servo_SetAngle(120);
    EXPECT_EQ(Servo_GetAngle(), 120);

    Servo_Center();
    EXPECT_EQ(Servo_GetAngle(), 90);
}

/**
 * TESTE ADICIONAL: Todos os canais TIM
 */
TEST_F(ServoTest, DifferentTimerChannels) {
    // Test Channel 1
    Servo_Init(&mock_tim, TIM_CHANNEL_1);
    EXPECT_TRUE(mock_tim_is_pwm_started());
    Servo_Disable();

    // Reset
    mock_tim_reset();

    // Test Channel 2
    Servo_Init(&mock_tim, TIM_CHANNEL_2);
    EXPECT_TRUE(mock_tim_is_pwm_started());
    Servo_Disable();

    // Reset
    mock_tim_reset();

    // Test Channel 3
    Servo_Init(&mock_tim, TIM_CHANNEL_3);
    EXPECT_TRUE(mock_tim_is_pwm_started());
}
