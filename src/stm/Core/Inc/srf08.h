/**
 * @file srf08.h
 * @brief Driver para sensor ultrassonico SRF-08 via I2C
 */

#ifndef INC_SRF08_H_
#define INC_SRF08_H_

#include "stm32u5xx_hal.h"
#include "system_ctx.h"

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
#define SRF08_RECOMMENDED_GAIN   1    // Gain mínimo: eco do chão mais fraco/intermitente (reduz falsos positivos)
#define SRF08_RECOMMENDED_RANGE  140  // ~6m, bom compromisso

// Distância mínima válida — leituras abaixo disto são eco do chão, não obstáculo
//
// CÁLCULO (sensor a ~138mm do chão real, feixe 55° → 27.5° de cada lado):
//   D_eco_chão = H / sin(27.5°) = 138mm / 0.462 = ~300mm
//
// !! CONFLITO FÍSICO !!
//   O eco do chão está a ~300mm.
//   O AEB quer detectar paredes a partir de ~400mm.
//   Threshold tem de ser > 300mm para rejeitar o eco do chão.
//   → Detecção mínima fiável começa em ~400mm (não 300mm).
//
// Para detectar abaixo de 400mm: elevar o sensor (objectivo H > 200mm):
//   H=200mm → eco~433mm → threshold=480mm, detecção a partir de ~480mm
//   Ou inclinar ~15° para cima → eco vai para >500mm com H=138mm
//
// Threshold = 350mm: rejeita eco do chão (300mm), aceita paredes ≥350mm
#define SRF08_MIN_VALID_DISTANCE_MM  30  // rejeita eco do chão (~300mm); sensor a ~138mm do solo, feixe 55°

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

HAL_StatusTypeDef SRF08_Init(SRF08_HandleTypeDef *hsrf, I2C_HandleTypeDef *hi2c, uint8_t addr, SystemCtx* ctx);
HAL_StatusTypeDef SRF08_InitWithConfig(SRF08_HandleTypeDef *hsrf, I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t gain, uint8_t range, SystemCtx* ctx);
uint8_t SRF08_GetVersion(SRF08_HandleTypeDef *hsrf, SystemCtx* ctx);
uint8_t SRF08_GetLight(SRF08_HandleTypeDef *hsrf, SystemCtx* ctx);
HAL_StatusTypeDef SRF08_StartRanging(SRF08_HandleTypeDef *hsrf, SystemCtx* ctx);
HAL_StatusTypeDef SRF08_WaitReady(SRF08_HandleTypeDef *hsrf, uint32_t timeout_ms, SystemCtx* ctx);
uint8_t SRF08_IsReady(SRF08_HandleTypeDef *hsrf, SystemCtx* ctx);
uint16_t SRF08_GetDistanceCm(SRF08_HandleTypeDef *hsrf, SystemCtx* ctx);
HAL_StatusTypeDef SRF08_SetGain(SRF08_HandleTypeDef *hsrf, uint8_t gain, SystemCtx* ctx);
HAL_StatusTypeDef SRF08_SetRange(SRF08_HandleTypeDef *hsrf, uint8_t range, SystemCtx* ctx);

#endif /* INC_SRF08_H_ */
