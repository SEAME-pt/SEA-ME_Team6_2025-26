/**
 ******************************************************************************
 * @file    task_indicator.c
 * @brief   KS0064 LED matrix indicator task – state derived from VehicleState
 *
 * Priority (highest first):
 *   1. SOS  – AEB active OR srf08 < SRF08_DIST_ALARM_MM  → fast blink, 5 s
 *   2. Turn – servo_angle outside ±STEER_THRESHOLD        → slow blink, one side
 *   3. On   – srf08_light < LIGHT_THRESHOLD (headlights) OR braking → steady
 *   4. Off
 *
 * KS0064 is driven ONLY from this file.
 ******************************************************************************
 */

#include "tasks/task_indicator.h"
#include "ks0064.h"
#include "i2c.h"
#include "sys_helpers.h"
#include <stdbool.h>

/* -------------------------------------------------------------------------
 * Timing & threshold configuration
 * -------------------------------------------------------------------------*/
#define TASK_STEP_MS          50U   /* task period (ms)                       */
#define BLINK_SLOW_HALF_MS   500U   /* turn signal: 500 ms on / 500 ms off    */
#define BLINK_FAST_HALF_MS   150U   /* SOS alert:   150 ms on / 150 ms off    */
#define SOS_DURATION_STEPS  (5000U / TASK_STEP_MS)  /* 100 steps = 5 s       */

#define STEER_THRESHOLD      15     /* |servo_angle| % to trigger turn signal */
#define SRF08_DIST_ALARM_MM  50     /* mm: SOS if srf08 distance below this   */
#define LIGHT_ON_THRESHOLD   80     /* turn headlights ON  when light < this  */
#define LIGHT_OFF_THRESHOLD  150    /* turn headlights OFF when light > this  */
#define LIGHT_DEBOUNCE_STEPS 60U    /* steps of stable reading before change  (60×50ms = 3s) */
#define BRAKE_SPEED_MIN_MH   500U   /* m/h: min speed to detect braking       */
#define BRAKE_SPEED_DROP_MH  200U   /* m/h: drop per step to flag braking     */

/* -------------------------------------------------------------------------
 * External API stubs
 * Kept for link compatibility – the step function reads VehicleState directly.
 * -------------------------------------------------------------------------*/
static TX_MUTEX               s_mutex;
static volatile IndicatorState_t s_state = INDICATOR_OFF;

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
 * Lifecycle
 * -------------------------------------------------------------------------*/
void task_indicator_init(SystemCtx *ctx)
{
    tx_mutex_create(&s_mutex, "Indicator Mutex", TX_NO_INHERIT);

    tx_mutex_get(&ctx->i2c2_mutex, TX_WAIT_FOREVER);
    MX_I2C2_Init();

    /* Scan I2C2 to confirm which matrices are present before init */
    HAL_StatusTypeDef l_rdy = HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t)(KS0064_ADDR_LEFT  << 1), 3, 10);
    HAL_StatusTypeDef r_rdy = HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t)(KS0064_ADDR_RIGHT << 1), 3, 10);

    if (l_rdy == HAL_OK) KS0064_Init(&hi2c2, KS0064_ADDR_LEFT);
    if (r_rdy == HAL_OK) KS0064_Init(&hi2c2, KS0064_ADDR_RIGHT);

    tx_mutex_put(&ctx->i2c2_mutex);

    sys_log(ctx, "[Indicator] I2C2 scan: LEFT(0x%02X)=%s RIGHT(0x%02X)=%s",
            KS0064_ADDR_LEFT,  l_rdy == HAL_OK ? "OK" : "ABSENT",
            KS0064_ADDR_RIGHT, r_rdy == HAL_OK ? "OK" : "ABSENT");
}

