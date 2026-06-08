#include "../Inc/tasks/task_aeb.h"

/*
  This file implements the STM32-side AEB (Autonomous Emergency Braking) logic.

  Right now using TTC (Time To Collision) and Stopping distance condition
  It is NOT doing the braking itself.
  Instead, it computes "should we force stop?" and writes flags into ctx->state:
    - aeb_stop_active: 1 => AEB wants the vehicle stopped
    - aeb_warn:        1 => warning level (about to brake / risk rising)
    - aeb_state:       current AEB state machine state
    - aeb_ttc_ms:      debug TTC estimate (ms)
    - aeb_dstop_mm:    debug stopping distance estimate (mm)
    - aeb_ts:          timestamp of last AEB update (ms)

  The thread on task_can_rx.c reads these flags and enforces actuation.
*/

/* -----------------------------
   AEB state machine states
   -----------------------------
   OFF      : AEB not running (not moving / not armed)
   ARMED    : moving, ready to detect risk
   WARN     : risk detected (warning zone)
   BRAKING  : AEB requests stop (throttle forward blocked + Motor_Stop)
   LATCHED  : keep stop request until reset condition
*/
typedef enum {
  AEB_OFF = 0,
  AEB_ARMED,
  AEB_WARN,
  AEB_BRAKING,
  AEB_LATCHED
} AebState;

/* -----------------------------
   Runtime state of the AEB module
   -----------------------------
   st            : current state machine state
   d_f_m         : filtered distance in meters (low-pass)
   below_stop_ms : how long we've been "nearly stopped" while braking
   initialized   : used to initialize distance filter on first valid sample
   safe_clear_ms : how long we've been in "safe" conditions (for unlatching)
*/
typedef struct {
  AebState st;
  float d_f_m;
  uint32_t below_stop_ms;
  bool initialized;
  uint32_t safe_clear_ms;
  uint8_t last_limit;
} AebCtx;

/* -----------------------------
   Tunable parameters for AEB
   -----------------------------
   d_offset_m : subtract bumper/sensor offset from measured distance
   v_min_mps  : minimum speed used in TTC computation (prevents divide-by-zero)

   t_react_s  : reaction/actuation delay (conservative)
   a_brake_mps2 : assumed deceleration capability (conservative)

   ttc_warn_s / ttc_brake_s : TTC thresholds
   margin_warn_m / margin_brake_m : extra margin on stopping distance thresholds

   speed_arm_mps : AEB only arms when speed is above this
   speed_stop_mps: speed below this counts as "stopped"
   stop_hold_ms  : must remain stopped this long to latch

   lp_alpha : low-pass filter coefficient for ultrasonic distance

   max_srf_age_ms / max_speed_age_ms :
     data freshness windows. If SRF08/speed data is too old, AEB treats it invalid.
*/
typedef struct {
  // offsets / guards
  float d_offset_m;
  float v_min_mps;

  // physical model
  float t_react_s;
  float a_brake_mps2;

  // thresholds
  float ttc_warn_s;
  float ttc_brake_s;
  float margin_warn_m;
  float margin_brake_m;

  // gating / latching
  float speed_arm_mps;
  float speed_stop_mps;
  uint32_t stop_hold_ms;

  // filter
  float lp_alpha;

  // freshness (ms)
  uint32_t max_srf_age_ms;
  uint32_t max_speed_age_ms;

  // unlatch policy
  float safe_unlatch_dist_m;    // 1.0m
  uint32_t safe_unlatch_hold_ms; // 1000ms

  float ttc_comfort_s;   // e.g. 1.2
  float v_max_mps;       // measured max speed at throttle=100 (e.g. 3.0)
  uint8_t min_creep_pct; // e.g. 8 (allow slow creep in WARN)
  uint8_t limit_slew_pct_per_step; // e.g. 5 (% per 20ms) to avoid jumps
} AebParams;

/* AEB module global instance (single instance for whole firmware) */
static AebCtx s_aeb;

