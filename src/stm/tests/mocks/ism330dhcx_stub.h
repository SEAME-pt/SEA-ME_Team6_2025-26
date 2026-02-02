#ifndef TEST_ISM330DHCX_STUB_H
#define TEST_ISM330DHCX_STUB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32u5xx_hal.h"

// Registos importantes referenciados pelo driver (endereços relativos)
#define ISM330DHCX_WHO_AM_I        0x0F
#define ISM330DHCX_OUTX_L_G        0x22 // Primeiro byte do eixo X do giroscópio
#define ISM330DHCX_OUTX_L_A        0x28 // Primeiro byte do eixo X do acelerómetro

// Endereço I2C do dispositivo (deslocado para 8-bit como usa o HAL)
#define ISM330DHCX_I2C_ADDR        (0x6B << 1)

// Valor esperado no registo WHO_AM_I para identificação
#define ISM330DHCX_ID              0x6B

// Estrutura simples para representar eixos convertidos (o driver
// converte os bytes brutos em valores float em unidades apropriadas)
typedef struct {
    float x; // componente X (em g ou dps consoante o sensor)
    float y; // componente Y
    float z; // componente Z
} ISM330DHCX_AxesRaw_t;

extern I2C_HandleTypeDef hi2c2;


// - ISM330DHCX_Init: inicializa o sensor e verifica WHO_AM_I
// - ISM330DHCX_ReadAccel: lê e converte dados do acelerómetro
// - ISM330DHCX_ReadGyro: lê e converte dados do giroscópio
HAL_StatusTypeDef ISM330DHCX_Init(void);
HAL_StatusTypeDef ISM330DHCX_ReadAccel(ISM330DHCX_AxesRaw_t *accel);
HAL_StatusTypeDef ISM330DHCX_ReadGyro(ISM330DHCX_AxesRaw_t *gyro);

#ifdef __cplusplus
}
#endif

#endif
