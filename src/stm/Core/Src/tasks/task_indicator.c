/**
 ******************************************************************************
 * @file    task_indicator.c
 * @brief   ThreadX task for KS0064 indicator lights
 ******************************************************************************
 */

#include "tasks/task_indicator.h"
#include "ks0064.h"
#include "i2c.h"
#include "sys_helpers.h"

/* -------------------------------------------------------------------------
 * Timing configuration
 * -------------------------------------------------------------------------*/
#define BLINK_SLOW_HALF_MS   500U   /* Turn signal  – 500 ms on / 500 ms off  */
#define BLINK_FAST_HALF_MS   150U   /* Hazard alert – 150 ms on / 150 ms off  */
#define TASK_STEP_MS          50U   /* Task wakes every 50 ms                 */

/* -------------------------------------------------------------------------
 * Private state
 * -------------------------------------------------------------------------*/
static TX_MUTEX              s_mutex;
static volatile IndicatorState_t s_state = INDICATOR_OFF;

/* -------------------------------------------------------------------------
 * Thread-safe state accessors (public)
 * -------------------------------------------------------------------------*/
void task_indicator_set_state(IndicatorState_t state)
{
    tx_mutex_get(&s_mutex, TX_WAIT_FOREVER);
    s_state = state;
    tx_mutex_put(&s_mutex);
}

IndicatorState_t task_indicator_get_state(void)
{
    IndicatorState_t s;
    tx_mutex_get(&s_mutex, TX_WAIT_FOREVER);
    s = s_state;
    tx_mutex_put(&s_mutex);
    return s;
}

/* -------------------------------------------------------------------------
 * Task lifecycle
 * -------------------------------------------------------------------------*/
void task_indicator_init(SystemCtx *ctx)
{
    tx_mutex_create(&s_mutex, "Indicator Mutex", TX_NO_INHERIT);

    tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);
    KS0064_Init(&hi2c1, KS0064_ADDR_LEFT);
    KS0064_Init(&hi2c1, KS0064_ADDR_RIGHT);
    tx_mutex_put(&ctx->i2c1_mutex);

    sys_log(ctx, "[Indicator] KS0064 inicializados (L=0x%02X R=0x%02X)",
            KS0064_ADDR_LEFT, KS0064_ADDR_RIGHT);
}

static const char *indicator_name(IndicatorState_t s)
{
    switch (s)
    {
        case INDICATOR_OFF:          return "OFF";
        case INDICATOR_LEFT_BLINK:   return "PISCA_ESQ";
        case INDICATOR_RIGHT_BLINK:  return "PISCA_DIR";
        case INDICATOR_HEADLIGHTS:   return "FAROIS";
        case INDICATOR_ALERT:        return "ALERTA";
        default:                     return "?";
    }
}

void task_indicator_step(SystemCtx *ctx)
{
    static IndicatorState_t prev_state  = INDICATOR_OFF;
    static uint32_t         blink_tick  = 0U;
    static uint32_t         alive_tick  = 0U;
    static uint8_t          led_on      = 0U;
    uint8_t                 needs_update = 0U;

    IndicatorState_t state = task_indicator_get_state();

    /* ---- Detect state change — print immediately ---- */
    if (state != prev_state)
    {
        IndicatorState_t old = prev_state;
        blink_tick   = 0U;
        led_on       = (state == INDICATOR_HEADLIGHTS) ? 1U : 0U;
        prev_state   = state;
        needs_update = 1U;
        sys_log(ctx, "[Indicator] Estado: %s -> %s (led_on=%u)",
                indicator_name(old), indicator_name(state), led_on);
    }

    /* ---- Periodic alive log every 4 s ---- */
    alive_tick += TASK_STEP_MS;
    if (alive_tick >= 4000U)
    {
        alive_tick = 0U;
        sys_log(ctx, "[Indicator] Alive | estado=%s led_on=%u",
                indicator_name(state), led_on);
    }

    /* ---- Advance blink timer ---- */
    blink_tick += TASK_STEP_MS;

    uint32_t half_period = 0U;
    if (state == INDICATOR_LEFT_BLINK || state == INDICATOR_RIGHT_BLINK)
    {
        half_period = BLINK_SLOW_HALF_MS;
    }
    else if (state == INDICATOR_ALERT)
    {
        half_period = BLINK_FAST_HALF_MS;
    }

    if (half_period > 0U && blink_tick >= half_period)
    {
        led_on      ^= 1U;
        blink_tick   = 0U;
        needs_update = 1U;
    }

    /* ---- Drive LEDs only when something changed ---- */
    if (needs_update)
    {
        tx_mutex_get(&ctx->i2c1_mutex, TX_WAIT_FOREVER);

        switch (state)
        {
            case INDICATOR_OFF:
                KS0064_SetAll(&hi2c1, KS0064_ADDR_LEFT,  0U);
                KS0064_SetAll(&hi2c1, KS0064_ADDR_RIGHT, 0U);
                break;

            case INDICATOR_LEFT_BLINK:
                KS0064_SetAll(&hi2c1, KS0064_ADDR_LEFT,  led_on);
                KS0064_SetAll(&hi2c1, KS0064_ADDR_RIGHT, 0U);
                break;

            case INDICATOR_RIGHT_BLINK:
                KS0064_SetAll(&hi2c1, KS0064_ADDR_LEFT,  0U);
                KS0064_SetAll(&hi2c1, KS0064_ADDR_RIGHT, led_on);
                break;

            case INDICATOR_HEADLIGHTS:
                KS0064_SetAll(&hi2c1, KS0064_ADDR_LEFT,  1U);
                KS0064_SetAll(&hi2c1, KS0064_ADDR_RIGHT, 1U);
                break;

            case INDICATOR_ALERT:
                KS0064_SetAll(&hi2c1, KS0064_ADDR_LEFT,  led_on);
                KS0064_SetAll(&hi2c1, KS0064_ADDR_RIGHT, led_on);
                break;

            default:
                break;
        }

        tx_mutex_put(&ctx->i2c1_mutex);
    }

    tx_thread_sleep(TASK_STEP_MS);
}