/* Parameter values (starting conservative defaults) */
static const AebParams P = {
  .d_offset_m = 0.10f, //changed to 0.1 because the srf08 is really in the nose - David
  .v_min_mps  = 0.10f,

  .t_react_s    = 0.25f,
  .a_brake_mps2 = 3.0f, //model of how quickly we can decelerate - change this in tests

  .ttc_warn_s   = 1.5f,
  .ttc_brake_s  = 0.7f, //the real time to break - chang this in tests it was 0.9
  .margin_warn_m  = 0.30f,
  .margin_brake_m = 0.10f,

  .speed_arm_mps  = 0.30f,
  .speed_stop_mps = 0.10f,
  .stop_hold_ms   = 300,

  .lp_alpha = 0.30f,

  .max_srf_age_ms   = 200,
  .max_speed_age_ms = 200,

  .safe_unlatch_dist_m = 0.2f, // it was 1.0
  .safe_unlatch_hold_ms = 1000u,

  .ttc_comfort_s = 1.2f,   // TTC threshold for "comfort braking" (inside WARN state)
  .v_max_mps = 2.35f,       // change this after measure max speed at full throttle
  .min_creep_pct = 10u,
  .limit_slew_pct_per_step = 5u
};

/* Simple float clamp helper */
static float clampf(float x, float lo, float hi) {
  if (x < lo) return lo;
  if (x > hi) return hi;
  return x;
}

static void aeb_reset(AebCtx* a) {
  a->st = AEB_OFF;
  a->d_f_m = 0.0f;
  a->below_stop_ms = 0;
  a->initialized = false;
  a->safe_clear_ms = 0;
  a->last_limit = 0;
}

void task_aeb_init(SystemCtx* ctx) {
  (void)ctx;
  aeb_reset(&s_aeb);
}

