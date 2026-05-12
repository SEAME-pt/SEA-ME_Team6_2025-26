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
#define SRF08_DEFAULT_GAIN    4    // Gain default do sensor
#define SRF08_DEFAULT_RANGE   255   // Range máximo por default

// Valores recomendados para operação
//
// GANHO — principal knob para supressão de eco do chão:
//   O sinal de eco do chão é fraco porque chega de forma oblíqua (ângulo do feixe
//   + reflexão difusa). Um obstáculo frontal ao mesmo ganho devolve sinal muito
//   mais forte. Ao ajustar o ganho, o eco do chão cai abaixo do threshold de
//   detecção enquanto obstáculos directos ainda são detectados.
//
//   Gain 1  → sensor quase cego, detecta apenas reflectores muito próximos (<50cm)
//   Gain 6  → bom para ambientes interiores, alcance ~1m, eco do chão suprimido
//   Gain 8  → bom compromisso: alcance ~1.5m, adequado para AEB a 30-80cm
//   Gain 16 → default de fábrica: alcance ~3m mas capta ecos do chão e laterais
//   Gain 25+ → máxima sensibilidade, requer montagem muito cuidada
//
#define SRF08_RECOMMENDED_GAIN   8    // Bom compromisso: ~1.5m alcance, eco do chão suprimido
#define SRF08_RECOMMENDED_RANGE  140  // ~6m, bom compromisso

// Distância mínima válida — protecção à dead zone do sensor (≥3cm hardware).
//
// GEOMETRIA ACTUAL (sensor a 110mm do chão, inclinado 5° para cima, feixe ±27.5°):
//   Feixe mais baixo = 5° tilt - 27.5° half-angle = -22.5° abaixo horizontal
//   D_eco_chão (medida inclinada) = 110 / sin(22.5°) = 110 / 0.3827 ≈ 287mm
//
// O eco do chão aparece a ~287mm. SRF08_RECOMMENDED_GAIN=8 deve suprimilo
// porque o sinal oblíquo (22.5° fora do eixo + reflexão difusa do chão) é
// muito mais fraco que um eco frontal directo à mesma distância.
//
// Se ecos do chão a ~287mm persistirem nos logs (ver "[SRF08] raw=28Xmm")
// apesar de nada estar à frente, reduzir gain para 6 ou 4.
//
// INTERACÇÃO COM AEB (task_aeb.c, d_offset_m=0.10m, d_limit_m=0.90m):
//   d_eff = distance_mm/1000 - 0.10
//   Eco do chão a 287mm → d_eff = 0.187m
//   Motor_Stop threshold ≈ 21% (speed_limit < 20% em task_can_rx.c)
//   Com d_limit_m=0.90m e a_comfort_mps2=0.2:
//     v_target = sqrt(2 × 0.2 × 0.187) = 0.274 m/s → limit = 16% → Motor_Stop!
//   → Se eco do chão persistir com gain=8, o carro fica bloqueado.
//   → Solução: reduzir gain até o eco desaparecer. Não alterar d_limit_m para
//     compensar — isso apenas mascara o problema e quebra o AEB para obstáculos reais.
//
// NOTA: este threshold (50mm) NÃO filtra ecos do chão (287mm > 50mm).
// Apenas rejeita artefactos dentro da dead zone do hardware (~30mm).
#define SRF08_MIN_VALID_DISTANCE_MM  50  // dead zone protection; ver geometria acima

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
