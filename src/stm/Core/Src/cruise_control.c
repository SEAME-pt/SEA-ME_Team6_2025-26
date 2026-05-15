/*
 * cruise_control.c
 *
 * Adaptive Cruise Control (V2) - PI speed controller with target rate limiting
 *
 * Created on: Mar 2026
 *     Author: rcosta-c
 */

#include "cruise_control.h"
#include <math.h>
#include <string.h>


/*============================================================================
 * Internal helpers
 *============================================================================*/

static float clampf(float val, float min, float max)
{
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

/**
 * Convert wheel RPM to km/h using wheel geometry defines.
 * circumference = pi * diameter
 * speed_m_s = (rpm / 60) * circumference
 * speed_kmh = speed_m_s * 3.6
 *
 * NOTE: In this project, ctx->state.speed_mh (already filtered) is preferred
 * over this function. Use: current_kmh = (float)snap.speed_mh / 1000.0f
 */
float CruiseControl_RpmToKmh(int16_t rpm)
{
    float circumference_m = (float)CC_WHEEL_DIAMETER_MM / 1000.0f * 3.14159f;
    float speed_m_s = (fabsf((float)rpm) / 60.0f) * circumference_m;
    return speed_m_s * 3.6f;
}


/*============================================================================
 * Init
 *============================================================================*/

void CruiseControl_Init(CruiseControl_t *cc)
{
    memset(cc, 0, sizeof(CruiseControl_t));
    cc->state = CC_STATE_OFF;
}


/*============================================================================
 * Command processing (called from CAN RX handler)
 *============================================================================*/

void CruiseControl_ProcessCommand(CruiseControl_t *cc, const CruiseControlCmd_t *cmd)
{
    float requested_speed = (float)cmd->target_speed / 100.0f;  /* 0.01 km/h units -> km/h */

    switch ((CruiseControlCommand_t)cmd->command)
    {
    case CC_CMD_SET_SPEED:
        /* Setpoint update — accepted from HMI or ADAS Manager.
         * active_target_kmh is NOT snapped here; the rate limiter in
         * CruiseControl_Update will track this new value smoothly. */
        if (requested_speed >= CC_SPEED_MIN && requested_speed <= CC_SPEED_MAX)
        {
            cc->target_speed_kmh = requested_speed;
        }
        break;

    case CC_CMD_ACTIVATE:
        /* Only activate if we have a valid target and car is moving */
        if (cc->target_speed_kmh >= CC_SPEED_MIN &&
            cc->current_speed_kmh >= CC_SPEED_MIN)
        {
            cc->state = CC_STATE_ACTIVE;
            cc->integral = 0.0f;            /* Reset integrator on fresh activation */
            cc->last_error = 0.0f;
            cc->stall_timer_ms = 0;
            cc->activation_grace_ms = 0;    /* Stall detection suspended for CC_STALL_GRACE_MS */

            /* Seed active_target at current speed so the rate limiter ramps up
             * from where we are, avoiding an instant step into the PI on activation. */
            cc->active_target_kmh = cc->current_speed_kmh;
        }
        break;

    case CC_CMD_DEACTIVATE:
        cc->state = CC_STATE_OFF;
        cc->integral = 0.0f;
        cc->applied_throttle = 0.0f;
        cc->active_target_kmh = 0.0f;
        /* target_speed_kmh is NOT cleared — available for resume */
        break;

    case CC_CMD_OFF:
        cc->state = CC_STATE_OFF;
        cc->integral = 0.0f;
        cc->applied_throttle = 0.0f;
        cc->target_speed_kmh = 0.0f;       /* Clear target completely */
        cc->active_target_kmh = 0.0f;
        break;

    case CC_CMD_RESUME:
        if (cc->state == CC_STATE_OVERRIDE &&
            cc->target_speed_kmh >= CC_SPEED_MIN &&
            cc->current_speed_kmh >= CC_SPEED_MIN)
        {
            cc->state = CC_STATE_ACTIVE;
            /* Don't reset integral — let it resume smoothly */
            cc->stall_timer_ms = 0;
            cc->activation_grace_ms = 0;    /* Fresh grace period on resume too */

            /* Seed active_target at current speed for smooth resume */
            cc->active_target_kmh = cc->current_speed_kmh;
        }
        break;

    case CC_CMD_INCREMENT:
    case CC_CMD_DECREMENT:
        /* V2: speed nudge ±0.5 km/h */
        break;
    }
}


/*============================================================================
 * Target rate limiter — smooths ADAS-driven setpoint changes
 *============================================================================*/

/**
 * Track target_speed_kmh into active_target_kmh at a bounded rate.
 *
 * Without this, a large step from the ADAS Manager (e.g. user 10 km/h →
 * TSR-50 mapped to 4 km/h) would feed a -6 km/h error step into the PI,
 * saturating throttle to 0 and producing a jerky decel. Rate-limiting the
 * setpoint converts the step into a ramp the PI can track linearly.
 *
 * Asymmetric: deceleration is permitted faster than acceleration (matches
 * real car ACC behavior — comfort on speed-up, responsiveness on slow-down).
 */
static void update_active_target(CruiseControl_t *cc, float dt_s)
{
    float delta = cc->target_speed_kmh - cc->active_target_kmh;
    float max_up = CC_TARGET_RAMP_UP_KMH_S * dt_s;
    float max_down = CC_TARGET_RAMP_DOWN_KMH_S * dt_s;

    if (delta > max_up)
        cc->active_target_kmh += max_up;
    else if (delta < -max_down)
        cc->active_target_kmh -= max_down;
    else
        cc->active_target_kmh = cc->target_speed_kmh;
}


/*============================================================================
 * PI Controller — the core
 *============================================================================*/

static float pi_compute(CruiseControl_t *cc, float dt_s)
{
    /* PI tracks active_target (rate-limited), not the raw setpoint. */
    float error = cc->active_target_kmh - cc->current_speed_kmh;

    /*
     * Dead-band: if within tolerance, don't accumulate integral error.
     * This prevents micro-oscillations around the target.
     */
    if (fabsf(error) < CC_SPEED_TOLERANCE)
    {
        error = 0.0f;
    }

    /* Proportional */
    float p_term = CC_KP * error;

    /* Integral with anti-windup clamping */
    cc->integral += error * dt_s;
    cc->integral = clampf(cc->integral, CC_INTEGRAL_MIN, CC_INTEGRAL_MAX);
    float i_term = CC_KI * cc->integral;

    /* Total output (throttle %) — forward only, never reverse */
    float output = p_term + i_term;
    output = clampf(output, CC_THROTTLE_MIN, CC_THROTTLE_MAX);

    /*
     * Deadzone compensation: the motor doesn't spin below a certain PWM.
     * If PI wants any positive throttle but below the deadzone, snap up to
     * the deadzone threshold. This prevents the integral from winding up
     * while the motor sits still, which causes the kick-stop-reverse cycle.
     *
     * When we boost past the deadzone, freeze the integral for this tick
     * so it doesn't accumulate "free" error that will discharge later.
     */
    if (output > 0.0f && output < CC_MOTOR_DEADZONE_PCT)
    {
        output = CC_MOTOR_DEADZONE_PCT;
        /* Undo this tick's integral accumulation — motor is being boosted,
         * not responding to the real PI output */
        cc->integral -= error * dt_s;
        cc->integral = clampf(cc->integral, CC_INTEGRAL_MIN, CC_INTEGRAL_MAX);
    }

    cc->last_error = error;
    return output;
}


/*============================================================================
 * Rate limiter — controls how fast throttle can change
 *============================================================================*/

static float apply_ramp(float current, float target)
{
    float delta = target - current;

    if (delta > CC_THROTTLE_RAMP_UP)
        return current + CC_THROTTLE_RAMP_UP;

    if (delta < -CC_THROTTLE_RAMP_DOWN)
        return current - CC_THROTTLE_RAMP_DOWN;

    return target;
}


/*============================================================================
 * Main update — call every CC_TASK_PERIOD_MS
 *============================================================================*/

void CruiseControl_Update(CruiseControl_t *cc, float current_speed_kmh)
{
    cc->current_speed_kmh = current_speed_kmh;

    if (cc->state != CC_STATE_ACTIVE)
    {
        /* Not active — nothing to compute */
        return;
    }

    float dt_s = (float)CC_TASK_PERIOD_MS / 1000.0f;

    /* --- Setpoint rate limiting (ACC adaptation smoothing) --- */
    update_active_target(cc, dt_s);

    /* --- Safety checks ---
     *
     * NOTE: stall detection compares against active_target_kmh, not the raw
     * setpoint. During a downward ramp (user 10 → TSR 4), this avoids spurious
     * stall triggers while the rate limiter is still tracking down. */

    /* Count up grace period: stall detection only starts after CC_STALL_GRACE_MS.
     * This prevents false stall triggers during initial ramp-up from low speed. */
    if (cc->activation_grace_ms < CC_STALL_GRACE_MS)
    {
        cc->activation_grace_ms += CC_TASK_PERIOD_MS;
    }

    /* Speed dropped too far below the active target (obstacle, stuck, wheel off ground).
     * Only evaluated after grace period expires. */
    if (cc->activation_grace_ms >= CC_STALL_GRACE_MS &&
        cc->current_speed_kmh < (cc->active_target_kmh - CC_SPEED_DROP_THRESHOLD) &&
        cc->active_target_kmh > CC_SPEED_MIN)
    {
        cc->stall_timer_ms += CC_TASK_PERIOD_MS;
        if (cc->stall_timer_ms >= CC_STALL_TIMEOUT_MS)
        {
            CruiseControl_ForceOverride(cc);
            return;
        }
    }
    else
    {
        cc->stall_timer_ms = 0;
    }

    /* Speed below minimum (car stopped externally) */
    if (cc->current_speed_kmh < CC_SPEED_MIN * 0.5f)
    {
        CruiseControl_ForceOverride(cc);
        return;
    }

    /* --- PI computation --- */
    cc->output_throttle = pi_compute(cc, dt_s);

    /* --- Rate limiting --- */
    cc->applied_throttle = apply_ramp(cc->applied_throttle, cc->output_throttle);
}


/*============================================================================
 * Public getters
 *============================================================================*/

float CruiseControl_GetThrottle(const CruiseControl_t *cc)
{
    if (cc->state == CC_STATE_ACTIVE)
        return cc->applied_throttle;

    return NAN;  /* Caller should use MotorCmd throttle instead */
}

bool CruiseControl_IsActive(const CruiseControl_t *cc)
{
    return (cc->state == CC_STATE_ACTIVE);
}


/*============================================================================
 * Force override (from AEB, emergency, brake)
 *============================================================================*/

void CruiseControl_ForceOverride(CruiseControl_t *cc)
{
    if (cc->state == CC_STATE_ACTIVE)
    {
        cc->state = CC_STATE_OVERRIDE;
        cc->applied_throttle = 0.0f;
        cc->integral = 0.0f;
        cc->stall_timer_ms = 0;
        cc->active_target_kmh = 0.0f;
        /* target_speed_kmh preserved for resume */
    }
}
