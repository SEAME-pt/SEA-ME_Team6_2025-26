/**
 * Testes de Integração
 * Combinam múltiplos módulos para testar interações
 */

#include <gtest/gtest.h>

extern "C" {
    #include "emergency_stop_testable.h"
    #include "motor_control_testable.h"
    #include "tof_testable.h"
    #include "servo_testable.h"
    #include "speedometer_testable.h"
    #include "stm32u5xx_hal.h"
}

/**
 * Fixture para os testes de integração
 */
class IntegrationTest : public ::testing::Test {
protected:
    I2C_HandleTypeDef mock_i2c;
    TIM_HandleTypeDef mock_tim;
    ToF_ResultsData mock_tof_data;

    void SetUp() override {
        // Reset all modules
        Emergency_Reset();
        Motor_Reset();
        ToF_Reset();
        Servo_Reset();

        // Setup mocks
        mock_set_i2c_status(HAL_OK);
        mock_set_tim_pwm_status(HAL_OK);
        mock_tim_reset();
        mock_tim_set_autoreload(20000);
        mock_clear_i2c_history();

        // Initialize mock structures
        mock_i2c.Instance = nullptr;
        mock_i2c.Init = 0;

        mock_tim.Instance = nullptr;
        mock_tim.Init = 0;
        mock_tim.Channel = TIM_CHANNEL_1;

        // Initialize mock ToF data
        mock_tof_data.nb_zones = TOF_RESOLUTION_8X8;
        for (int i = 0; i < TOF_MAX_ZONES; i++) {
            mock_tof_data.distance_mm[i] = 0;
            mock_tof_data.target_status[i] = TOF_STATUS_INVALID;
        }

        // Setup Emergency callback to stop motor
        Emergency_SetMotorStopCallback([]() {
            Motor_Stop();
        });
    }

    void TearDown() override {
        Emergency_Reset();
        Motor_Reset();
        ToF_Reset();
        Servo_Reset();
    }
};

/**
 * INT-01: Emergency Stop + Motor Control
 * ToF detecta objeto próximo → Emergency Stop ativa → Motor para
 */
TEST_F(IntegrationTest, EmergencyStop_StopsMotor) {
    // Arrange - Inicializar módulos
    Motor_Init(&mock_i2c);
    Emergency_Init();

    // Configurar callback (após Emergency_Init que pode ter resetado)
    Emergency_SetMotorStopCallback([]() {
        Motor_Stop();
    });

    // Motor em movimento
    Motor_Forward(75);
    EXPECT_TRUE(motor_is_running());

    // Act - ToF detecta objeto a 80mm (< 100mm = EMERGENCY)
    EmergencyStopState_t state = Emergency_ProcessDistance(80);

    // Assert
    EXPECT_EQ(state, ESTOP_STATE_EMERGENCY);
    EXPECT_TRUE(Emergency_IsActive());
    EXPECT_FALSE(motor_is_running()) << "Motor deve ser parado pelo emergency stop";

    // Verify I2C command was sent to stop motor
    uint8_t i2c_data[3];
    uint16_t i2c_size;
    uint16_t i2c_addr;
    mock_get_i2c_last_tx(i2c_data, &i2c_size, &i2c_addr);
    EXPECT_EQ(i2c_data[0], MOTOR_CMD_BRAKE);  // Motor_Stop sends BRAKE command
}

/**
 * INT-02: Emergency Stop Release + Motor Resume
 */
TEST_F(IntegrationTest, EmergencyRelease_AllowsMotorResume) {
    // Arrange - Estado de emergência ativo
    Motor_Init(&mock_i2c);
    Emergency_Init();
    Emergency_ProcessDistance(50);  // Ativar emergência
    EXPECT_TRUE(Emergency_IsActive());

    // Act - Objeto se afasta (> 200mm = NORMAL)
    EmergencyStopState_t state = Emergency_ProcessDistance(500);

    // Assert - Emergência desativada
    EXPECT_EQ(state, ESTOP_STATE_NORMAL);
    EXPECT_FALSE(Emergency_IsActive());

    // Motor pode ser acionado novamente
    HAL_StatusTypeDef status = Motor_Forward(50);
    EXPECT_EQ(status, HAL_OK);
    EXPECT_TRUE(motor_is_running());
}

/**
 * INT-03: ToF + Emergency Stop + Motor
 * Cenário completo: ToF lê distância → processa emergency → controla motor
 */
