#include <gtest/gtest.h>

extern "C" {
#include "../mocks/hts221_stub.h"
}

// Funções auxiliares/mocks exportadas pelo stm32_mocks.cpp
extern "C" void mock_i2c_set_mem_status(HAL_StatusTypeDef status);
extern "C" void mock_i2c_set_whoami(uint8_t v);
extern "C" void mock_hts221_set_regs(uint8_t start, const uint8_t *buf, uint8_t len);
extern "C" void mock_hts221_clear(void);

// Teste: Inicialização bem sucedida quando o WHO_AM_I está correto
// - Configura o mock I2C para responder OK
// - Define WHO_AM_I para o valor esperado pelo driver
// - Fornece um bloco de calibração mínimo necessário para o HTS221_Init
// - Verifica que HTS221_Init() retorna HAL_OK
TEST(HTS221_Init, SuccessWhenWhoAmI_OK) {
    mock_i2c_set_mem_status(HAL_OK);
    mock_i2c_set_whoami(HTS221_ID);
    // Fornece bloco de calibração mínimo para o Init poder dar pars
    uint8_t calib[16] = {0};
    // H0_rH_x2 = 50, H1_rH_x2 = 100 (valores de humidade calibracao)
    calib[0] = 50; calib[1] = 100;
    calib[2] = 160;
    calib[3] = 240;
    calib[5] = 0x00;
    calib[6] = (1000 & 0xFF);
    calib[7] = (1000 >> 8) & 0xFF;
    calib[10] = (2000 & 0xFF);
    calib[11] = (2000 >> 8) & 0xFF;
    calib[12] = (1000 & 0xFF);
    calib[13] = (1000 >> 8) & 0xFF;
    calib[14] = (2000 & 0xFF);
    calib[15] = (2000 >> 8) & 0xFF;

    mock_hts221_set_regs(0x30, calib, 16);

    EXPECT_EQ(HTS221_Init(), HAL_OK);
}

// Teste: Inicialização falha quando WHO_AM_I está incorreto
// - Simula leitura I2C OK, mas retorna um WHO_AM_I inválido
// - Espera que HTS221_Init() propague erro (HAL_ERROR)
TEST(HTS221_Init, FailsWhenWhoAmI_Wrong) {
    mock_i2c_set_mem_status(HAL_OK);
    mock_i2c_set_whoami(0x00);
    EXPECT_EQ(HTS221_Init(), HAL_ERROR);
}

// Teste: Propagação de erro de leitura I2C durante a inicialização
// - Configura o mock I2C para devolver erro
// - Verifica que HTS221_Init() retorna HAL_ERROR
TEST(HTS221_Init, PropagatesReadError) {
    mock_i2c_set_mem_status(HAL_ERROR);
    EXPECT_EQ(HTS221_Init(), HAL_ERROR);
    mock_i2c_set_mem_status(HAL_OK);
}

// Teste: Conversão de valor bruto de humidade para percentagem
// - Configura calibração H0/H1 e seus pontos
// - Escreve um valor bruto de humidade (1500) nos registos de saída
// - Executa HTS221_Init() para carregar a calibração e depois HTS221_ReadHumidity()
// - Verifica que a humidade convertida é a esperada (~37.5%)
TEST(HTS221_ReadHumidity, ConvertsRawToPercent) {
    mock_i2c_set_mem_status(HAL_OK);
    mock_i2c_set_whoami(HTS221_ID);
    mock_hts221_clear();
    uint8_t calib[16] = {0};
    calib[0] = 50; calib[1] = 100; // H0=25%, H1=50%
    calib[6] = (1000 & 0xFF); calib[7] = (1000 >> 8) & 0xFF;
    calib[10] = (2000 & 0xFF); calib[11] = (2000 >> 8) & 0xFF;
    mock_hts221_set_regs(0x30, calib, 16);

    // Preparar bytes de humidade em 0x28 -> 1500
    uint8_t hum[2];
    int16_t hraw = 1500;
    hum[0] = hraw & 0xFF; hum[1] = (hraw >> 8) & 0xFF;
    mock_hts221_set_regs(0x28, hum, 2);

    // Executar init para popular dados de calibração
    EXPECT_EQ(HTS221_Init(), HAL_OK);

    float h = 0.0f;
    EXPECT_EQ(HTS221_ReadHumidity(&h), HAL_OK);
    EXPECT_NEAR(h, 37.5f, 1e-3f);
}

// Teste: Propagação de erro ao ler humidade
// - Configura o mock I2C para devolver erro e espera que a função também devolva HAL_ERROR
TEST(HTS221_ReadHumidity, ReturnsErrorOnReadFailure) {
    mock_i2c_set_mem_status(HAL_ERROR);
    float h;
    EXPECT_EQ(HTS221_ReadHumidity(&h), HAL_ERROR);
    mock_i2c_set_mem_status(HAL_OK);
}

// Teste: Conversão de temperatura bruta para graus Celsius
// - Configura calibração T0/T1 e seus pontos (T0_OUT, T1_OUT)
// - Escreve um valor de temperatura (1500) nos registos de saída
// - Após HTS221_Init(), chama HTS221_ReadTemperature() e verifica a conversão para 25.0°C
TEST(HTS221_ReadTemperature, ConvertsRawToDegC) {
    mock_i2c_set_mem_status(HAL_OK);
    mock_i2c_set_whoami(HTS221_ID);
    mock_hts221_clear();
    uint8_t calib[16] = {0};
    // T0_degC_x8 = 160 (20C), T1_degC_x8 = 240 (30C)
    calib[2] = 160; calib[3] = 240; calib[5] = 0x00;
    // T0_OUT = 1000, T1_OUT = 2000
    calib[12] = (1000 & 0xFF); calib[13] = (1000 >> 8) & 0xFF;
    calib[14] = (2000 & 0xFF); calib[15] = (2000 >> 8) & 0xFF;
    mock_hts221_set_regs(0x30, calib, 16);

    // Preparar bytes de temperatura em 0x2A -> 1500
    uint8_t tmp[2];
    int16_t traw = 1500;
    tmp[0] = traw & 0xFF; tmp[1] = (traw >> 8) & 0xFF;
    mock_hts221_set_regs(0x2A, tmp, 2);

    EXPECT_EQ(HTS221_Init(), HAL_OK);
    float t = 0.0f;
    EXPECT_EQ(HTS221_ReadTemperature(&t), HAL_OK);
    EXPECT_NEAR(t, 25.0f, 1e-3f);
}

// Teste: Propagação de erro ao ler temperatura
// - Configura o mock I2C para devolver erro e espera que a função também devolva HAL_ERROR
TEST(HTS221_ReadTemperature, ReturnsErrorOnReadFailure) {
    mock_i2c_set_mem_status(HAL_ERROR);
    float t;
    EXPECT_EQ(HTS221_ReadTemperature(&t), HAL_ERROR);
    mock_i2c_set_mem_status(HAL_OK);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
