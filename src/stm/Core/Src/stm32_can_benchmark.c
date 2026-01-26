/**
 * @file stm32_can_benchmark.c
 * @brief CAN Bus Throughput Benchmark - Versão com Timing Preciso
 * @project SEA:ME Autonomous Vehicle
 * 
 * @description
 * Versão corrigida com cálculo preciso de intervalos usando microssegundos.
 * Resolve o problema da divisão inteira que causava envio de pacotes extra.
 */

#include "main.h"
#include "mcp2515.h"
#include "tx_api.h"
#include <string.h>
#include <stdio.h>

/*============================================================================
 * CONFIGURAÇÃO
 *============================================================================*/

#define CAN_ID_BENCH_CONTROL    0x7F0
#define CAN_ID_BENCH_DATA       0x7F1
#define CAN_ID_BENCH_STATS      0x7F2

#define BENCH_CMD_START         0x01
#define BENCH_CMD_STOP          0x02
#define BENCH_CMD_REPORT        0x03

#define BENCH_MAX_RATE          1500    /* Testar até 1500 para ver limite */
#define BENCH_STEP_SIZE         50
#define BENCH_TEST_DURATION_MS  1000

/*============================================================================
 * MCP2515 REGISTERS
 *============================================================================*/

#define TXB0CTRL    0x30
#define TXB0SIDH    0x31
#define TXB1CTRL    0x40
#define TXB1SIDH    0x41
#define TXB2CTRL    0x50
#define TXB2SIDH    0x51
#define TXREQ       0x08

#define MCP_RTS_TX0     0x81
#define MCP_RTS_TX1     0x82
#define MCP_RTS_TX2     0x84
#define MCP_LOAD_TX0    0x40

/*============================================================================
 * EXTERNAL REFERENCES
 *============================================================================*/

extern SPI_HandleTypeDef hspi1;
extern TX_MUTEX printf_mutex;

#define MCP_CS_L() HAL_GPIO_WritePin(CS_PIN_GPIO_Port, CS_PIN_Pin, GPIO_PIN_RESET)
#define MCP_CS_H() HAL_GPIO_WritePin(CS_PIN_GPIO_Port, CS_PIN_Pin, GPIO_PIN_SET)

/*============================================================================
 * MICROSECOND TIMER (usando DWT cycle counter)
 *============================================================================*/

static volatile uint32_t g_cpu_freq = 160000000;  /* 160 MHz default */

static void DWT_Init(void)
{
    /* Habilitar DWT cycle counter */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    /* Obter frequência real do CPU */
    g_cpu_freq = HAL_RCC_GetHCLKFreq();
}

static inline uint32_t DWT_GetMicros(void)
{
    return DWT->CYCCNT / (g_cpu_freq / 1000000);
}

static inline void DelayMicros(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t cycles = us * (g_cpu_freq / 1000000);
    while ((DWT->CYCCNT - start) < cycles);
}

/*============================================================================
 * DATA STRUCTURES
 *============================================================================*/

typedef struct {
    uint32_t target_rate;
    uint32_t packets_sent;
    uint32_t packets_failed;
    uint32_t duration_ms;
    float actual_rate;
} BenchResult_t;

/*============================================================================
 * MCP2515 TX FUNCTIONS
 *============================================================================*/

static uint8_t MCP2515_TxBufferFree(uint8_t buffer)
{
    uint8_t ctrl_reg;
    switch(buffer) {
        case 0: ctrl_reg = TXB0CTRL; break;
        case 1: ctrl_reg = TXB1CTRL; break;
        case 2: ctrl_reg = TXB2CTRL; break;
        default: return 0;
    }
    return !(MCP2515_ReadRegister(ctrl_reg) & TXREQ);
}

