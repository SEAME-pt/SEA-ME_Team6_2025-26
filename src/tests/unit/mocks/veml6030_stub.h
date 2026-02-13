#ifndef __VEML6030_STUB_H__
#define __VEML6030_STUB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32u5xx_hal.h"


// Endereço I2C do sensor (deslocado para 8-bit como espera o HAL)
#define VEML6030_I2C_ADDR       (0x10 << 1)

// Registos relevantes para os testes
#define VEML6030_ALS_CONF       0x00  // Configuração do ALS 
#define VEML6030_ALS_WH         0x01  // Limite superior de interrupt 
#define VEML6030_ALS_WL         0x02  // Limite inferior de interrupt 
#define VEML6030_POWER_SAVING   0x03  // Registo de power saving
#define VEML6030_ALS            0x04  // Leitura ALS (2 bytes)
#define VEML6030_WHITE          0x05  // Leitura WHITE (2 bytes)
#define VEML6030_ALS_INT        0x06  // Estado de interrupt do ALS

// Bits e máscaras usados pelo código de produção para configurar o ALS
#define VEML6030_ALS_SD         (1 << 0)   // Shutdown bit (0 = active)
#define VEML6030_ALS_INT_EN     (1 << 1)   // Habilitar interrupções ALS
#define VEML6030_ALS_PERS_1     (0 << 4)   // Persistência = 1
#define VEML6030_ALS_IT_100ms   (0 << 6)   // Tempo de integração 100 ms
#define VEML6030_ALS_GAIN_1     (0 << 11)  // Ganho = 1

// O handle I2C (`hi2c2`) é esperado pelo driver de produção; nos testes
// este handle é providenciado pelo mock do HAL, por isso declaramos a
// variável externa aqui para permitir a compilação.
extern I2C_HandleTypeDef hi2c2;


// - VEML6030_Init: escreve configuração e coloca o sensor em modo ativo
// - VEML6030_ReadALS: lê contagens ALS (convertidas depois para lux)
// - VEML6030_ReadWhite: lê contagens do canal WHITE
HAL_StatusTypeDef VEML6030_Init(void);
HAL_StatusTypeDef VEML6030_ReadALS(uint16_t *lux);
HAL_StatusTypeDef VEML6030_ReadWhite(uint16_t *white);

#ifdef __cplusplus
}
#endif

#endif
#ifndef __VEML6030_STUB_H__
#define __VEML6030_STUB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32u5xx_hal.h"


// Endereço I2C do sensor (deslocado para 8-bit como espera o HAL)
#define VEML6030_I2C_ADDR       (0x10 << 1)

// Registos relevantes para os testes
#define VEML6030_ALS_CONF       0x00  // Configuração do ALS 
#define VEML6030_ALS_WH         0x01  // Limite superior de interrupt 
#define VEML6030_ALS_WL         0x02  // Limite inferior de interrupt 
#define VEML6030_POWER_SAVING   0x03  // Registo de power saving
#define VEML6030_ALS            0x04  // Leitura ALS (2 bytes)
#define VEML6030_WHITE          0x05  // Leitura WHITE (2 bytes)
#define VEML6030_ALS_INT        0x06  // Estado de interrupt do ALS

// Bits e máscaras usados pelo código de produção para configurar o ALS
#define VEML6030_ALS_SD         (1 << 0)   // Shutdown bit (0 = active)
#define VEML6030_ALS_INT_EN     (1 << 1)   // Habilitar interrupções ALS
#define VEML6030_ALS_PERS_1     (0 << 4)   // Persistência = 1
#define VEML6030_ALS_IT_100ms   (0 << 6)   // Tempo de integração 100 ms
#define VEML6030_ALS_GAIN_1     (0 << 11)  // Ganho = 1

// O handle I2C (`hi2c2`) é esperado pelo driver de produção; nos testes
// este handle é providenciado pelo mock do HAL, por isso declaramos a
// variável externa aqui para permitir a compilação.
extern I2C_HandleTypeDef hi2c2;


// - VEML6030_Init: escreve configuração e coloca o sensor em modo ativo
// - VEML6030_ReadALS: lê contagens ALS (convertidas depois para lux)
// - VEML6030_ReadWhite: lê contagens do canal WHITE
HAL_StatusTypeDef VEML6030_Init(void);
HAL_StatusTypeDef VEML6030_ReadALS(uint16_t *lux);
HAL_StatusTypeDef VEML6030_ReadWhite(uint16_t *white);

#ifdef __cplusplus
}
#endif

#endif
