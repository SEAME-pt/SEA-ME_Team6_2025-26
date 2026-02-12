#ifndef TEST_LPS22HH_STUB_H
#define TEST_LPS22HH_STUB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32u5xx_hal.h"

// Endereço I2C do sensor (8-bit como usado pelo HAL) e registos relevantes
#define LPS22HH_I2C_ADDR        (0x5D << 1)
#define LPS22HH_WHO_AM_I        0x0F
#define LPS22HH_CTRL_REG1       0x10
#define LPS22HH_CTRL_REG2       0x11
#define LPS22HH_CTRL_REG3       0x12
#define LPS22HH_STATUS          0x27

// Registos de saída: pressão e temperatura (LSB..MSB)
#define LPS22HH_PRESS_OUT_XL    0x28
#define LPS22HH_PRESS_OUT_L     0x29
#define LPS22HH_PRESS_OUT_H     0x2A
#define LPS22HH_TEMP_OUT_L      0x2B
#define LPS22HH_TEMP_OUT_H      0x2C

// Valor esperado em WHO_AM_I para identificar o sensor
#define LPS22HH_ID              0xB3

// O driver de produção usa um handle I2C externo (`hi2c2`). Nos testes,
// este handle é fornecido pelo mock, por isso declaramos a extern aqui
// para permitir a compilação do código do driver.
extern I2C_HandleTypeDef hi2c2;

// - LPS22HH_Init: inicializa o sensor e faz a verificação WHO_AM_I
// - LPS22HH_ReadTemperature: lê e converte a temperatura em °C
// - LPS22HH_ReadPressure: lê e converte a pressão em hPa (ou unidade usada)
HAL_StatusTypeDef LPS22HH_Init(void);
HAL_StatusTypeDef LPS22HH_ReadTemperature(float *temperature);
HAL_StatusTypeDef LPS22HH_ReadPressure(float *pressure);

#ifdef __cplusplus
}
#endif

#endif
#ifndef TEST_LPS22HH_STUB_H
#define TEST_LPS22HH_STUB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32u5xx_hal.h"

// Endereço I2C do sensor (8-bit como usado pelo HAL) e registos relevantes
#define LPS22HH_I2C_ADDR        (0x5D << 1)
#define LPS22HH_WHO_AM_I        0x0F
#define LPS22HH_CTRL_REG1       0x10
#define LPS22HH_CTRL_REG2       0x11
#define LPS22HH_CTRL_REG3       0x12
#define LPS22HH_STATUS          0x27

// Registos de saída: pressão e temperatura (LSB..MSB)
#define LPS22HH_PRESS_OUT_XL    0x28
#define LPS22HH_PRESS_OUT_L     0x29
#define LPS22HH_PRESS_OUT_H     0x2A
#define LPS22HH_TEMP_OUT_L      0x2B
#define LPS22HH_TEMP_OUT_H      0x2C

// Valor esperado em WHO_AM_I para identificar o sensor
#define LPS22HH_ID              0xB3

// O driver de produção usa um handle I2C externo (`hi2c2`). Nos testes,
// este handle é fornecido pelo mock, por isso declaramos a extern aqui
// para permitir a compilação do código do driver.
extern I2C_HandleTypeDef hi2c2;

// - LPS22HH_Init: inicializa o sensor e faz a verificação WHO_AM_I
// - LPS22HH_ReadTemperature: lê e converte a temperatura em °C
// - LPS22HH_ReadPressure: lê e converte a pressão em hPa (ou unidade usada)
HAL_StatusTypeDef LPS22HH_Init(void);
HAL_StatusTypeDef LPS22HH_ReadTemperature(float *temperature);
HAL_StatusTypeDef LPS22HH_ReadPressure(float *pressure);

#ifdef __cplusplus
}
#endif

#endif