static HAL_StatusTypeDef MCP2515_SendFrameFast(uint16_t id, uint8_t *data, uint8_t len)
{
    /* Verificar se TX0 está livre */
    if (!MCP2515_TxBufferFree(0)) {
        if (!MCP2515_TxBufferFree(1)) {
            if (!MCP2515_TxBufferFree(2)) {
                return HAL_BUSY;
            }
            /* Usar TX2 */
            MCP2515_WriteRegister(TXB2SIDH, (id >> 3) & 0xFF);
            MCP2515_WriteRegister(TXB2SIDH + 1, (id << 5) & 0xE0);
            MCP2515_WriteRegister(TXB2SIDH + 2, 0x00);
            MCP2515_WriteRegister(TXB2SIDH + 3, 0x00);
            MCP2515_WriteRegister(TXB2SIDH + 4, len);
            for (uint8_t i = 0; i < len; i++)
                MCP2515_WriteRegister(TXB2SIDH + 5 + i, data[i]);
            uint8_t cmd = MCP_RTS_TX2;
            MCP_CS_L();
            HAL_SPI_Transmit(&hspi1, &cmd, 1, 10);
            MCP_CS_H();
            return HAL_OK;
        }
        /* Usar TX1 */
        MCP2515_WriteRegister(TXB1SIDH, (id >> 3) & 0xFF);
        MCP2515_WriteRegister(TXB1SIDH + 1, (id << 5) & 0xE0);
        MCP2515_WriteRegister(TXB1SIDH + 2, 0x00);
        MCP2515_WriteRegister(TXB1SIDH + 3, 0x00);
        MCP2515_WriteRegister(TXB1SIDH + 4, len);
        for (uint8_t i = 0; i < len; i++)
            MCP2515_WriteRegister(TXB1SIDH + 5 + i, data[i]);
        uint8_t cmd = MCP_RTS_TX1;
        MCP_CS_L();
        HAL_SPI_Transmit(&hspi1, &cmd, 1, 10);
        MCP_CS_H();
        return HAL_OK;
    }
    
    /* Usar TX0 com Load TX Buffer (mais rápido) */
    uint8_t buf[13];
    buf[0] = (id >> 3) & 0xFF;
    buf[1] = (id << 5) & 0xE0;
    buf[2] = 0x00;
    buf[3] = 0x00;
    buf[4] = len & 0x0F;
    for (uint8_t i = 0; i < len && i < 8; i++)
        buf[5 + i] = data[i];
    
    uint8_t cmd = MCP_LOAD_TX0;
    MCP_CS_L();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 10);
    HAL_SPI_Transmit(&hspi1, buf, 5 + len, 10);
    MCP_CS_H();
    
    cmd = MCP_RTS_TX0;
    MCP_CS_L();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 10);
    MCP_CS_H();
    
    return HAL_OK;
}

/*============================================================================
 * BENCHMARK FUNCTIONS
 *============================================================================*/

static void SendBenchControl(uint8_t cmd, uint32_t d1, uint32_t d2)
{
    uint8_t data[8] = {
        cmd,
        (d1 >> 16) & 0xFF, (d1 >> 8) & 0xFF, d1 & 0xFF,
        (d2 >> 16) & 0xFF, (d2 >> 8) & 0xFF, d2 & 0xFF, 0
    };
    MCP2515_SendFrameFast(CAN_ID_BENCH_CONTROL, data, 8);
}

static void SendBenchStats(BenchResult_t *r)
{
    uint8_t data[8];
    uint16_t rate_int = (uint16_t)r->actual_rate;
    
    data[0] = (r->target_rate >> 8) & 0xFF;
    data[1] = r->target_rate & 0xFF;
    data[2] = (r->packets_sent >> 8) & 0xFF;
    data[3] = r->packets_sent & 0xFF;
    data[4] = (r->packets_failed >> 8) & 0xFF;
    data[5] = r->packets_failed & 0xFF;
    data[6] = (rate_int >> 8) & 0xFF;
    data[7] = rate_int & 0xFF;
    
    MCP2515_SendFrameFast(CAN_ID_BENCH_STATS, data, 8);
}

/**
 * @brief Executa teste com timing preciso em microssegundos
 */
