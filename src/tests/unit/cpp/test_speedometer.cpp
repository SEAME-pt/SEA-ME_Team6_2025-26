/**
  * Testes unitários para o speedometer
  * Usando Google Test Framework
  */

#include <gtest/gtest.h>

extern "C" {
    #include "speedometer_testable.h"
    #include "main.h"
}

/**
 * Fixture para os testes do speedometer
 * Setup e TearDown executam antes e depois de cada teste
 */
class SpeedometerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset das variáveis globais antes de cada teste
        pulse_count = 0;
        current_speed_kmh = 0.0f;
        current_rpm = 0.0f;
    }

    void TearDown() override {
        // Cleanup após cada teste (se necessário)
    }
};

/**
 * TESTE 1: Velocidade deve ser zero quando não há pulsos
 */
TEST_F(SpeedometerTest, ZeroPulses_ShouldResultInZeroSpeed) {
    // Arrange (preparar)
    pulse_count = 0;

    // Act (executar)
    Speedometer_CalculateSpeed();

    // Assert (verificar)
    EXPECT_FLOAT_EQ(Speedometer_GetSpeed(), 0.0f);
    EXPECT_FLOAT_EQ(Speedometer_GetRPM(), 0.0f);
}

/**
 * TESTE 2: 18 pulsos = 1 rotação completa
 * Vamos testar se 18 pulsos em 1 segundo resultam em 60 RPM
 */
TEST_F(SpeedometerTest, OneRotationPerSecond_ShouldBe60RPM) {
    // Arrange
    pulse_count = 18;  // 18 pulsos = 1 rotação (ENCODER_HOLES = 18)

    // Act
    Speedometer_CalculateSpeed();

    // Assert
    // 1 rotação por segundo = 60 RPM
    EXPECT_FLOAT_EQ(Speedometer_GetRPM(), 60.0f);
}

/**
 * TESTE 3: Calcular velocidade com 18 pulsos (1 rotação/seg)
 * Velocidade esperada = perímetro da roda * rotações por segundo * 3.6 (m/s -> km/h)
 */
TEST_F(SpeedometerTest, OneRotationPerSecond_CalculatesCorrectSpeed) {
    // Arrange
    pulse_count = 18;  // 1 rotação

    // Cálculo esperado:
    // - Perímetro = PI * 0.06675 = 0.2096 m
    // - RPS = 18/18 = 1
    // - Velocidade (m/s) = 1 * 0.2096 = 0.2096 m/s
    // - Velocidade (km/h) = 0.2096 * 3.6 = 0.7546 km/h
    float expected_speed = (PI * WHEEL_DIAMETER) * 3.6f;

    // Act
    Speedometer_CalculateSpeed();

    // Assert
    EXPECT_NEAR(Speedometer_GetSpeed(), expected_speed, 0.01f);
}

/**
 * TESTE 4: 36 pulsos = 2 rotações = 120 RPM
 */
TEST_F(SpeedometerTest, TwoRotationsPerSecond_ShouldBe120RPM) {
    // Arrange
    pulse_count = 36;  // 2 rotações

    // Act
    Speedometer_CalculateSpeed();

    // Assert
    EXPECT_FLOAT_EQ(Speedometer_GetRPM(), 120.0f);
}

/**
 * TESTE 5: Velocidade maior com mais pulsos
 */
TEST_F(SpeedometerTest, MorePulses_ShouldResultInHigherSpeed) {
    // Arrange & Act - Teste com 1 rotação
    pulse_count = 18;
    Speedometer_CalculateSpeed();
    float speed_1_rotation = Speedometer_GetSpeed();

    // Arrange & Act - Teste com 2 rotações
    pulse_count = 36;
    Speedometer_CalculateSpeed();
    float speed_2_rotations = Speedometer_GetSpeed();

    // Assert - Velocidade com 2 rotações deve ser o dobro
    EXPECT_FLOAT_EQ(speed_2_rotations, speed_1_rotation * 2.0f);
}

/**
 * TESTE 6: Após o cálculo, pulse_count deve ser resetado
 */
TEST_F(SpeedometerTest, CalculateSpeed_ShouldResetPulseCount) {
    // Arrange
    pulse_count = 18;

    // Act
    Speedometer_CalculateSpeed();

    // Assert
    EXPECT_EQ(pulse_count, 0);
}

/**
 * TESTE 7: Getters devem retornar os valores calculados
 */
TEST_F(SpeedometerTest, Getters_ShouldReturnCalculatedValues) {
    // Arrange
    pulse_count = 18;

    // Act
    Speedometer_CalculateSpeed();
    float speed = Speedometer_GetSpeed();
    float rpm = Speedometer_GetRPM();

    // Assert
    EXPECT_GT(speed, 0.0f);  // Velocidade deve ser maior que zero
    EXPECT_GT(rpm, 0.0f);    // RPM deve ser maior que zero
    EXPECT_FLOAT_EQ(rpm, 60.0f);  // Deve ser exatamente 60 RPM
}

/**
 * TESTE 8: Teste com valor alto de pulsos (simulando alta velocidade)
 */
TEST_F(SpeedometerTest, HighSpeed_ShouldCalculateCorrectly) {
    // Arrange
    // 180 pulsos = 10 rotações por segundo = 600 RPM
    pulse_count = 180;

    // Act
    Speedometer_CalculateSpeed();

    // Assert
    EXPECT_FLOAT_EQ(Speedometer_GetRPM(), 600.0f);

    // Velocidade = 10 rotações/seg * perímetro * 3.6
    float expected_speed = 10.0f * (PI * WHEEL_DIAMETER) * 3.6f;
    EXPECT_NEAR(Speedometer_GetSpeed(), expected_speed, 0.01f);
}