TEST_F(IntegrationTest, ToF_Emergency_Motor_CompleteScenario) {
    // Arrange - Inicializar todos os módulos
    ToF_Init();
    ToF_StartRanging();
    Motor_Init(&mock_i2c);
    Emergency_Init();

    // Motor em movimento
    Motor_Forward(80);
    EXPECT_TRUE(motor_is_running());

    // Simular ToF: objeto a 90mm na zona 20
    mock_tof_data.distance_mm[20] = 90;
    mock_tof_data.target_status[20] = TOF_STATUS_VALID_5;
    ToF_SimulateData(&mock_tof_data);

    // Act - Ler dados do ToF
    ToF_ResultsData tof_results;
    ToF_GetData(&tof_results);

    // Encontrar distância mais próxima
    int nearest_zone = -1;
    uint16_t nearest_dist = ToF_FindNearestDistance(&tof_results, &nearest_zone);

    // Processar emergency stop
    EmergencyStopState_t state = Emergency_ProcessDistance(nearest_dist);

    // Assert
    EXPECT_EQ(nearest_dist, 90);
    EXPECT_EQ(nearest_zone, 20);
    EXPECT_EQ(state, ESTOP_STATE_EMERGENCY);
    EXPECT_FALSE(motor_is_running());
}

/**
 * INT-04: ToF WARNING state (100mm < dist < 200mm)
 */
TEST_F(IntegrationTest, ToF_Emergency_WarningState) {
    // Arrange
    ToF_Init();
    ToF_StartRanging();
    Emergency_Init();

    // Simular ToF: objeto a 150mm
    mock_tof_data.distance_mm[10] = 150;
    mock_tof_data.target_status[10] = TOF_STATUS_VALID_5;
    ToF_SimulateData(&mock_tof_data);

    // Act
    ToF_ResultsData tof_results;
    ToF_GetData(&tof_results);
    uint16_t nearest_dist = ToF_FindNearestDistance(&tof_results, NULL);
    EmergencyStopState_t state = Emergency_ProcessDistance(nearest_dist);

    // Assert
    EXPECT_EQ(nearest_dist, 150);
    EXPECT_EQ(state, ESTOP_STATE_WARNING);
    EXPECT_FALSE(Emergency_IsActive());  // WARNING não para o motor
}

/**
 * INT-05: Servo + Motor - Controle coordenado
 * Virar à esquerda enquanto avança
 */
TEST_F(IntegrationTest, Servo_Motor_CoordinatedControl) {
    // Arrange
    Servo_Init(&mock_tim, TIM_CHANNEL_1);
    Motor_Init(&mock_i2c);

    // Act - Virar à esquerda (0°) e avançar
    Servo_SetAngle(0);      // Esquerda máxima
    Motor_Forward(60);       // Avançar a 60%

    // Assert
    EXPECT_EQ(Servo_GetAngle(), 0);
    EXPECT_TRUE(motor_is_running());

    // Verificar PWM do servo
    uint32_t servo_pwm = mock_tim_get_compare();
    EXPECT_EQ(servo_pwm, SERVO_PULSE_MIN_US);  // 1000us = 0°
}

/**
 * INT-06: Servo + Motor - Virar à direita e avançar
 */
TEST_F(IntegrationTest, Servo_Motor_TurnRight) {
    // Arrange
    Servo_Init(&mock_tim, TIM_CHANNEL_1);
    Motor_Init(&mock_i2c);

    // Act - Virar à direita (180°) e avançar
    Servo_SetAngle(180);    // Direita máxima
    Motor_Forward(70);       // Avançar a 70%

    // Assert
    EXPECT_EQ(Servo_GetAngle(), 180);
    EXPECT_TRUE(motor_is_running());

    uint32_t servo_pwm = mock_tim_get_compare();
    EXPECT_EQ(servo_pwm, SERVO_PULSE_MAX_US);  // 2000us = 180°
}

/**
 * INT-07: Servo + Motor - Centro e ré
 */
TEST_F(IntegrationTest, Servo_Motor_CenterAndReverse) {
    // Arrange
    Servo_Init(&mock_tim, TIM_CHANNEL_1);
    Motor_Init(&mock_i2c);

    // Act - Centro (90°) e marcha atrás
    Servo_Center();
    Motor_Backward(50);

    // Assert
    EXPECT_EQ(Servo_GetAngle(), 90);
    EXPECT_TRUE(motor_is_running());

    uint32_t servo_pwm = mock_tim_get_compare();
    EXPECT_EQ(servo_pwm, SERVO_PULSE_CENTER_US);  // 1500us = 90°
}