static void RunTestPrecise(uint32_t rate, BenchResult_t *res)
{
    memset(res, 0, sizeof(BenchResult_t));
    res->target_rate = rate;
    
    /* Calcular intervalo em MICROSSEGUNDOS para precisão */
    uint32_t interval_us = 1000000 / rate;  /* Ex: 100 Hz -> 10000 us */
    
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[BENCH] Target: %lu pkt/s (intervalo: %lu us)\r\n", rate, interval_us);
    tx_mutex_put(&printf_mutex);
    
    /* Notificar início */
    SendBenchControl(BENCH_CMD_START, rate, BENCH_TEST_DURATION_MS);
    tx_thread_sleep(1);

    /* Calcular quantos pacotes enviar (exatamente!) */
    uint32_t total_packets = rate;  /* Em 1 segundo, enviar 'rate' pacotes */
    
    uint32_t start_ms = HAL_GetTick();
    uint32_t start_us = DWT_GetMicros();
    uint32_t seq = 0;
    
    /* Enviar exatamente 'total_packets' com espaçamento uniforme */
    for (uint32_t i = 0; i < total_packets; i++)
    {
        /* Calcular quando este pacote deve ser enviado */
        uint32_t target_time_us = (uint32_t)(((uint64_t)i * 1000000) / rate);

        /* Esperar até ao momento certo */
        while ((DWT_GetMicros() - start_us) < target_time_us)
        {
            /* Busy wait - mais preciso que sleep */
        }

        /* Preparar dados */
        uint8_t data[8];
        uint32_t now = HAL_GetTick();
        data[0] = (seq >> 24) & 0xFF;
        data[1] = (seq >> 16) & 0xFF;
        data[2] = (seq >> 8) & 0xFF;
        data[3] = seq & 0xFF;
        data[4] = (now >> 24) & 0xFF;
        data[5] = (now >> 16) & 0xFF;
        data[6] = (now >> 8) & 0xFF;
        data[7] = now & 0xFF;

        /* Enviar */
        HAL_StatusTypeDef status = MCP2515_SendFrameFast(CAN_ID_BENCH_DATA, data, 8);
        
        if (status == HAL_OK) {
            res->packets_sent++;
        } else {
            res->packets_failed++;
        }

        seq++;

        /* Verificar timeout (proteção) */
        if ((HAL_GetTick() - start_ms) > (BENCH_TEST_DURATION_MS + 500)) {
            break;
        }
    }
    
    uint32_t end_ms = HAL_GetTick();
    res->duration_ms = end_ms - start_ms;
    
    /* Calcular taxa real */
    if (res->duration_ms > 0) {
        res->actual_rate = (float)res->packets_sent * 1000.0f / (float)res->duration_ms;
    }
    
    /* Notificar fim */
    SendBenchControl(BENCH_CMD_STOP, res->packets_sent, res->packets_failed);
    tx_thread_sleep(1);
    SendBenchStats(res);
    
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("  -> Enviados: %lu/%lu, Falhas: %lu, Taxa: %.1f pkt/s, Duracao: %lu ms\r\n",
           res->packets_sent, total_packets, res->packets_failed,
           res->actual_rate, res->duration_ms);
    tx_mutex_put(&printf_mutex);
}

/*============================================================================
 * PUBLIC API
 *============================================================================*/

