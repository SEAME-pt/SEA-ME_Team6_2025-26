/**
 * @file srf08.h
 * @brief Driver para sensor ultrassonico SRF-08 via I2C
 */

#ifndef INC_SRF08_H_
#define INC_SRF08_H_

#include "stm32u5xx_hal.h"

// Endereco I2C padrao do SRF-08 (8-bit)
#define SRF08_DEFAULT_ADDR  0xE0

// Registos
#define SRF08_REG_COMMAND   0x00  // Escrita: comando, Leitura: versao SW
#define SRF08_REG_GAIN      0x01  // Escrita: ganho (0-31), Leitura: luz ambiente
#define SRF08_REG_RANGE     0x02  // Escrita: range max, Leitura: distancia byte alto
#define SRF08_REG_LIGHT     0x01  // Leitura: luz ambiente
#define SRF08_REG_RANGE_H   0x02  // Leitura: distancia byte alto
#define SRF08_REG_RANGE_L   0x03  // Leitura: distancia byte baixo

// Comandos de ranging
#define SRF08_CMD_RANGE_INCH  0x50  // Resultado em polegadas
#define SRF08_CMD_RANGE_CM    0x51  // Resultado em centimetros
#define SRF08_CMD_RANGE_US    0x52  // Resultado em microsegundos

// Valores default do sensor (após power-on)
#define SRF08_DEFAULT_GAIN    16    // Gain default do sensor
#define SRF08_DEFAULT_RANGE   255   // Range máximo por default

// Valores recomendados para operação
#define SRF08_RECOMMENDED_GAIN   12   // Gain médio (melhor para distâncias curtas)
#define SRF08_RECOMMENDED_RANGE  140  // ~6m, bom compromisso

// Timing e polling
#define SRF08_MAX_MEASUREMENT_TIME_MS  65   // Tempo máximo de medição (datasheet)
#define SRF08_POLL_INTERVAL_MS         5    // Intervalo entre polls
#define SRF08_MAX_POLL_ATTEMPTS        20   // Max tentativas (20 * 5ms = 100ms)
#define SRF08_MEASUREMENT_READY        0x00 // Valor do reg COMMAND quando pronto

// Opção: Desabilitar polling (alguns SRF08 podem não suportar)
// Se 1, usa apenas sleep fixo. Se 0, usa polling inteligente
#define SRF08_DISABLE_POLLING          1  // Sensor não suporta polling (CMD_REG=0x0B)

// Limites físicos do sensor
#define SRF08_MIN_DISTANCE_CM     3     // Dead zone: ~3cm
#define SRF08_MAX_DISTANCE_CM     600   // ~6m com range=140

typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint8_t addr;
} SRF08_HandleTypeDef;

HAL_StatusTypeDef SRF08_Init(SRF08_HandleTypeDef *hsrf, I2C_HandleTypeDef *hi2c, uint8_t addr);
HAL_StatusTypeDef SRF08_InitWithConfig(SRF08_HandleTypeDef *hsrf, I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t gain, uint8_t range);
uint8_t SRF08_GetVersion(SRF08_HandleTypeDef *hsrf);
uint8_t SRF08_GetLight(SRF08_HandleTypeDef *hsrf);
HAL_StatusTypeDef SRF08_StartRanging(SRF08_HandleTypeDef *hsrf);
HAL_StatusTypeDef SRF08_WaitReady(SRF08_HandleTypeDef *hsrf, uint32_t timeout_ms);
uint8_t SRF08_IsReady(SRF08_HandleTypeDef *hsrf);
uint16_t SRF08_GetDistanceCm(SRF08_HandleTypeDef *hsrf);
HAL_StatusTypeDef SRF08_SetGain(SRF08_HandleTypeDef *hsrf, uint8_t gain);
HAL_StatusTypeDef SRF08_SetRange(SRF08_HandleTypeDef *hsrf, uint8_t range);

#endif /* INC_SRF08_H_ */
