#include <gtest/gtest.h>

extern "C" {
#include "../mocks/iis2mdc_stub.h"
}

// Funções auxiliares/mocks exportadas pelo stm32_mocks.cpp
extern "C" void mock_i2c_set_mem_status(HAL_StatusTypeDef status);
extern "C" void mock_i2c_set_whoami(uint8_t v);
extern "C" void mock_i2c_set_mag_bytes(const uint8_t *buf);
extern "C" void mock_i2c_set_temp_bytes(const uint8_t *buf);

// Teste: Inicialização bem sucedida quando WHO_AM_I está correto
// - Configura o mock I2C para responder com HAL_OK
// - Faz o mock do WHO_AM_I com o valor esperado pelo driver
// - Verifica que IIS2MDC_Init() retorna HAL_OK
TEST(IIS2MDC_Init, SuccessWhenWhoAmI_OK) {
    mock_i2c_set_mem_status(HAL_OK);
    mock_i2c_set_whoami(IIS2MDC_ID);
    EXPECT_EQ(IIS2MDC_Init(), HAL_OK);
}

// Teste: Falha de inicialização quando WHO_AM_I está incorreto
// - Mock de leitura I2C OK, mas WHO_AM_I com valor inválido
// - Espera-se que IIS2MDC_Init() devolva HAL_ERROR
TEST(IIS2MDC_Init, FailsWhenWhoAmI_Wrong) {
    mock_i2c_set_mem_status(HAL_OK);
    mock_i2c_set_whoami(0x00);
    EXPECT_EQ(IIS2MDC_Init(), HAL_ERROR);
}

// Teste: Propagação de erro de leitura durante a inicialização
// - Configura o mock I2C para devolver erro e verifica que o Init propaga o erro
TEST(IIS2MDC_Init, PropagatesReadError) {
    mock_i2c_set_mem_status(HAL_ERROR);
    EXPECT_EQ(IIS2MDC_Init(), HAL_ERROR);
    mock_i2c_set_mem_status(HAL_OK);
}

// Teste: Leitura e conversão dos eixos magnéticos
// - Define valores brutos (int16) para X, Y e Z
// - Insere os bytes LSB/MSB no mock dos registos magnéticos
// - Chama IIS2MDC_ReadMag() e verifica a conversão por 1.5 (escala do sensor)
TEST(IIS2MDC_ReadMag, ConvertsRawToMilliGauss) {
    // valores brutos de exemplo: X=1000, Y=-2000, Z=32767
    int16_t rx = 1000;
    int16_t ry = -2000;
    int16_t rz = 32767;
    uint8_t magbuf[6];
    magbuf[0] = rx & 0xFF;
    magbuf[1] = (rx >> 8) & 0xFF;
    magbuf[2] = ry & 0xFF;
    magbuf[3] = (ry >> 8) & 0xFF;
    magbuf[4] = rz & 0xFF;
    magbuf[5] = (rz >> 8) & 0xFF;

    mock_i2c_set_mem_status(HAL_OK);
    mock_i2c_set_mag_bytes(magbuf);

    IIS2MDC_MagData_t mag;
    EXPECT_EQ(IIS2MDC_ReadMag(&mag), HAL_OK);
    // O driver aplica um fator de 1.5 para obter milliGauss
    EXPECT_FLOAT_EQ(mag.x, (float)rx * 1.5f);
    EXPECT_FLOAT_EQ(mag.y, (float)ry * 1.5f);
    EXPECT_FLOAT_EQ(mag.z, (float)rz * 1.5f);
}

// Teste: Propagação de erro ao ler dados magnéticos
// - Configura mock I2C para devolver erro e espera que a função devolva HAL_ERROR
TEST(IIS2MDC_ReadMag, ReturnsErrorOnReadFailure) {
    mock_i2c_set_mem_status(HAL_ERROR);
    IIS2MDC_MagData_t mag;
    EXPECT_EQ(IIS2MDC_ReadMag(&mag), HAL_ERROR);
    mock_i2c_set_mem_status(HAL_OK);
}

// Teste: Conversão de temperatura bruta para graus Celsius
// - Prepara um valor bruto (int16) e escreve nos bytes de temperatura do mock
// - Verifica a conversão aplicada pelo driver: temp = raw/8 + 25
TEST(IIS2MDC_ReadTemp, ConvertsRawToDegC) {
    int16_t raw = 40; // corresponde a 30°C segundo a fórmula do driver
    uint8_t buf[2];
    buf[0] = raw & 0xFF;
    buf[1] = (raw >> 8) & 0xFF;
    mock_i2c_set_temp_bytes(buf);
    float t = 0.0f;
    EXPECT_EQ(IIS2MDC_ReadTemp(&t), HAL_OK);
    EXPECT_FLOAT_EQ(t, ((float)raw / 8.0f) + 25.0f);
}

// Teste: Propagação de erro ao ler temperatura
// - Configura I2C mock para erro e verifica que a função devolve HAL_ERROR
TEST(IIS2MDC_ReadTemp, ReturnsErrorOnReadFailure) {
    mock_i2c_set_mem_status(HAL_ERROR);
    float t;
    EXPECT_EQ(IIS2MDC_ReadTemp(&t), HAL_ERROR);
    mock_i2c_set_mem_status(HAL_OK);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
