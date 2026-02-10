#include "./tasks/task_srf08.h"

/* ---------------------------
 * Config
 * --------------------------- */
#ifndef SRF08_PERIOD_MS
#define SRF08_PERIOD_MS 70u /* ~14Hz, sensor does not like aggressive polling */
#endif

#define SRF08_FILTER_SIZE 5
#define SRF08_LIGHT_THRESHOLD 2

/* ---------------------------
 * Task-local state
 * --------------------------- */
typedef struct {
    SRF08_HandleTypeDef hsrf08;
    HAL_StatusTypeDef init_status;

    uint32_t can_send_counter;

    /* Moving average filter */
    uint16_t distance_buf[SRF08_FILTER_SIZE];
    uint8_t  buf_idx;
    uint8_t  buf_filled;
    uint16_t distance_mm_filtered;

    /* Debug counters */
    uint8_t err_log_counter;
    uint8_t timeout_log_counter;
    uint8_t srf08_log_counter;
} Srf08TaskCtx;

static Srf08TaskCtx s_srf;

/* If these exist elsewhere, keep extern. Prefer making them part of ctx later. */
volatile uint8_t srf08_speed_limit = 100; /* Speed limit 0-100% (100=full speed) */
volatile uint8_t emergency_stop_active = 0;
volatile EmergencyStopState_t srf08_emergency_state = ESTOP_STATE_NORMAL;

void task_srf08_init(SystemCtx* ctx)
{
    sys_log(ctx, "\r\n[SRF08] Thread iniciada (SAFETY CRITICAL)!\r\n");

    s_srf.can_send_counter = 0;
    s_srf.buf_idx = 0;
    s_srf.buf_filled = 0;
    s_srf.distance_mm_filtered = 0;
    s_srf.err_log_counter = 0;
    s_srf.timeout_log_counter = 0;
    s_srf.srf08_log_counter = 0;

    for (uint8_t i = 0; i < SRF08_FILTER_SIZE; i++)
        s_srf.distance_buf[i] = 0;

    s_srf.init_status = SRF08_Init(&s_srf.hsrf08, &hi2c1, SRF08_DEFAULT_ADDR);

    if (s_srf.init_status == HAL_OK) {
        uint8_t version = SRF08_GetVersion(&s_srf.hsrf08);
        sys_log(ctx,
            "[SRF08] Sensor OK! Versao: %u | Emergency @ %u mm\r\n"
            "[SRF08] Configurado: Gain=12, Range=140 | Period=%u ms\r\n",
            version, SRF08_EMERGENCY_THRESHOLD_MM, SRF08_PERIOD_MS);
    } else {
        sys_log(ctx, "[SRF08] ERRO init! Status: %d\r\n", s_srf.init_status);
    }
}

