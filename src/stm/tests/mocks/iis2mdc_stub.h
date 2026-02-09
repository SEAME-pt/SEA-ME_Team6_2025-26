#ifndef TEST_IIS2MDC_STUB_H
#define TEST_IIS2MDC_STUB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32u5xx_hal.h"

// Este ficheiro é um "stub" para testes unitários do driver IIS2MDC.
// Fornece apenas as definições mínimas (endereços, registos e tipos)
// necessárias para compilar e testar o driver no host, sem as
// dependências completas do SDK STM32.

// Endereços dos registos utilizados pelo driver
#define IIS2MDC_WHO_AM_I        0x4F
#define IIS2MDC_CFG_REG_A       0x60
#define IIS2MDC_CFG_REG_B       0x61
#define IIS2MDC_CFG_REG_C       0x62
#define IIS2MDC_OUTX_L          0x68
#define IIS2MDC_TEMP_OUT_L      0x6E

// Endereço I2C (já deslocado para 8-bit como no HAL)
#define IIS2MDC_I2C_ADDR        (0x1E << 1)

// Valor esperado no registo WHO_AM_I para identificação do sensor
#define IIS2MDC_ID              0x40

// Tipo simples para armazenar os dados magnéticos convertidos (em mG
// ou outra unidade que o driver usa). Os testes verificam valores
// convertidos a partir dos bytes brutos fornecidos pelo mock.
typedef struct {
    float x; // campo magnético X convertido
    float y; // campo magnético Y convertido
    float z; // campo magnético Z convertido
} IIS2MDC_MagData_t;

// Handle I2C fornecido pelo mock dos testes (o driver produzional
// espera uma variável externa `hi2c2`). O stub declara-a aqui para
// que a compilação seja bem-sucedida ao ligar com os mocks.
extern I2C_HandleTypeDef hi2c2;

// Protótipos do driver expostos aos testes:
// - IIS2MDC_Init: inicializa o sensor e verifica WHO_AM_I
// - IIS2MDC_ReadMag: lê os bytes brutos e converte para valores float
// - IIS2MDC_ReadTemp: lê temperatura e converte para graus Celsius
HAL_StatusTypeDef IIS2MDC_Init(void);
HAL_StatusTypeDef IIS2MDC_ReadMag(IIS2MDC_MagData_t *mag);
HAL_StatusTypeDef IIS2MDC_ReadTemp(float *temp);

#ifdef __cplusplus
}
#endif

#endif