void CAN_Benchmark_RunFull(void)
{
    /* Inicializar timer de microssegundos */
    DWT_Init();

    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("\r\n");
    printf("================================================================\r\n");
    printf("    SEA:ME CAN THROUGHPUT BENCHMARK v2.0 (Timing Preciso)\r\n");
    printf("================================================================\r\n");
    printf("  CPU: %lu MHz | Bitrate: 500 kbps | Teste: 1s por taxa\r\n", g_cpu_freq/1000000);
    printf("================================================================\r\n\r\n");
    tx_mutex_put(&printf_mutex);
    
    BenchResult_t results[32];
    int count = 0;
    
    /* Testes: 1, 10, 25, 50, 100, 150, ... 1000, 1100, 1200 */
    uint32_t test_rates[] = {1, 10, 25, 50, 75, 100, 150, 200, 250, 300,
                             350, 400, 450, 500, 550, 600, 650, 700, 750,
                             800, 850, 900, 950, 1000, 1050, 1100, 1150, 1200};
    int num_tests = sizeof(test_rates) / sizeof(test_rates[0]);
    
    for (int i = 0; i < num_tests && count < 32; i++)
    {
        RunTestPrecise(test_rates[i], &results[count]);
        count++;

        /* Pausa entre testes */
        tx_thread_sleep(20);  /* 200ms */

        /* Se perda > 20%, parar (encontrámos o limite) */
        if (results[count-1].packets_failed > results[count-1].packets_sent / 5) {
            tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
            printf("\n[INFO] Parando - perda > 20%%\r\n");
            tx_mutex_put(&printf_mutex);
            break;
        }
    }
    
    /* Imprimir resumo */
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("\r\n");
    printf("================================================================\r\n");
    printf("                        RESUMO\r\n");
    printf("================================================================\r\n");
    printf(" Target | Enviados | Falhas |   Taxa Real   | Precisao\r\n");
    printf("--------|----------|--------|---------------|----------\r\n");
    
    for (int i = 0; i < count; i++)
    {
        float precision = 0;
        if (results[i].target_rate > 0) {
            precision = (results[i].actual_rate / results[i].target_rate) * 100.0f;
        }
        
        printf(" %5lu  |  %6lu  |  %4lu  | %7.1f pkt/s |  %5.1f%%\r\n",
               results[i].target_rate,
               results[i].packets_sent,
               results[i].packets_failed,
               results[i].actual_rate,
               precision);
    }
    
    printf("================================================================\r\n");
    
    /* Encontrar limite prático (onde falhas começam) */
    uint32_t max_reliable = 0;
    for (int i = 0; i < count; i++) {
        if (results[i].packets_failed == 0) {
            max_reliable = results[i].target_rate;
        }
    }
    
    printf("\n[RESULTADO] Taxa maxima fiavel (0%% perda): %lu pkt/s\r\n", max_reliable);
    printf("================================================================\r\n\r\n");
    tx_mutex_put(&printf_mutex);
    
    SendBenchControl(BENCH_CMD_REPORT, count, max_reliable);
}

void CAN_Benchmark_RunSingle(uint32_t rate)
{
    DWT_Init();

    if (rate < 1) rate = 1;
    if (rate > BENCH_MAX_RATE) rate = BENCH_MAX_RATE;
    
    BenchResult_t res;
    RunTestPrecise(rate, &res);
}

/*============================================================================
 * THREADX INTEGRATION
 *============================================================================*/

static TX_THREAD bench_thread;
static UCHAR bench_stack[2048];
static TX_SEMAPHORE bench_sem;
static volatile uint8_t bench_full = 0;
static volatile uint32_t bench_rate = 0;

static void Benchmark_Thread_Entry(ULONG arg)
{
    (void)arg;
    
    tx_mutex_get(&printf_mutex, TX_WAIT_FOREVER);
    printf("[BENCH] Thread iniciada\r\n");
    tx_mutex_put(&printf_mutex);
    
    while (1)
    {
        tx_semaphore_get(&bench_sem, TX_WAIT_FOREVER);
        
        if (bench_full) {
            CAN_Benchmark_RunFull();
            bench_full = 0;
        } else if (bench_rate > 0) {
            CAN_Benchmark_RunSingle(bench_rate);
            bench_rate = 0;
        }
    }
}

UINT CAN_Benchmark_Init(TX_BYTE_POOL *pool)
{
    (void)pool;
    UINT status;
    
    status = tx_semaphore_create(&bench_sem, "bench_sem", 0);
    if (status != TX_SUCCESS) return status;
    
    status = tx_thread_create(&bench_thread,
                              "Benchmark",
                              Benchmark_Thread_Entry,
                              0,
                              bench_stack,
                              sizeof(bench_stack),
                              12,
                              12,
                              TX_NO_TIME_SLICE,
                              TX_AUTO_START);
    
    return status;
}

void CAN_Benchmark_TriggerFull(void)
{
    bench_full = 1;
    tx_semaphore_put(&bench_sem);
}

void CAN_Benchmark_TriggerSingle(uint32_t rate)
{
    bench_rate = rate;
    bench_full = 0;
    tx_semaphore_put(&bench_sem);
}