void task_srf08_step(SystemCtx* ctx)
{
    /* Iniciar medição */
    HAL_StatusTypeDef ranging_status = SRF08_StartRanging(&s_srf.hsrf08);

    /* DEBUG: Verificar se comando foi aceite */
    if (ranging_status != HAL_OK && ++s_srf.err_log_counter >= 15) {
        s_srf.err_log_counter = 0;
        sys_log(ctx, "[SRF08] ERRO ao enviar comando ranging! Status: %d\r\n", ranging_status);
    }

    /* === AGUARDAR MEDIÇÃO COMPLETAR === */
    uint8_t ready = 0;
    uint8_t poll_attempts = 0;

    #if SRF08_DISABLE_POLLING
        tx_thread_sleep(70);  /* 70ms fixo (datasheet: 65ms min) */
        ready = 1;
        poll_attempts = 14;
    #else
    const uint8_t MAX_POLLS = 14;  /* 14 * 5ms = 70ms */
    while (!ready && poll_attempts < MAX_POLLS) {
        tx_thread_sleep(5);
        ready = SRF08_IsReady(&s_srf.hsrf08);
        poll_attempts++;
    }
    #endif

    /* Verificar se deu timeout */
    if (!ready && ++s_srf.timeout_log_counter >= 15) {
        s_srf.timeout_log_counter = 0;

        uint8_t cmd_reg = 0xFF;
        HAL_I2C_Mem_Read(s_srf.hsrf08.hi2c, s_srf.hsrf08.addr, SRF08_REG_COMMAND, 1,
                         &cmd_reg, 1, 100);

        sys_log(ctx, "[SRF08] WARNING: Timeout! Polls=%u | CMD_REG=0x%02X (esperado 0x00)\r\n",
                poll_attempts, cmd_reg);
    }

    /* Ler distância e luz - APENAS se pronto ou timeout */
    uint16_t distance_cm = SRF08_GetDistanceCm(&s_srf.hsrf08);
    uint8_t  light       = SRF08_GetLight(&s_srf.hsrf08);
    uint16_t distance_mm_raw = (distance_cm == 0xFFFF) ? 0u : (uint16_t)(distance_cm * 10u);

    /* === FILTRO DE MÉDIA MÓVEL (5 amostras) ===
       Aceitar leitura se:
       1) Light >= threshold
       2) OU distância válida (mesmo com light baixo)
       Rejeitar apenas se light=0 E distance=0 (sem eco nenhum)
    */
    if (light >= SRF08_LIGHT_THRESHOLD || distance_mm_raw > 0u) {
        s_srf.distance_buf[s_srf.buf_idx] = distance_mm_raw;
        s_srf.buf_idx = (uint8_t)((s_srf.buf_idx + 1u) % SRF08_FILTER_SIZE);
        if (!s_srf.buf_filled && s_srf.buf_idx == 0u) s_srf.buf_filled = 1u;

        uint32_t sum = 0;
        uint8_t count = s_srf.buf_filled ? SRF08_FILTER_SIZE : s_srf.buf_idx;
        for (uint8_t i = 0; i < count; i++) sum += s_srf.distance_buf[i];
        s_srf.distance_mm_filtered = (count > 0u) ? (uint16_t)(sum / count) : 0u;
    }
    /* Senão (light=0 E distance=0), manter valor filtrado anterior */

    uint16_t distance_mm = s_srf.distance_mm_filtered;

    /* DEBUG: Log valores brutos (a cada ~1s) */
    if (++s_srf.srf08_log_counter >= 15) {
        s_srf.srf08_log_counter = 0;
        if (distance_cm == 0xFFFF) {
            sys_log(ctx, "[SRF08] ERRO I2C ao ler distancia!\r\n");
        } else {
            sys_log(ctx, "[SRF08] %u mm | L=%u | SpeedLimit=%u%%\r\n",
                    distance_mm, light, (unsigned)srf08_speed_limit);
        }
    }

    /* Atualizar VehicleState */
    tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
    ctx->state.srf08_distance_mm = distance_mm;
    ctx->state.srf08_light       = light;
    ctx->state.srf08_ts          = tx_time_get();
    tx_mutex_put(&ctx->state_mutex);

    /* === SPEED CONTROL BASED ON DISTANCE === */
    if (distance_mm >= SRF08_SLOWDOWN_THRESHOLD_MM || light == 0) {
        srf08_speed_limit = 100;
    }
    else if (distance_mm <= SRF08_EMERGENCY_THRESHOLD_MM && light > 0) {
        srf08_speed_limit = 0;
    }
    else if (light > 0) {
        srf08_speed_limit = SRF08_SLOWDOWN_SPEED_PERCENT;
    }

    /* === EMERGENCY STOP LOGIC (replicado) === */
    EmergencyStopState_t new_state = srf08_emergency_state;

    if (distance_mm < SRF08_EMERGENCY_THRESHOLD_MM && light > 0) {
        if (srf08_emergency_state != ESTOP_STATE_EMERGENCY) {
            Motor_Stop();

            new_state = ESTOP_STATE_EMERGENCY;
            emergency_stop_active = 1;

            EmergencyStop_t estop_frame;
            estop_frame.active = 1;
            estop_frame.source = 1; /* SRF08 */
            estop_frame.distance_mm = distance_mm;
            estop_frame.reason = 0x01;
            estop_frame.reserved[0] = 0;
            estop_frame.reserved[1] = 0;
            estop_frame.crc = calculate_crc8((uint8_t*)&estop_frame, sizeof(estop_frame) - 1);

            mcp_send_message(CAN_ID_EMERGENCY_STOP, (uint8_t*)&estop_frame, sizeof(estop_frame));

            sys_log(ctx, "[SRF08 ESTOP!] %u mm < %u mm (L=%u)\r\n",
                    distance_mm, SRF08_EMERGENCY_THRESHOLD_MM, light);
        }
    }
    else if (srf08_emergency_state == ESTOP_STATE_EMERGENCY) {
        if (distance_mm >= SRF08_RECOVERY_THRESHOLD_MM || light == 0) {
            new_state = ESTOP_STATE_NORMAL;
            emergency_stop_active = 0;

            EmergencyStop_t estop_frame;
            estop_frame.active = 0;
            estop_frame.source = 1;
            estop_frame.distance_mm = distance_mm;
            estop_frame.reason = 0x00;
            estop_frame.reserved[0] = 0;
            estop_frame.reserved[1] = 0;
            estop_frame.crc = calculate_crc8((uint8_t*)&estop_frame, sizeof(estop_frame) - 1);

            mcp_send_message(CAN_ID_EMERGENCY_STOP, (uint8_t*)&estop_frame, sizeof(estop_frame));

            sys_log(ctx, "[SRF08] RECOVERY! Dist=%u mm L=%u (CLEAR ENVIADO)\r\n",
                    distance_mm, light);
        }
    }
    else if (distance_mm >= SRF08_RECOVERY_THRESHOLD_MM &&
             srf08_emergency_state == ESTOP_STATE_WARNING) {
        new_state = ESTOP_STATE_NORMAL;
    }
    else if (distance_mm < SRF08_RECOVERY_THRESHOLD_MM &&
             distance_mm >= SRF08_EMERGENCY_THRESHOLD_MM && light > 0) {
        if (srf08_emergency_state == ESTOP_STATE_NORMAL) {
            new_state = ESTOP_STATE_WARNING;
        }
    }

    srf08_emergency_state = new_state;

    /* === ENVIAR DADOS CAN PERIODICAMENTE (replicado) === */
    s_srf.can_send_counter++;
    if (light > 0 &&
        srf08_emergency_state != ESTOP_STATE_EMERGENCY &&
        (s_srf.can_send_counter % 1u) == 0u) {

        SRF08Distance_t srf08_frame;
        srf08_frame.distance_mm = distance_mm;
        srf08_frame.light_level = light;
        srf08_frame.gain = 0;
        srf08_frame.range_setting = 0;
        srf08_frame.reserved[0] = 0;
        srf08_frame.reserved[1] = 0;
        srf08_frame.status = 0x01;

        if (s_srf.init_status != HAL_OK) {
            srf08_frame.status |= (1 << 1);
        }

        mcp_send_message(CAN_ID_SRF08_DISTANCE, (uint8_t*)&srf08_frame, sizeof(srf08_frame));

        if ((s_srf.can_send_counter % 10u) == 0u) {
            sys_log(ctx, "[SRF08] %u mm | L=%u | State=%u\r\n",
                    distance_mm, light, (unsigned)srf08_emergency_state);
        }
    }

    /* DELAY ZERO - Loop contínuo para resposta máxima */
    //tx_thread_relinquish();
}
