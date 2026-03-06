/**
 * @file srf08.c
 * @brief Driver para sensor ultrassonico SRF-08 via I2C
 */

#include "srf08.h"
#include "system_ctx.h"

HAL_StatusTypeDef SRF08_Init(SRF08_HandleTypeDef *hsrf, I2C_HandleTypeDef *hi2c, uint8_t addr, SystemCtx* ctx)
{
    // Init com configuração recomendada por default
    return SRF08_InitWithConfig(hsrf, hi2c, addr, SRF08_RECOMMENDED_GAIN, SRF08_RECOMMENDED_RANGE, ctx);
}

HAL_StatusTypeDef SRF08_InitWithConfig(SRF08_HandleTypeDef *hsrf, I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t gain, uint8_t range, SystemCtx* ctx)
{
    hsrf->hi2c = hi2c;
    hsrf->addr = addr;

    // 1. Verifica se o sensor responde
    tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);
    HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(hi2c, addr, 3, 100);
    tx_mutex_put(&ctx->i2c1_mutex);
    if (status != HAL_OK) {
        return status;
    }

    // 2. Aguardar sensor estabilizar após power-on
    //HAL_Delay(100);

    // 3. Configurar Gain
    status = SRF08_SetGain(hsrf, gain, ctx);
    if (status != HAL_OK) {
        return status;
    }
    //HAL_Delay(10);  // Delay entre comandos

    // 4. Configurar Range
    status = SRF08_SetRange(hsrf, range, ctx);
    if (status != HAL_OK) {
        return status;
    }
    //HAL_Delay(10);

    return HAL_OK;
}

uint8_t SRF08_GetVersion(SRF08_HandleTypeDef *hsrf, SystemCtx* ctx)
{
    uint8_t version = 0;
    tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);
    HAL_I2C_Mem_Read(hsrf->hi2c, hsrf->addr, SRF08_REG_COMMAND, 1, &version, 1, 100);
    tx_mutex_put(&ctx->i2c1_mutex);
    return version;
}

uint8_t SRF08_GetLight(SRF08_HandleTypeDef *hsrf, SystemCtx* ctx)
{
    uint8_t light = 0xFF;
    tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);
    HAL_StatusTypeDef st = HAL_I2C_Mem_Read(hsrf->hi2c, hsrf->addr, SRF08_REG_LIGHT, 1, &light, 1, 100);
    tx_mutex_put(&ctx->i2c1_mutex);
    if (st != HAL_OK) return 0xFF;
    return light;
}

HAL_StatusTypeDef SRF08_StartRanging(SRF08_HandleTypeDef *hsrf, SystemCtx* ctx)
{
    uint8_t cmd = SRF08_CMD_RANGE_CM;
    tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(hsrf->hi2c, hsrf->addr, SRF08_REG_COMMAND, 1, &cmd, 1, 100);
    tx_mutex_put(&ctx->i2c1_mutex);
    return status;
}

uint8_t SRF08_IsReady(SRF08_HandleTypeDef *hsrf, SystemCtx* ctx)
{
    uint8_t status = 0xFF;  // Inicializar com valor inválido
    tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);
    HAL_StatusTypeDef hal_status = HAL_I2C_Mem_Read(hsrf->hi2c, hsrf->addr, SRF08_REG_COMMAND, 1, &status, 1, 100);
    tx_mutex_put(&ctx->i2c1_mutex);

    // Se I2C falhar, assumir que não está pronto
    if (hal_status != HAL_OK) {
        return 0;
    }

    // Sensor pronto quando reg COMMAND = 0x00
    // NOTA: Durante medição, reg mantém o comando (0x51 para cm)
    return (status == SRF08_MEASUREMENT_READY);
}

HAL_StatusTypeDef SRF08_WaitReady(SRF08_HandleTypeDef *hsrf, uint32_t timeout_ms, SystemCtx* ctx)
{
    uint32_t start_tick = HAL_GetTick();

    while ((HAL_GetTick() - start_tick) < timeout_ms)
    {
        if (SRF08_IsReady(hsrf, ctx)) {
            return HAL_OK;  // Medição completa!
        }

        HAL_Delay(SRF08_POLL_INTERVAL_MS);
    }

    return HAL_TIMEOUT;  // Timeout - medição não completou
}

uint16_t SRF08_GetDistanceCm(SRF08_HandleTypeDef *hsrf, SystemCtx* ctx)
{
    uint8_t data[2] = {0, 0};
    tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hsrf->hi2c, hsrf->addr, SRF08_REG_RANGE_H, 1, data, 2, 100);
    tx_mutex_put(&ctx->i2c1_mutex);

    // Se I2C falhar, retorna 0xFFFF para indicar erro
    if (status != HAL_OK) {
        return 0xFFFF;
    }

    return (data[0] << 8) | data[1];
}

HAL_StatusTypeDef SRF08_SetGain(SRF08_HandleTypeDef *hsrf, uint8_t gain, SystemCtx* ctx)
{
    // Gain: 0-31 (quanto maior, mais sensivel)
    // Default: 16
    // Recomendado: 16-20 para curta distancia, 25-31 para longa distancia
    if (gain > 31) gain = 31;
    tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(hsrf->hi2c, hsrf->addr, SRF08_REG_GAIN, 1, &gain, 1, 100);
    tx_mutex_put(&ctx->i2c1_mutex);
    return status;
}

HAL_StatusTypeDef SRF08_SetRange(SRF08_HandleTypeDef *hsrf, uint8_t range, SystemCtx* ctx)
{
    // Range: distancia maxima em unidades de 43mm
    // Range=255 -> 255*43mm = ~11m (maximo)
    // Range=140 -> 140*43mm = ~6m (recomendado)
    // Range=23 -> 23*43mm = ~1m (curta distancia)
    tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(hsrf->hi2c, hsrf->addr, SRF08_REG_RANGE, 1, &range, 1, 100);
    tx_mutex_put(&ctx->i2c1_mutex);
    return status;
}