/*
  Internal step: executes one AEB evaluation cycle.

  Inputs are taken from ctx->state (snapshot under mutex).
  Outputs are written back into ctx->state (under mutex).
*/
static void aeb_step_internal(SystemCtx* ctx, uint32_t dt_ms)
{
  /* -------- 1) Snapshot inputs (thread-safe) -------- */
  VehicleState snap;
  tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
  snap = ctx->state;
  tx_mutex_put(&ctx->state_mutex);

  const uint32_t now_ms = (uint32_t)tx_time_get();

  /* -------- 2) Validate "freshness" + sensor flags --------
     We treat sensor values as valid only if:
       - SRF08 valid flag is set
       - SRF08 timestamp is recent enough (<= max_srf_age_ms)
       - Speed timestamp is recent enough (<= max_speed_age_ms)
  */
  bool srf_ok = (snap.srf08_valid != 0) && ((now_ms - snap.srf08_ts) <= P.max_srf_age_ms);
  bool spd_ok = ((now_ms - snap.speed_ts) <= P.max_speed_age_ms);

  /* -------- 3) Convert units to SI --------
     speed_mh = meters/hour -> m/s by /3600
     srf08_distance_mm -> m by /1000
  */
  float v_mps = (float)snap.speed_mh / 3600.0f;
  float d_m   = (float)snap.srf08_distance_mm / 1000.0f;

  /* -------- 4) Fault policy (important!) --------
     If sensors are invalid:

       - If we were already BRAKING/LATCHED:
           stay LATCHED and keep stop request (fail-safe bias).
       - Else:
           go OFF and clear stop request.
  */
  if (!srf_ok || !spd_ok) {
    if (s_aeb.st == AEB_BRAKING || s_aeb.st == AEB_LATCHED) {
      s_aeb.st = AEB_LATCHED;

      tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
      ctx->state.aeb_stop_active = 1;        // keep stopping
      ctx->state.aeb_warn = 1;
      ctx->state.aeb_state = (uint8_t)s_aeb.st;
      ctx->state.aeb_ts = now_ms;
      tx_mutex_put(&ctx->state_mutex);
      return;
    }

    s_aeb.st = AEB_OFF;

    tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
    ctx->state.aeb_stop_active = 0;          // no AEB stop
    ctx->state.aeb_warn = 0;
    ctx->state.aeb_state = (uint8_t)s_aeb.st;
    ctx->state.aeb_ts = now_ms;
    tx_mutex_put(&ctx->state_mutex);
    return;
  }

  /* -------- 5) Filter distance (low-pass) --------
     Ultrasonic data can spike. We smooth it:

       d_f = d_f + alpha * (d_raw - d_f)

     initialized==false means first sample sets the filter baseline.
  */
  if (!s_aeb.initialized) {
    s_aeb.d_f_m = d_m;
    s_aeb.initialized = true;
  } else {
    float a = clampf(P.lp_alpha, 0.0f, 1.0f);
    s_aeb.d_f_m = s_aeb.d_f_m + a * (d_m - s_aeb.d_f_m);
  }

  float d = s_aeb.d_f_m;

  /* Clamp to avoid absurd/zero distances */
  if (d < 0.02f) d = 0.02f;

  /* -------- 6) Compute TTC + stopping distance --------

     TTC = (d - offset) / max(v, v_min)

     stopping distance model:
       d_stop = v*t_react + v^2/(2*a_brake)

     Ccompute warn/brake thresholds based on d_stop + margins.
  */
  float v_eff = (v_mps > P.v_min_mps) ? v_mps : P.v_min_mps;

  float d_eff = d - P.d_offset_m;
  if (d_eff < 0.05f) d_eff = 0.05f;   // clamp effective distance

  float ttc = d_eff / v_eff;

  float a_brake = (P.a_brake_mps2 > 0.1f) ? P.a_brake_mps2 : 0.1f;
  float d_stop = v_mps * P.t_react_s + (v_mps * v_mps) / (2.0f * a_brake);

  float d_warn  = d_stop + P.margin_warn_m;
  float d_brake = d_stop + P.margin_brake_m;

  bool warn = false;
  bool brake = false;

  /* -------- 6.1) Compute speed limit  -------- */
  uint8_t desired_limit = 100;

  if (s_aeb.st == AEB_BRAKING || s_aeb.st == AEB_LATCHED) {
    desired_limit = 0;
  } else {
  // Soft layer active only when moving forward-ish
  if (v_mps > 0.1f) { //changed 0.2 to 0.1
    float v_target = d_eff / P.ttc_comfort_s;   // TTC-based target speed
    if (v_target < 0.0f) v_target = 0.0f;
    if (v_target > P.v_max_mps) v_target = P.v_max_mps;

    float pct_f = (P.v_max_mps > 0.1f) ? (100.0f * (v_target / P.v_max_mps)) : 100.0f;
    if (pct_f < (float)P.min_creep_pct) pct_f = (float)P.min_creep_pct;
    if (pct_f > 100.0f) pct_f = 100.0f;

    desired_limit = (uint8_t)pct_f;
    }
  }

  // Slew-rate limit so throttle cap doesn’t jump around
  int diff = (int)desired_limit - (int)s_aeb.last_limit;
  int max_step = (int)P.limit_slew_pct_per_step;
  if (diff >  max_step) desired_limit = (uint8_t)(s_aeb.last_limit + max_step);
  if (diff < -max_step) desired_limit = (uint8_t)(s_aeb.last_limit - max_step);

  s_aeb.last_limit = desired_limit;

  //added log to see if soft braking is active
  #define COLOR_GREEN   "\x1b[32m"
  #define COLOR_RESET   "\x1b[0m"
  sys_log(ctx, COLOR_GREEN "[AEB] current_speed=%.2f desired_limit=%u%%" COLOR_RESET,
          v_mps, desired_limit);

  // Publish
  tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
  ctx->state.aeb_speed_limit = desired_limit;
  tx_mutex_put(&ctx->state_mutex);

  /* -------- 7) State machine --------
     OFF   -> ARMED when speed is enough to care
     ARMED -> WARN  when TTC small or distance below warn threshold
     WARN  -> BRAKING when TTC smaller or distance below brake threshold
     BRAKING -> LATCHED once "stopped" long enough
     LATCHED stays until an external reset policy exists
  */
  switch (s_aeb.st) {
    case AEB_OFF:
      if (v_mps > P.speed_arm_mps) s_aeb.st = AEB_ARMED;
      break;

    case AEB_ARMED:
      if (ttc < P.ttc_warn_s || d_eff < d_warn) s_aeb.st = AEB_WARN;
      break;

    case AEB_WARN:
      warn = true;

      if (ttc < P.ttc_brake_s || d_eff < d_brake) {
        s_aeb.st = AEB_BRAKING;
        s_aeb.below_stop_ms = 0;
      } else {
        /* only return to ARMED if we are clearly safe again */
        if (ttc > (P.ttc_warn_s + 0.5f) && d_eff > (d_warn + 0.3f))
          s_aeb.st = AEB_ARMED;
      }
      break;

    case AEB_BRAKING:
      warn = true;
      brake = true;

      /* Latch when vehicle stays below "stop speed" long enough */
      if (v_mps < P.speed_stop_mps) {
        s_aeb.below_stop_ms += dt_ms;
        if (s_aeb.below_stop_ms >= P.stop_hold_ms) s_aeb.st = AEB_LATCHED;
      } else {
        s_aeb.below_stop_ms = 0;
      }
      break;

    case AEB_LATCHED:
      warn = true;
      brake = true;
      /*
        UNLATCH policy:
        If we are basically stopped AND the obstacle is far away for long enough,
        we clear the latch and return to OFF.

        Condition:
          speed < speed_stop_mps
          AND filtered distance > safe_unlatch_dist_m
          for safe_unlatch_hold_ms
      */
      if ((v_mps < P.speed_stop_mps) && (d > P.safe_unlatch_dist_m)) {
        s_aeb.safe_clear_ms += dt_ms;

        if (s_aeb.safe_clear_ms >= P.safe_unlatch_hold_ms) {
          // Unlatch: go OFF and release brake request
          s_aeb.st = AEB_OFF;
          brake = false;
          warn = false;
          s_aeb.below_stop_ms = 0;
          s_aeb.safe_clear_ms = 0;
        }
      } else {
        s_aeb.safe_clear_ms = 0;
      }
      break;
  }

  /* -------- 8) Debug metrics --------
     Store TTC and stopping distance in integer fields for logging/telemetry.
     Clamp so they fit into uint16_t later.
  */
  uint32_t ttc_ms   = (ttc > 65.0f)   ? 65000u : (uint32_t)(ttc * 1000.0f);
  uint32_t dstop_mm = (d_stop > 65.0f)? 65000u : (uint32_t)(d_stop * 1000.0f);

  /* -------- 9) Publish AEB outputs into shared state --------
     The actuation thread reads these flags.
     aeb_stop_active = 1 means "block forward + stop motor"
  */
  tx_mutex_get(&ctx->state_mutex, TX_WAIT_FOREVER);
  ctx->state.aeb_stop_active = brake ? 1 : 0;
  ctx->state.aeb_warn = warn ? 1 : 0;
  ctx->state.aeb_state = (uint8_t)s_aeb.st;
  ctx->state.aeb_ttc_ms = (uint16_t)((ttc_ms > 65535u) ? 65535u : ttc_ms);
  ctx->state.aeb_dstop_mm = (uint16_t)((dstop_mm > 65535u) ? 65535u : dstop_mm);
  ctx->state.aeb_ts = now_ms;
  tx_mutex_put(&ctx->state_mutex);
}

/* Public API called by the AEB thread loop */
void task_aeb_step(SystemCtx* ctx, uint32_t dt_ms) {
  if (!ctx) return;
  if (dt_ms == 0) dt_ms = 1; // guard against zero dt
  aeb_step_internal(ctx, dt_ms);
}