void task_indicator_step(SystemCtx *ctx)
{
    static uint32_t blink_tick        = 0U;
    static uint32_t sos_steps         = 0U;   /* countdown in task steps        */
    static uint32_t prev_speed        = 0U;
    static uint8_t  led_on            = 0U;
    static uint8_t  prev_left         = 0xFFU; /* force hardware update on first run */
    static uint8_t  prev_right        = 0xFFU;
    static uint8_t  headlights_on     = 0U;   /* debounced headlights state     */
    static uint32_t light_stable_steps = 0U;  /* steps stable in current dir    */

    /* ---- Snapshot VehicleState ---- */
    VehicleState s;
    tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
    s = ctx->state;
    tx_mutex_put(&ctx->state_mutex);

    /* ---- Derive conditions ---- */
    bool trig_sos   = s.aeb_stop_active ||
                      (s.srf08_valid && s.srf08_distance_mm < SRF08_DIST_ALARM_MM);
    bool turn_left  = s.servo_angle < -STEER_THRESHOLD;
    bool turn_right = s.servo_angle >  STEER_THRESHOLD;
    bool braking    = (prev_speed > BRAKE_SPEED_MIN_MH) &&
                      (s.speed_mh + BRAKE_SPEED_DROP_MH < prev_speed);

    /* Headlights: debounced to avoid feedback loop (matrices emit light the
     * SRF08 sensor reads, which would cause rapid oscillation without debounce).
     * State only flips after LIGHT_DEBOUNCE_STEPS steps of stable condition. */
    if (!headlights_on) {
        if (s.srf08_light < LIGHT_ON_THRESHOLD) {
            if (++light_stable_steps >= LIGHT_DEBOUNCE_STEPS) {
                headlights_on = 1U;
                light_stable_steps = 0U;
            }
        } else {
            light_stable_steps = 0U;
        }
    } else {
        if (s.srf08_light > LIGHT_OFF_THRESHOLD) {
            if (++light_stable_steps >= LIGHT_DEBOUNCE_STEPS) {
                headlights_on = 0U;
                light_stable_steps = 0U;
            }
        } else {
            light_stable_steps = 0U;
        }
    }
    bool headlights = headlights_on;

    prev_speed = s.speed_mh;

    /* ---- SOS timer (re-armed each step while condition is active) ---- */
    if (trig_sos)
        sos_steps = SOS_DURATION_STEPS;
    else if (sos_steps > 0U)
        sos_steps--;

    /* ---- Blink phase ---- */
    uint32_t half_period = (sos_steps > 0U)          ? BLINK_FAST_HALF_MS :
                           (turn_left || turn_right)  ? BLINK_SLOW_HALF_MS : 0U;

    blink_tick += TASK_STEP_MS;
    if (half_period > 0U) {
        if (blink_tick >= half_period) { led_on ^= 1U; blink_tick = 0U; }
    } else {
        blink_tick = 0U;
        led_on = 1U; /* reset phase so first blink entry starts ON */
    }

    /* ---- Compute per-matrix output ---- */
    uint8_t steady  = (headlights || braking) ? 1U : 0U;
    uint8_t left_on, right_on;

    if (sos_steps > 0U) {
        left_on = right_on = led_on;
    } else if (turn_left || turn_right) {
        left_on  = turn_left  ? led_on : steady;
        right_on = turn_right ? led_on : steady;
    } else {
        left_on = right_on = steady;
    }

    /* ---- Drive hardware only when output changes ---- */
    if (left_on != prev_left || right_on != prev_right) {
        static uint8_t err_log_count = 0U;

        tx_mutex_get(&ctx->i2c2_mutex, TX_WAIT_FOREVER);

        HAL_StatusTypeDef r1 = KS0064_SetDisplay(&hi2c2, KS0064_ADDR_LEFT,  left_on);
        HAL_StatusTypeDef r2 = KS0064_SetDisplay(&hi2c2, KS0064_ADDR_RIGHT, right_on);

        /* Handle each matrix independently so a missing/faulty RIGHT matrix
         * does not prevent LEFT from updating (and vice versa). */
        if (r1 == HAL_OK)
        {
            prev_left = left_on;
        }
        else
        {
            /* LEFT failed — reinit only LEFT */
            MX_I2C2_Init();
            KS0064_Init(&hi2c2, KS0064_ADDR_LEFT);
        }

        if (r2 == HAL_OK)
        {
            prev_right = right_on;
        }
        else
        {
            /* RIGHT failed — reinit only RIGHT (bus was already reset above if
             * LEFT also failed, so just re-init the chip registers) */
            if (r1 == HAL_OK) MX_I2C2_Init();  /* reset bus only if not already done */
            KS0064_Init(&hi2c2, KS0064_ADDR_RIGHT);
        }

        if ((r1 != HAL_OK || r2 != HAL_OK) && (++err_log_count >= 20U))
        {
            err_log_count = 0U;
            sys_log(ctx, "[IND] I2C2 err r1=%d r2=%d (L=%u R=%u)",
                    r1, r2, left_on, right_on);
        }

        tx_mutex_put(&ctx->i2c2_mutex);
    }

    tx_thread_sleep(TASK_STEP_MS);
}
