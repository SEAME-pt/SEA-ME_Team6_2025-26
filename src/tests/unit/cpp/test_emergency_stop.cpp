/**
  * Testes unitários para o Emergency Stop System
  * Usando Google Test Framework
  */

#include <gtest/gtest.h>

extern "C" {
    #include "emergency_stop_testable.h"
}

/**
 * Fixture para os testes do emergency stop
 */
class EmergencyStopTest : public ::testing::Test {
protected:
    // Flags para verificar callbacks
    static bool motor_stop_called;
    static bool emergency_frame_sent;
    static bool clear_frame_sent;

    static void motor_stop_callback() {
        motor_stop_called = true;
    }

    static void emergency_frame_callback() {
        emergency_frame_sent = true;
    }

    static void clear_frame_callback() {
        clear_frame_sent = true;
    }

    void SetUp() override {
        // Reset estado antes de cada teste
        Emergency_Reset();
        motor_stop_called = false;
        emergency_frame_sent = false;
        clear_frame_sent = false;

        // Configurar callbacks
        Emergency_SetMotorStopCallback(motor_stop_callback);
        Emergency_SetEmergencyFrameCallback(emergency_frame_callback);
        Emergency_SetClearFrameCallback(clear_frame_callback);
    }

    void TearDown() override {
        // Cleanup após cada teste
        Emergency_Reset();
    }
};

// Inicialização de variáveis estáticas
bool EmergencyStopTest::motor_stop_called = false;
bool EmergencyStopTest::emergency_frame_sent = false;
bool EmergencyStopTest::clear_frame_sent = false;

/**
 * EMG-01: Deteção de objeto a 80mm
 */
TEST_F(EmergencyStopTest, DetectObjectAt80mm) {
    // Act
    EmergencyStopState_t state = Emergency_ProcessDistance(80);

    // Assert
    EXPECT_EQ(state, ESTOP_STATE_EMERGENCY);
    EXPECT_EQ(Emergency_IsActive(), 1);
    EXPECT_TRUE(motor_stop_called) << "Motor stop deve ser chamado";
}

/**
 * EMG-02: Parar motores imediatamente
 */
TEST_F(EmergencyStopTest, StopMotorsImmediately) {
    // Act
    Emergency_ProcessDistance(50);  // Bem abaixo do threshold

    // Assert
    EXPECT_TRUE(motor_stop_called) << "Motor stop callback deve ser chamado";
}

/**
 * EMG-03: Enviar frame Emergency Stop CAN
 */
TEST_F(EmergencyStopTest, SendEmergencyStopCANFrame) {
    // Act
    Emergency_ProcessDistance(80);

    // Assert
    EXPECT_TRUE(emergency_frame_sent) << "Emergency frame deve ser enviado";
}

/**
 * EMG-06: Estado muda para ESTOP_STATE_EMERGENCY
 */
TEST_F(EmergencyStopTest, StateChangesToEmergency) {
    // Arrange
    EXPECT_EQ(Emergency_GetState(), ESTOP_STATE_NORMAL);

    // Act
    EmergencyStopState_t state = Emergency_ProcessDistance(80);

    // Assert
    EXPECT_EQ(state, ESTOP_STATE_EMERGENCY);
    EXPECT_EQ(Emergency_GetState(), ESTOP_STATE_EMERGENCY);
}

/**
 * EMG-07: Histerese (100-120mm) = WARNING
 */
TEST_F(EmergencyStopTest, HysteresisZoneWarning) {
    // Act
    EmergencyStopState_t state = Emergency_ProcessDistance(110);

    // Assert
    EXPECT_EQ(state, ESTOP_STATE_WARNING);
    EXPECT_EQ(Emergency_IsActive(), 0) << "Emergency não deve estar ativo em WARNING";
}

/**
 * EMG-08: Recovery (ToF >= 220mm)
 */
TEST_F(EmergencyStopTest, RecoveryAbove120mm) {
    // Arrange - Entrar em emergency primeiro
    Emergency_ProcessDistance(80);
    EXPECT_EQ(Emergency_IsActive(), 1);
    clear_frame_sent = false;  // Reset flag

    // Act - Recovery (>= 220mm)
    EmergencyStopState_t state = Emergency_ProcessDistance(250);

    // Assert
    EXPECT_EQ(state, ESTOP_STATE_NORMAL);
    EXPECT_EQ(Emergency_IsActive(), 0);
    EXPECT_TRUE(clear_frame_sent) << "Clear frame deve ser enviado";
}

/**
 * EMG-11: Receber Emergency Stop do AGL
 */
