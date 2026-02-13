#include <gtest/gtest.h>

extern "C" {
#include "../mocks/ism330dhcx_stub.h"
}

// Helpers/mocks exportados por stm32_mocks.cpp
// - mock_i2c_set_mem_status: força o próximo HAL_I2C_Mem_Read/Write a devolver o status indicado
// - mock_i2c_set_whoami: define o valor que será lido do registo WHO_AM_I
// - mock_i2c_set_accel_bytes: pré-carrega 6 bytes que serão retornados na leitura do acelerómetro
// - mock_i2c_set_gyro_bytes: pré-carrega 6 bytes que serão retornados na leitura do giroscópio
extern "C" void mock_i2c_set_mem_status(HAL_StatusTypeDef status);
extern "C" void mock_i2c_set_whoami(uint8_t v);
extern "C" void mock_i2c_set_accel_bytes(const uint8_t *buf);
extern "C" void mock_i2c_set_gyro_bytes(const uint8_t *buf);

// Teste: Inicialização bem sucedida quando WHO_AM_I está correcto
// - Mock do I2C devolve HAL_OK
// - WHO_AM_I devolvido pelo mock é o ID esperado
// - Verifica que ISM330DHCX_Init() retorna HAL_OK
TEST(ISM330DHCX_Init, SuccessWhenWhoAmI_OK) {
    mock_i2c_set_mem_status(HAL_OK);
    mock_i2c_set_whoami(ISM330DHCX_ID);
    EXPECT_EQ(ISM330DHCX_Init(), HAL_OK);
}

// Teste: Inicialização falha quando WHO_AM_I está incorreto
// - Mock do I2C OK, mas WHO_AM_I devolvido é inválido
// - Espera-se que o Init devolva HAL_ERROR
TEST(ISM330DHCX_Init, FailsWhenWhoAmI_Wrong) {
    mock_i2c_set_mem_status(HAL_OK);
    mock_i2c_set_whoami(0x00);
    EXPECT_EQ(ISM330DHCX_Init(), HAL_ERROR);
}

// Teste: Propagação de erro de leitura I2C durante a inicialização
// - Configura o mock para devolver HAL_ERROR e verifica que Init propaga esse erro
TEST(ISM330DHCX_Init, PropagatesReadError) {
    mock_i2c_set_mem_status(HAL_ERROR);
    EXPECT_EQ(ISM330DHCX_Init(), HAL_ERROR);
    mock_i2c_set_mem_status(HAL_OK);
}

// Teste: Leitura do acelerómetro e conversão para g
// - Pré-carrega 6 bytes representando X/Y/Z (LSB primeiro)
// - Executa ISM330DHCX_ReadAccel() e verifica a escala aplicada
TEST(ISM330DHCX_ReadAccel, ConvertsRawToG) {
    // valores brutos de exemplo: X=10000, Y=-20000, Z=32767
    int16_t rx = 10000;
    int16_t ry = -20000;
    int16_t rz = 32767;
    uint8_t buf[6];
    buf[0] = rx & 0xFF;
    buf[1] = (rx >> 8) & 0xFF;
    buf[2] = ry & 0xFF;
    buf[3] = (ry >> 8) & 0xFF;
    buf[4] = rz & 0xFF;
    buf[5] = (rz >> 8) & 0xFF;

    mock_i2c_set_mem_status(HAL_OK);
    mock_i2c_set_accel_bytes(buf);

    ISM330DHCX_AxesRaw_t accel;
    EXPECT_EQ(ISM330DHCX_ReadAccel(&accel), HAL_OK);
    EXPECT_FLOAT_EQ(accel.x, (float)rx * 0.000122f);
    EXPECT_FLOAT_EQ(accel.y, (float)ry * 0.000122f);
    EXPECT_FLOAT_EQ(accel.z, (float)rz * 0.000122f);
}

// Teste: Propagação de erro ao ler acelerómetro
// - Configura mock I2C para erro e verifica que a função devolve HAL_ERROR
TEST(ISM330DHCX_ReadAccel, ReturnsErrorOnReadFailure) {
    mock_i2c_set_mem_status(HAL_ERROR);
    ISM330DHCX_AxesRaw_t accel;
    EXPECT_EQ(ISM330DHCX_ReadAccel(&accel), HAL_ERROR);
    mock_i2c_set_mem_status(HAL_OK);
}

// Teste: Leitura do giroscópio e conversão para dps
// - Pré-carrega 6 bytes para RX/RY/RZ e verifica a conversão aplicada
TEST(ISM330DHCX_ReadGyro, ConvertsRawToDPS) {
    int16_t rx = 1000;
    int16_t ry = -2000;
    int16_t rz = 32767;
    uint8_t buf[6];
    buf[0] = rx & 0xFF;
    buf[1] = (rx >> 8) & 0xFF;
    buf[2] = ry & 0xFF;
    buf[3] = (ry >> 8) & 0xFF;
    buf[4] = rz & 0xFF;
    buf[5] = (rz >> 8) & 0xFF;

    mock_i2c_set_mem_status(HAL_OK);
    mock_i2c_set_gyro_bytes(buf);

    ISM330DHCX_AxesRaw_t gyro;
    EXPECT_EQ(ISM330DHCX_ReadGyro(&gyro), HAL_OK);
    EXPECT_FLOAT_EQ(gyro.x, (float)rx * 0.0175f);
    EXPECT_FLOAT_EQ(gyro.y, (float)ry * 0.0175f);
    EXPECT_FLOAT_EQ(gyro.z, (float)rz * 0.0175f);
}

// Teste: Propagação de erro ao ler giroscópio
// - Configura I2C mock para erro e verifica que a função devolve HAL_ERROR
TEST(ISM330DHCX_ReadGyro, ReturnsErrorOnReadFailure) {
    mock_i2c_set_mem_status(HAL_ERROR);
    ISM330DHCX_AxesRaw_t gyro;
    EXPECT_EQ(ISM330DHCX_ReadGyro(&gyro), HAL_ERROR);
    mock_i2c_set_mem_status(HAL_OK);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
