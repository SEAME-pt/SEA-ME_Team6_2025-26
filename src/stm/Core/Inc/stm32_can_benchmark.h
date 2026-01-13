/**
 * @file stm32_can_benchmark.h
 * @brief CAN Bus Throughput Benchmark - Header
 * @project SEA:ME Autonomous Vehicle
 */

#ifndef STM32_CAN_BENCHMARK_H
#define STM32_CAN_BENCHMARK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tx_api.h"
#include <stdint.h>

/*============================================================================
 * CAN BENCHMARK MESSAGE IDs
 *============================================================================*/

#define CAN_ID_BENCH_CONTROL    0x7F0   /* Comandos de controlo */
#define CAN_ID_BENCH_DATA       0x7F1   /* Pacotes de teste */
#define CAN_ID_BENCH_STATS      0x7F2   /* Estatísticas */

/*============================================================================
 * CONTROL COMMANDS
 *============================================================================*/

#define BENCH_CMD_START         0x01    /* Início de teste */
#define BENCH_CMD_STOP          0x02    /* Fim de teste */
#define BENCH_CMD_REPORT        0x03    /* Relatório final */

/*============================================================================
 * PUBLIC API
 *============================================================================*/

/**
 * @brief Inicializa o módulo de benchmark (cria thread ThreadX)
 * @param byte_pool Pool de memória ThreadX (não usado, pode ser NULL)
 * @return TX_SUCCESS se sucesso
 */
UINT CAN_Benchmark_Init(TX_BYTE_POOL *byte_pool);

/**
 * @brief Dispara benchmark completo (1 a 1000 pkt/s)
 * @note Non-blocking - executa numa thread separada
 */
void CAN_Benchmark_TriggerFull(void);

/**
 * @brief Dispara teste único com taxa específica
 * @param rate Taxa em pacotes/segundo (1-1000)
 * @note Non-blocking - executa numa thread separada
 */
void CAN_Benchmark_TriggerSingle(uint32_t rate);

/**
 * @brief Executa benchmark completo (blocking)
 * @note Pode ser chamada diretamente se não quiseres usar threads
 */
void CAN_Benchmark_RunFull(void);

/**
 * @brief Executa teste único (blocking)
 * @param rate Taxa em pacotes/segundo
 */
void CAN_Benchmark_RunSingle(uint32_t rate);

#ifdef __cplusplus
}
#endif

#endif /* STM32_CAN_BENCHMARK_H */
