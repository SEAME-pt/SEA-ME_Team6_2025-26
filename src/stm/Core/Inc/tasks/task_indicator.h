/**
 ******************************************************************************
 * @file    task_indicator.h
 * @brief   ThreadX task for KS0064 indicator lights
 *
 * States:
 *   INDICATOR_OFF          – both matrices off
 *   INDICATOR_LEFT_BLINK   – left matrix blinks (pisca esquerdo)
 *   INDICATOR_RIGHT_BLINK  – right matrix blinks (pisca direito)
 *   INDICATOR_HEADLIGHTS   – both matrices fully on (faróis)
 *   INDICATOR_ALERT        – both matrices blink fast (hazard / alerta)
 *
 * Any thread can change the state via task_indicator_set_state().
 * The change is protected by an internal mutex.
 ******************************************************************************
 */

#ifndef INC_TASKS_TASK_INDICATOR_H_
#define INC_TASKS_TASK_INDICATOR_H_

#include "system_ctx.h"
#include "tx_api.h"

/* -------------------------------------------------------------------------
 * States
 * -------------------------------------------------------------------------*/
typedef enum
{
    INDICATOR_OFF           = 0,
    INDICATOR_LEFT_BLINK    = 1,    /* Pisca esquerdo  – slow blink, left only  */
    INDICATOR_RIGHT_BLINK   = 2,    /* Pisca direito   – slow blink, right only */
    INDICATOR_HEADLIGHTS    = 3,    /* Faróis ligados  – both on steady         */
    INDICATOR_ALERT         = 4,    /* Alerta / hazard – both blink fast        */
} IndicatorState_t;

/* -------------------------------------------------------------------------
 * Task lifecycle  (called from app_threadx.c)
 * -------------------------------------------------------------------------*/
void task_indicator_init(SystemCtx *ctx);
void task_indicator_step(SystemCtx *ctx);

/* -------------------------------------------------------------------------
 * State control  (thread-safe, callable from any thread)
 * -------------------------------------------------------------------------*/
void             task_indicator_set_state(IndicatorState_t state);
IndicatorState_t task_indicator_get_state(void);

#endif /* INC_TASKS_TASK_INDICATOR_H_ */
