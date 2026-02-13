#include <gtest/gtest.h>

extern "C" {
#include "../mocks/veml6030_stub.h"
}

// Helpers/mocks exportados por stm32_mocks.cpp
extern "C" void mock_i2c_set_mem_status(HAL_StatusTypeDef status);
extern "C" void mock_veml6030_set_als(const uint8_t *buf);
extern "C" void mock_veml6030_set_white(const uint8_t *buf);
extern "C" void mock_clear_i2c_history(void);
extern "C" void mock_get_i2c_last_tx(uint8_t *buffer, uint16_t *size, uint16_t *address);

// Teste: Inicialização — verifica que o driver escreve configuração e registo de power-saving
// - Configura o mock I2C para responder OK
// - Limpa histórico I2C para inspecionar a última escrita
// - Chama VEML6030_Init() e verifica que foi escrita uma payload (pelo menos 2 bytes)
//   para o endereço do dispositivo (pode ser escrita de configuração seguida de power-saving)
TEST(VEML6030_Init, WritesConfigAndPowerSaving) {
    mock_i2c_set_mem_status(HAL_OK);
    mock_clear_i2c_history();

    EXPECT_EQ(VEML6030_Init(), HAL_OK);

    uint8_t buf[16]; uint16_t size; uint16_t addr;
    mock_get_i2c_last_tx(buf, &size, &addr);
    // A última escrita deve ser para o endereço do dispositivo e conter >=2 bytes
    EXPECT_EQ(addr, VEML6030_I2C_ADDR);
    EXPECT_GE(size, 2);
}

// Teste: Leitura ALS e conversão para lux
// - Pré-carrega o contador ALS com um valor bruto (LSB primeiro)
// - Chama VEML6030_ReadALS() e verifica a conversão usando a resolução do driver
TEST(VEML6030_ReadALS, ConvertsCountsToLux) {
    mock_i2c_set_mem_status(HAL_OK);
    // Define counts = 1000 (LSB primeiro)
    uint16_t counts = 1000;
    uint8_t a[2];
    a[0] = counts & 0xFF; a[1] = (counts >> 8) & 0xFF;
    mock_veml6030_set_als(a);

    uint16_t lux = 0;
    EXPECT_EQ(VEML6030_ReadALS(&lux), HAL_OK);
    uint16_t expected = (uint16_t)((float)counts * 0.0288f);
    EXPECT_EQ(lux, expected);
}

// Teste: Leitura do canal WHITE (retorna contagens brutas)
// - Pré-carrega os 2 bytes do canal WHITE e verifica que VEML6030_ReadWhite() os devolve corretamente
TEST(VEML6030_ReadWhite, ReturnsWhiteCounts) {
    mock_i2c_set_mem_status(HAL_OK);
    uint16_t white_counts = 0x1234;
    uint8_t w[2]; w[0] = white_counts & 0xFF; w[1] = (white_counts >> 8) & 0xFF;
    mock_veml6030_set_white(w);

    uint16_t white = 0;
    EXPECT_EQ(VEML6030_ReadWhite(&white), HAL_OK);
    EXPECT_EQ(white, white_counts);
}

// Teste: Propagação de erro na leitura ALS
// - Configura mock I2C para devolver HAL_ERROR e verifica que VEML6030_ReadALS() propaga o erro
TEST(VEML6030_ReadALS, PropagatesReadError) {
    mock_i2c_set_mem_status(HAL_ERROR);
    uint16_t lux;
    EXPECT_EQ(VEML6030_ReadALS(&lux), HAL_ERROR);
    mock_i2c_set_mem_status(HAL_OK);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
