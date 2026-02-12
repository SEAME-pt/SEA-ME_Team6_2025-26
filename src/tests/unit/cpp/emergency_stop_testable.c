/**
  * Versão testável da lógica de Emergency Stop
  * Extraída da ToF Thread
  */

#include "emergency_stop_testable.h"
#include <stdio.h>

// Emergency thresholds (from emergency_stop.h)
#define TOF_EMERGENCY_THRESHOLD_MM    100
#define TOF_WARNING_THRESHOLD_MM      200
#define TOF_HYSTERESIS_MM             20
#define TOF_RECOVERY_THRESHOLD_MM     (TOF_WARNING_THRESHOLD_MM + TOF_HYSTERESIS_MM)

// Global state
volatile uint8_t emergency_stop_active = 0;
static EmergencyStopState_t emergency_state = ESTOP_STATE_NORMAL;

// Callbacks para permitir testes verificarem ações
static emergency_callback_t motor_stop_callback = NULL;
static emergency_callback_t emergency_frame_callback = NULL;
static emergency_callback_t clear_frame_callback = NULL;

/**
  * @brief  Set callback for motor stop action
  */
void Emergency_SetMotorStopCallback(emergency_callback_t callback)
{
    motor_stop_callback = callback;
}

/**
  * @brief  Set callback for emergency frame TX
  */
void Emergency_SetEmergencyFrameCallback(emergency_callback_t callback)
{
    emergency_frame_callback = callback;
}

/**
  * @brief  Set callback for clear frame TX
  */
void Emergency_SetClearFrameCallback(emergency_callback_t callback)
{
    clear_frame_callback = callback;
}

/**
  * @brief  Get current emergency state
  */
EmergencyStopState_t Emergency_GetState(void)
{
    return emergency_state;
}

/**
  * @brief  Get emergency stop active flag
  */
uint8_t Emergency_IsActive(void)
{
    return emergency_stop_active;
}

/**
  * @brief  Reset emergency stop system (for testing)
  */
void Emergency_Reset(void)
{
    emergency_stop_active = 0;
    emergency_state = ESTOP_STATE_NORMAL;
}

/**
  * @brief  Initialize emergency stop system (alias for Emergency_Reset)
  */
void Emergency_Init(void)
{
    Emergency_Reset();
}

/**
  * @brief  Process ToF distance and update emergency state
  * @param  distance_mm: Distance in millimeters from ToF sensor
  * @retval New emergency state
  */
EmergencyStopState_t Emergency_ProcessDistance(uint16_t distance_mm)
{
    EmergencyStopState_t new_state = emergency_state;

    if (distance_mm < TOF_EMERGENCY_THRESHOLD_MM)
    {
        // Entrar em EMERGENCY
        if (emergency_state != ESTOP_STATE_EMERGENCY)
        {
            new_state = ESTOP_STATE_EMERGENCY;
            emergency_stop_active = 1;

            // Call motor stop callback
            if (motor_stop_callback) {
                motor_stop_callback();
            }

            // Call emergency frame callback
            if (emergency_frame_callback) {
                emergency_frame_callback();
            }
        }
    }
    else if (distance_mm < TOF_WARNING_THRESHOLD_MM)
    {
        // Entrar em WARNING (100mm - 200mm)
        if (emergency_state == ESTOP_STATE_EMERGENCY)
        {
            // Transição de EMERGENCY → WARNING
            new_state = ESTOP_STATE_WARNING;
            emergency_stop_active = 0;
        }
        else if (emergency_state == ESTOP_STATE_NORMAL)
        {
            // Transição de NORMAL → WARNING
            new_state = ESTOP_STATE_WARNING;
        }
    }
    else if (distance_mm >= TOF_RECOVERY_THRESHOLD_MM)
    {
        // Recuperar para NORMAL (> 220mm)
        if (emergency_state == ESTOP_STATE_EMERGENCY)
        {
            new_state = ESTOP_STATE_NORMAL;
            emergency_stop_active = 0;

            // Call clear frame callback
            if (clear_frame_callback) {
                clear_frame_callback();
            }
        }
        else if (emergency_state == ESTOP_STATE_WARNING)
        {
            new_state = ESTOP_STATE_NORMAL;
        }
    }
    else
    {
        // Zona de histerese (100-120mm): WARNING
        if (emergency_state == ESTOP_STATE_NORMAL)
        {
            new_state = ESTOP_STATE_WARNING;
        }
    }

    emergency_state = new_state;
    return new_state;
}

/**
  * @brief  Simular recepção de Emergency Stop via CAN
  * @param  active: 1 = emergency, 0 = clear
  * @retval 0 = success, -1 = blocked
  */
int Emergency_ProcessCANCommand(uint8_t active)
{
    if (active)
    {
        // Emergency Stop recebido do AGL
        emergency_stop_active = 1;
        emergency_state = ESTOP_STATE_EMERGENCY;

        // Call motor stop callback
        if (motor_stop_callback) {
            motor_stop_callback();
        }

        return 0;
    }
    else
    {
        // Emergency cleared pelo AGL
        emergency_stop_active = 0;
        emergency_state = ESTOP_STATE_NORMAL;
        return 0;
    }
}

/**
  * @brief  Verificar se um comando de motor deve ser bloqueado
  * @param  has_emergency_flag: Se o comando tem a flag de emergency
  * @retval 1 = bloqueado, 0 = permitido
  */
int Emergency_ShouldBlockMotorCommand(uint8_t has_emergency_flag)
{
    // Bloquear se emergency ativo E comando não tem flag de emergency
    if (emergency_stop_active && !has_emergency_flag)
    {
        return 1;  // Bloqueado
    }
    return 0;  // Permitido
}