/**
 * INT-08: Multiple ToF zones - Objeto mais próximo
 * Múltiplos objetos detectados, encontrar o mais perigoso
 */
TEST_F(IntegrationTest, MultipleObjects_FindClosest) {
    // Arrange
    ToF_Init();
    ToF_StartRanging();
    Emergency_Init();

    // Simular múltiplos objetos
    mock_tof_data.distance_mm[5] = 400;   // Longe
    mock_tof_data.target_status[5] = TOF_STATUS_VALID_5;

    mock_tof_data.distance_mm[15] = 80;   // PERIGOSO!
    mock_tof_data.target_status[15] = TOF_STATUS_VALID_9;

    mock_tof_data.distance_mm[35] = 250;  // Médio
    mock_tof_data.target_status[35] = TOF_STATUS_VALID_5;

    ToF_SimulateData(&mock_tof_data);

    // Act
    ToF_ResultsData tof_results;
    ToF_GetData(&tof_results);

    int nearest_zone = -1;
    uint16_t nearest_dist = ToF_FindNearestDistance(&tof_results, &nearest_zone);

    EmergencyStopState_t state = Emergency_ProcessDistance(nearest_dist);

    // Assert - Deve detectar o objeto a 80mm (zona 15)
    EXPECT_EQ(nearest_dist, 80);
    EXPECT_EQ(nearest_zone, 15);
    EXPECT_EQ(state, ESTOP_STATE_EMERGENCY);
}

/**
 * INT-09: ToF status inválido ignorado
 */
TEST_F(IntegrationTest, ToF_IgnoreInvalidStatus_EmergencyCheck) {
    // Arrange
    ToF_Init();
    ToF_StartRanging();
    Emergency_Init();

    // Simular objeto próximo MAS com status inválido
    mock_tof_data.distance_mm[10] = 50;  // Muito próximo
    mock_tof_data.target_status[10] = TOF_STATUS_INVALID;  // Status inválido

    // Objeto longe com status válido
    mock_tof_data.distance_mm[20] = 300;
    mock_tof_data.target_status[20] = TOF_STATUS_VALID_5;

    ToF_SimulateData(&mock_tof_data);

    // Act
    ToF_ResultsData tof_results;
    ToF_GetData(&tof_results);
    uint16_t nearest_dist = ToF_FindNearestDistance(&tof_results, NULL);
    EmergencyStopState_t state = Emergency_ProcessDistance(nearest_dist);

    // Assert - Deve usar 300mm (ignorar 50mm inválido)
    EXPECT_EQ(nearest_dist, 300);
    EXPECT_EQ(state, ESTOP_STATE_NORMAL);
}

/**
 * INT-10: Emergency Stop durante manobra
 * Veículo está virando quando objeto aparece
 */
TEST_F(IntegrationTest, Emergency_DuringManeuver) {
    // Arrange - Veículo virando à esquerda e avançando
    Servo_Init(&mock_tim, TIM_CHANNEL_1);
    Motor_Init(&mock_i2c);
    Emergency_Init();

    Servo_SetAngle(45);     // Virando à esquerda
    Motor_Forward(70);       // Avançando
    EXPECT_TRUE(motor_is_running());

    // Act - Objeto repentino a 60mm
    EmergencyStopState_t state = Emergency_ProcessDistance(60);

    // Assert - Motor deve parar, mas servo mantém posição
    EXPECT_EQ(state, ESTOP_STATE_EMERGENCY);
    EXPECT_FALSE(motor_is_running());
    EXPECT_EQ(Servo_GetAngle(), 45) << "Servo deve manter posição durante emergency";
}

/**
 * INT-11: Speedometer + Motor - Acelerar e medir
 * NOTA: Teste simplificado - apenas verifica que motor está a funcionar
 */
TEST_F(IntegrationTest, Speedometer_Motor_Acceleration) {
    // Arrange
    Motor_Init(&mock_i2c);
    Speedometer_Init();

    // Act - Acelerar motor
    Motor_Forward(80);

    // Simular pulsos
    for (int i = 0; i < 18; i++) {
        Speedometer_CountPulse();
    }

    // Assert - Motor está a funcionar e pulsos foram contados
    EXPECT_TRUE(motor_is_running());
    // RPM calculation requires time delta, which is not easily mockable in unit tests
    // This is better tested in integration tests with real hardware
}

