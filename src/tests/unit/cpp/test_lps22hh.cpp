#include <gtest/gtest.h>

extern "C" {
#include "../mocks/lps22hh_stub.h"
}

// Funções auxiliares/mocks exportadas pelo stm32_mocks.cpp
extern "C" void mock_i2c_set_mem_status(HAL_StatusTypeDef status);
extern "C" void mock_i2c_set_whoami(uint8_t v);
// Reutilizamos o helper genérico para escrever blocos de registos (mock_hts221_set_regs)
extern "C" void mock_hts221_set_regs(uint8_t start, const uint8_t *buf, uint8_t len);

// Teste: Inicialização bem sucedida quando WHO_AM_I está correto
// - Mock I2C devolve HAL_OK
// - WHO_AM_I devolvido é o ID esperado pelo driver
// - Verifica que LPS22HH_Init() retorna HAL_OK
TEST(LPS22HH_Init, SuccessWhenWhoAmI_OK) {
    mock_i2c_set_mem_status(HAL_OK);
    mock_i2c_set_whoami(LPS22HH_ID);
    EXPECT_EQ(LPS22HH_Init(), HAL_OK);
}

// Teste: Falha de inicialização quando WHO_AM_I está incorreto
// - Mock I2C OK, mas WHO_AM_I inválido
// - Espera-se que LPS22HH_Init() devolva HAL_ERROR
TEST(LPS22HH_Init, FailsWhenWhoAmI_Wrong) {
    mock_i2c_set_mem_status(HAL_OK);
    mock_i2c_set_whoami(0x00);
    EXPECT_EQ(LPS22HH_Init(), HAL_ERROR);
}

// Teste: Propagação de erro de leitura durante a inicialização
// - Configura mock I2C para devolver HAL_ERROR e verifica que Init propaga o erro
TEST(LPS22HH_Init, PropagatesReadError) {
    mock_i2c_set_mem_status(HAL_ERROR);
    EXPECT_EQ(LPS22HH_Init(), HAL_ERROR);
    mock_i2c_set_mem_status(HAL_OK);
}

// Teste: Conversão de temperatura bruta para °C
// - Fornece valor bruto (int16) nos registos de temperatura e verifica a fórmula
TEST(LPS22HH_ReadTemperature, ConvertsRawToDegC) {
    // Valor bruto de exemplo: 2500 -> 25.00 °C
    int16_t traw = 2500;
    uint8_t tmp[2];
    tmp[0] = traw & 0xFF; tmp[1] = (traw >> 8) & 0xFF;

    mock_i2c_set_mem_status(HAL_OK);
    mock_i2c_set_whoami(LPS22HH_ID);
    mock_hts221_set_regs(LPS22HH_TEMP_OUT_L, tmp, 2);

    float t = 0.0f;
    EXPECT_EQ(LPS22HH_ReadTemperature(&t), HAL_OK);
    EXPECT_NEAR(t, 25.00f, 1e-3f);
}

// Teste: Propagação de erro ao ler temperatura
// - Configura mock I2C para erro e verifica que a função devolve HAL_ERROR
TEST(LPS22HH_ReadTemperature, ReturnsErrorOnReadFailure) {
    mock_i2c_set_mem_status(HAL_ERROR);
    float t;
    EXPECT_EQ(LPS22HH_ReadTemperature(&t), HAL_ERROR);
    mock_i2c_set_mem_status(HAL_OK);
}

// Teste: Conversão de pressão positiva
// - Fornece 24-bit raw em três bytes e verifica a conversão feita pelo driver (raw/4096)
TEST(LPS22HH_ReadPressure, ConvertsRawToHPa_Positive) {
    int32_t praw = 101325; // apenas um número de teste
    uint8_t pbuf[3];
    pbuf[0] = praw & 0xFF;
    pbuf[1] = (praw >> 8) & 0xFF;
    pbuf[2] = (praw >> 16) & 0xFF;

    mock_i2c_set_mem_status(HAL_OK);
    mock_i2c_set_whoami(LPS22HH_ID);
    mock_hts221_set_regs(LPS22HH_PRESS_OUT_XL, pbuf, 3);

    float p = 0.0f;
    EXPECT_EQ(LPS22HH_ReadPressure(&p), HAL_OK);
    EXPECT_NEAR(p, (float)praw / 4096.0f, 1e-3f);
}

// Teste: Conversão de pressão com sinal
// - Simula um raw 24-bit negativo (bit 23 = 1) e verifica que o driver faz a extensão de sinal correta
TEST(LPS22HH_ReadPressure, ConvertsRawToHPa_NegativeSignExtend) {
    // Prepara bytes com bit de sinal definido
    uint8_t pbuf[3] = {0x00, 0x00, 0x80};
    mock_i2c_set_mem_status(HAL_OK);
    mock_i2c_set_whoami(LPS22HH_ID);
    mock_hts221_set_regs(LPS22HH_PRESS_OUT_XL, pbuf, 3);

    float p = 0.0f;
    EXPECT_EQ(LPS22HH_ReadPressure(&p), HAL_OK);
    // Após extensão de sinal e divisão por 4096 espera-se -2048
    EXPECT_NEAR(p, -2048.0f, 1e-3f);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