TEST_F(EmergencyStopTest, ReceiveEmergencyFromAGL) {
    // Act
    int result = Emergency_ProcessCANCommand(1);  // active = 1

    // Assert
    EXPECT_EQ(result, 0);  // Success
    EXPECT_EQ(Emergency_IsActive(), 1);
    EXPECT_EQ(Emergency_GetState(), ESTOP_STATE_EMERGENCY);
    EXPECT_TRUE(motor_stop_called);
}

/**
 * EMG-12: Bloquear comandos de motor durante ESTOP
 */
TEST_F(EmergencyStopTest, BlockMotorCommandsDuringEmergency) {
    // Arrange - Ativar emergency
    Emergency_ProcessDistance(80);

    // Act - Tentar comando normal (sem flag de emergency)
    int blocked = Emergency_ShouldBlockMotorCommand(0);

    // Assert
    EXPECT_EQ(blocked, 1) << "Comando deve ser bloqueado";
}

/**
 * EMG-14: Receber Emergency CLEAR do AGL
 */
TEST_F(EmergencyStopTest, ReceiveClearFromAGL) {
    // Arrange - Ativar emergency primeiro
    Emergency_ProcessCANCommand(1);
    EXPECT_EQ(Emergency_IsActive(), 1);

    // Act
    int result = Emergency_ProcessCANCommand(0);  // active = 0 (clear)

    // Assert
    EXPECT_EQ(result, 0);  // Success
    EXPECT_EQ(Emergency_IsActive(), 0);
    EXPECT_EQ(Emergency_GetState(), ESTOP_STATE_NORMAL);
}

/**
 * EMG-18: Flags de emergency no MotorCmd
 */
TEST_F(EmergencyStopTest, EmergencyFlagAllowsCommand) {
    // Arrange - Ativar emergency
    Emergency_ProcessDistance(80);

    // Act - Comando COM flag de emergency
    int blocked = Emergency_ShouldBlockMotorCommand(1);

    // Assert
    EXPECT_EQ(blocked, 0) << "Comando com flag de emergency deve passar";
}

/**
 * EMG-19: Estado inicial = NORMAL
 */
TEST_F(EmergencyStopTest, InitialStateIsNormal) {
    // Assert
    EXPECT_EQ(Emergency_GetState(), ESTOP_STATE_NORMAL);
    EXPECT_EQ(Emergency_IsActive(), 0);
}

/**
 * EMG-20: Transição NORMAL -> WARNING
 */
TEST_F(EmergencyStopTest, TransitionNormalToWarning) {
    // Arrange
    EXPECT_EQ(Emergency_GetState(), ESTOP_STATE_NORMAL);

    // Act
    EmergencyStopState_t state = Emergency_ProcessDistance(110);

    // Assert
    EXPECT_EQ(state, ESTOP_STATE_WARNING);
}

/**
 * EMG-21: Transição WARNING -> EMERGENCY
 */
TEST_F(EmergencyStopTest, TransitionWarningToEmergency) {
    // Arrange - Entrar em WARNING
    Emergency_ProcessDistance(110);
    EXPECT_EQ(Emergency_GetState(), ESTOP_STATE_WARNING);
    motor_stop_called = false;  // Reset flag

    // Act
    EmergencyStopState_t state = Emergency_ProcessDistance(80);

    // Assert
    EXPECT_EQ(state, ESTOP_STATE_EMERGENCY);
    EXPECT_TRUE(motor_stop_called);
}

/**
 * EMG-22: Transição EMERGENCY -> NORMAL
 */
TEST_F(EmergencyStopTest, TransitionEmergencyToNormal) {
    // Arrange - Entrar em EMERGENCY
    Emergency_ProcessDistance(50);
    EXPECT_EQ(Emergency_GetState(), ESTOP_STATE_EMERGENCY);

    // Act - Distância >= 220mm vai para NORMAL
    EmergencyStopState_t state = Emergency_ProcessDistance(250);

    // Assert
    EXPECT_EQ(state, ESTOP_STATE_NORMAL);
    EXPECT_EQ(Emergency_IsActive(), 0);
}

/**
 * EMG-23: Transição WARNING -> NORMAL
 */
TEST_F(EmergencyStopTest, TransitionWarningToNormal) {
    // Arrange - Entrar em WARNING
    Emergency_ProcessDistance(110);
    EXPECT_EQ(Emergency_GetState(), ESTOP_STATE_WARNING);

    // Act - Distância >= 220mm vai para NORMAL
    EmergencyStopState_t state = Emergency_ProcessDistance(250);

    // Assert
    EXPECT_EQ(state, ESTOP_STATE_NORMAL);
}

/**
 * EMG-24: Transição NORMAL -> EMERGENCY (direto)
 */