/**
 * INT-12: Sistema completo - Cenário de uso real
 * Motor avança → ToF detecta objeto → Emergency para motor → Objeto sai → Motor retoma
 */
TEST_F(IntegrationTest, CompleteSystem_RealScenario) {
    // === FASE 1: Inicialização ===
    ToF_Init();
    ToF_StartRanging();
    Motor_Init(&mock_i2c);
    Servo_Init(&mock_tim, TIM_CHANNEL_1);
    Emergency_Init();

    // === FASE 2: Veículo avançando ===
    Servo_Center();
    Motor_Forward(60);
    EXPECT_TRUE(motor_is_running());
    EXPECT_EQ(Servo_GetAngle(), 90);

    // ToF: caminho livre (500mm)
    mock_tof_data.distance_mm[35] = 500;
    mock_tof_data.target_status[35] = TOF_STATUS_VALID_5;
    ToF_SimulateData(&mock_tof_data);

    ToF_ResultsData tof_results1;
    ToF_GetData(&tof_results1);
    uint16_t dist1 = ToF_FindNearestDistance(&tof_results1, NULL);
    EmergencyStopState_t state1 = Emergency_ProcessDistance(dist1);

    EXPECT_EQ(state1, ESTOP_STATE_NORMAL);
    EXPECT_TRUE(motor_is_running());

    // === FASE 3: Objeto aparece (70mm) ===
    mock_tof_data.distance_mm[35] = 70;
    ToF_SimulateData(&mock_tof_data);

    ToF_ResultsData tof_results2;
    ToF_GetData(&tof_results2);
    uint16_t dist2 = ToF_FindNearestDistance(&tof_results2, NULL);
    EmergencyStopState_t state2 = Emergency_ProcessDistance(dist2);

    EXPECT_EQ(state2, ESTOP_STATE_EMERGENCY);
    EXPECT_FALSE(motor_is_running()) << "Motor deve parar";

    // === FASE 4: Objeto se afasta (600mm) ===
    mock_tof_data.distance_mm[35] = 600;
    ToF_SimulateData(&mock_tof_data);

    ToF_ResultsData tof_results3;
    ToF_GetData(&tof_results3);
    uint16_t dist3 = ToF_FindNearestDistance(&tof_results3, NULL);
    EmergencyStopState_t state3 = Emergency_ProcessDistance(dist3);

    EXPECT_EQ(state3, ESTOP_STATE_NORMAL);

    // === FASE 5: Retomar movimento ===
    Motor_Forward(60);
    EXPECT_TRUE(motor_is_running());
}

/**
 * INT-13: Transição de estados Emergency: NORMAL → WARNING → EMERGENCY
 */
TEST_F(IntegrationTest, Emergency_StateTransitions) {
    // Arrange
    Emergency_Init();
    Motor_Init(&mock_i2c);
    Motor_Forward(50);

    // Estado inicial: NORMAL (> 200mm)
    EmergencyStopState_t state1 = Emergency_ProcessDistance(500);
    EXPECT_EQ(state1, ESTOP_STATE_NORMAL);
    EXPECT_TRUE(motor_is_running());

    // Transição: NORMAL → WARNING (100-200mm)
    EmergencyStopState_t state2 = Emergency_ProcessDistance(150);
    EXPECT_EQ(state2, ESTOP_STATE_WARNING);
    EXPECT_TRUE(motor_is_running());  // WARNING não para motor

    // Transição: WARNING → EMERGENCY (< 100mm)
    EmergencyStopState_t state3 = Emergency_ProcessDistance(80);
    EXPECT_EQ(state3, ESTOP_STATE_EMERGENCY);
    EXPECT_FALSE(motor_is_running());

    // Transição: EMERGENCY → WARNING (100-200mm)
    EmergencyStopState_t state4 = Emergency_ProcessDistance(150);
    EXPECT_EQ(state4, ESTOP_STATE_WARNING);

    // Transição: WARNING → NORMAL (> 200mm)
    EmergencyStopState_t state5 = Emergency_ProcessDistance(300);
    EXPECT_EQ(state5, ESTOP_STATE_NORMAL);
}