TEST_F(EmergencyStopTest, TransitionNormalToEmergencyDirect) {
    // Arrange
    EXPECT_EQ(Emergency_GetState(), ESTOP_STATE_NORMAL);

    // Act - Distância muito baixa (pular WARNING)
    EmergencyStopState_t state = Emergency_ProcessDistance(50);

    // Assert
    EXPECT_EQ(state, ESTOP_STATE_EMERGENCY);
    EXPECT_TRUE(motor_stop_called);
}

/**
 * EMG-25: Permanecer em WARNING (histerese)
 */
TEST_F(EmergencyStopTest, StayInWarningDuringHysteresis) {
    // Arrange - Entrar em WARNING
    Emergency_ProcessDistance(110);
    EXPECT_EQ(Emergency_GetState(), ESTOP_STATE_WARNING);

    // Act - Oscilar dentro da zona de histerese
    EmergencyStopState_t state1 = Emergency_ProcessDistance(105);
    EmergencyStopState_t state2 = Emergency_ProcessDistance(115);
    EmergencyStopState_t state3 = Emergency_ProcessDistance(110);

    // Assert - Deve permanecer em WARNING
    EXPECT_EQ(state1, ESTOP_STATE_WARNING);
    EXPECT_EQ(state2, ESTOP_STATE_WARNING);
    EXPECT_EQ(state3, ESTOP_STATE_WARNING);
}

/**
 * TESTE ADICIONAL: Emergency não dispara em distância normal
 */
TEST_F(EmergencyStopTest, NoEmergencyAtNormalDistance) {
    // Act
    EmergencyStopState_t state = Emergency_ProcessDistance(500);

    // Assert
    EXPECT_EQ(state, ESTOP_STATE_NORMAL);
    EXPECT_EQ(Emergency_IsActive(), 0);
    EXPECT_FALSE(motor_stop_called);
}

/**
 * TESTE ADICIONAL: Comando permitido quando emergency não está ativo
 */
TEST_F(EmergencyStopTest, CommandAllowedWhenNotEmergency) {
    // Act
    int blocked = Emergency_ShouldBlockMotorCommand(0);

    // Assert
    EXPECT_EQ(blocked, 0) << "Comando deve ser permitido";
}

/**
 * TESTE ADICIONAL: Emergency frame só enviado uma vez
 */
TEST_F(EmergencyStopTest, EmergencyFrameSentOnce) {
    // Act - Primeira vez
    Emergency_ProcessDistance(80);
    EXPECT_TRUE(emergency_frame_sent);

    // Reset flag e processar novamente
    emergency_frame_sent = false;
    Emergency_ProcessDistance(70);

    // Assert - Não deve enviar novamente
    EXPECT_FALSE(emergency_frame_sent) << "Frame não deve ser enviado novamente";
}

/**
 * TESTE ADICIONAL: Threshold exato (100mm)
 */
TEST_F(EmergencyStopTest, ThresholdExact100mm) {
    // Act
    EmergencyStopState_t state = Emergency_ProcessDistance(100);

    // Assert - 100mm está na zona de WARNING (não < 100)
    EXPECT_EQ(state, ESTOP_STATE_WARNING);
}

/**
 * TESTE ADICIONAL: Recovery threshold exato (220mm)
 */
TEST_F(EmergencyStopTest, RecoveryThresholdExact120mm) {
    // Arrange - Entrar em EMERGENCY
    Emergency_ProcessDistance(80);
    clear_frame_sent = false;

    // Act - Recovery threshold é >= 220mm
    EmergencyStopState_t state = Emergency_ProcessDistance(220);

    // Assert - 220mm deve fazer recovery para NORMAL
    EXPECT_EQ(state, ESTOP_STATE_NORMAL);
    EXPECT_TRUE(clear_frame_sent);
}

/**
 * TESTE ADICIONAL: 99mm dispara emergency
 */
TEST_F(EmergencyStopTest, Distance99mmTriggersEmergency) {
    // Act
    EmergencyStopState_t state = Emergency_ProcessDistance(99);

    // Assert
    EXPECT_EQ(state, ESTOP_STATE_EMERGENCY);
    EXPECT_EQ(Emergency_IsActive(), 1);
}

/**
 * TESTE ADICIONAL: 221mm faz recovery para NORMAL
 */
TEST_F(EmergencyStopTest, Distance121mmTriggersRecovery) {
    // Arrange
    Emergency_ProcessDistance(80);
    clear_frame_sent = false;

    // Act - 221mm > 220mm threshold
    EmergencyStopState_t state = Emergency_ProcessDistance(221);

    // Assert
    EXPECT_EQ(state, ESTOP_STATE_NORMAL);
    EXPECT_TRUE(clear_frame_sent);
}
