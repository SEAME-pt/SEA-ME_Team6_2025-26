/**
 * Testes unitários para o ToF Sensor (VL53L5CX)
 * Usando Google Test Framework
 */

#include <gtest/gtest.h>

extern "C" {
    #include "tof_testable.h"
    #include "stm32u5xx_hal.h"
}

/**
 * Fixture para os testes do ToF
 */
class ToFTest : public ::testing::Test {
protected:
    ToF_ResultsData mock_results;

    void SetUp() override {
        // Reset ToF state
        ToF_Reset();

        // Initialize mock results
        mock_results.nb_zones = TOF_RESOLUTION_8X8;
        for (int i = 0; i < TOF_MAX_ZONES; i++) {
            mock_results.distance_mm[i] = 0;
            mock_results.target_status[i] = TOF_STATUS_INVALID;
        }
    }

    void TearDown() override {
        ToF_Reset();
    }
};

/**
 * TOF-01: Inicialização do sensor
 */
TEST_F(ToFTest, InitializeSensor) {
    // Act
    HAL_StatusTypeDef status = ToF_Init();

    // Assert
    EXPECT_EQ(status, HAL_OK);
}

/**
 * TOF-02: Inicialização dupla deve falhar
 */
TEST_F(ToFTest, DoubleInitializationFails) {
    // Arrange
    ToF_Init();

    // Act - Tentar inicializar novamente
    HAL_StatusTypeDef status = ToF_Init();

    // Assert
    EXPECT_EQ(status, HAL_ERROR);
}

/**
 * TOF-03: Start ranging
 */
TEST_F(ToFTest, StartRanging) {
    // Arrange
    ToF_Init();

    // Act
    HAL_StatusTypeDef status = ToF_StartRanging();

    // Assert
    EXPECT_EQ(status, HAL_OK);
}

/**
 * TOF-04: Start ranging sem init deve falhar
 */
TEST_F(ToFTest, StartRangingWithoutInit) {
    // Act (sem init)
    HAL_StatusTypeDef status = ToF_StartRanging();

    // Assert
    EXPECT_EQ(status, HAL_ERROR);
}

/**
 * TOF-05: Stop ranging
 */
TEST_F(ToFTest, StopRanging) {
    // Arrange
    ToF_Init();
    ToF_StartRanging();

    // Act
    HAL_StatusTypeDef status = ToF_StopRanging();

    // Assert
    EXPECT_EQ(status, HAL_OK);
}

/**
 * TOF-06: Stop sem ranging ativo deve falhar
 */
TEST_F(ToFTest, StopWithoutRanging) {
    // Arrange
    ToF_Init();

    // Act (sem start ranging)
    HAL_StatusTypeDef status = ToF_StopRanging();

    // Assert
    EXPECT_EQ(status, HAL_ERROR);
}

/**
 * TOF-07: Leitura de dados válidos
 */
TEST_F(ToFTest, GetValidData) {
    // Arrange
    ToF_Init();
    ToF_StartRanging();

    // Simular dados
    mock_results.distance_mm[0] = 500;
    mock_results.target_status[0] = TOF_STATUS_VALID_5;
    ToF_SimulateData(&mock_results);

    // Act
    ToF_ResultsData results;
    HAL_StatusTypeDef status = ToF_GetData(&results);

    // Assert
    EXPECT_EQ(status, HAL_OK);
    EXPECT_EQ(results.distance_mm[0], 500);
    EXPECT_EQ(results.target_status[0], TOF_STATUS_VALID_5);
    EXPECT_EQ(results.nb_zones, 64);
}

/**
 * TOF-08: GetData sem ranging ativo deve falhar
 */
TEST_F(ToFTest, GetDataWithoutRanging) {
    // Arrange
    ToF_Init();
    ToF_ResultsData results;

    // Act (sem start ranging)
    HAL_StatusTypeDef status = ToF_GetData(&results);

    // Assert
    EXPECT_EQ(status, HAL_ERROR);
}

/**
 * TOF-09: GetData com NULL pointer deve falhar
 */
TEST_F(ToFTest, GetDataWithNullPointer) {
    // Arrange
    ToF_Init();
    ToF_StartRanging();

    // Act
    HAL_StatusTypeDef status = ToF_GetData(NULL);

    // Assert
    EXPECT_EQ(status, HAL_ERROR);
}

/**
 * TOF-10: Validação de status válido (5)
 */
TEST_F(ToFTest, StatusValidation_Status5) {
    // Act
    uint8_t is_valid = ToF_IsStatusValid(TOF_STATUS_VALID_5);

    // Assert
    EXPECT_EQ(is_valid, 1);
}

/**
 * TOF-11: Validação de status válido (9)
 */
TEST_F(ToFTest, StatusValidation_Status9) {
    // Act
    uint8_t is_valid = ToF_IsStatusValid(TOF_STATUS_VALID_9);

    // Assert
    EXPECT_EQ(is_valid, 1);
}

/**
 * TOF-12: Validação de status inválido
 */
TEST_F(ToFTest, StatusValidation_InvalidStatus) {
    // Act & Assert
    EXPECT_EQ(ToF_IsStatusValid(0), 0);
    EXPECT_EQ(ToF_IsStatusValid(1), 0);
    EXPECT_EQ(ToF_IsStatusValid(255), 0);
}

/**
 * TOF-13: Encontrar objeto mais próximo em zona única
 */
TEST_F(ToFTest, FindNearestDistance_SingleZone) {
    // Arrange
    mock_results.distance_mm[10] = 300;
    mock_results.target_status[10] = TOF_STATUS_VALID_5;

    // Act
    int nearest_zone = -1;
    uint16_t nearest_dist = ToF_FindNearestDistance(&mock_results, &nearest_zone);

    // Assert
    EXPECT_EQ(nearest_dist, 300);
    EXPECT_EQ(nearest_zone, 10);
}

/**
 * TOF-14: Encontrar objeto mais próximo entre múltiplas zonas
 */
TEST_F(ToFTest, FindNearestDistance_MultipleZones) {
    // Arrange - 3 objetos a diferentes distâncias
    mock_results.distance_mm[5] = 500;
    mock_results.target_status[5] = TOF_STATUS_VALID_5;

    mock_results.distance_mm[20] = 150;  // Mais próximo
    mock_results.target_status[20] = TOF_STATUS_VALID_9;

    mock_results.distance_mm[35] = 800;
    mock_results.target_status[35] = TOF_STATUS_VALID_5;

    // Act
    int nearest_zone = -1;
    uint16_t nearest_dist = ToF_FindNearestDistance(&mock_results, &nearest_zone);

    // Assert
    EXPECT_EQ(nearest_dist, 150);
    EXPECT_EQ(nearest_zone, 20);
}

/**
 * TOF-15: Ignorar zonas com status inválido
 */
TEST_F(ToFTest, FindNearestDistance_IgnoreInvalidStatus) {
    // Arrange
    mock_results.distance_mm[10] = 100;  // Mais próximo mas status inválido
    mock_results.target_status[10] = TOF_STATUS_INVALID;

    mock_results.distance_mm[20] = 300;  // Status válido
    mock_results.target_status[20] = TOF_STATUS_VALID_5;

    // Act
    int nearest_zone = -1;
    uint16_t nearest_dist = ToF_FindNearestDistance(&mock_results, &nearest_zone);

    // Assert - Deve retornar 300mm (ignorar 100mm com status inválido)
    EXPECT_EQ(nearest_dist, 300);
    EXPECT_EQ(nearest_zone, 20);
}

/**
 * TOF-16: Sem objetos detectados
 */
TEST_F(ToFTest, FindNearestDistance_NoObjects) {
    // Arrange - Todos com status inválido ou distância zero
    for (int i = 0; i < TOF_MAX_ZONES; i++) {
        mock_results.distance_mm[i] = 0;
        mock_results.target_status[i] = TOF_STATUS_INVALID;
    }

    // Act
    int nearest_zone = -1;
    uint16_t nearest_dist = ToF_FindNearestDistance(&mock_results, &nearest_zone);

    // Assert
    EXPECT_EQ(nearest_dist, 0);
    EXPECT_EQ(nearest_zone, -1);
}

/**
 * TOF-17: Threshold de EMERGÊNCIA (< 100mm)
 */
TEST_F(ToFTest, EmergencyThreshold_Detection) {
    // Arrange - Objeto a 80mm
    mock_results.distance_mm[0] = 80;
    mock_results.target_status[0] = TOF_STATUS_VALID_5;

    // Act
    uint16_t nearest_dist = ToF_FindNearestDistance(&mock_results, NULL);

    // Assert
    EXPECT_LT(nearest_dist, TOF_THRESHOLD_EMERGENCY);
    EXPECT_EQ(nearest_dist, 80);
}

/**
 * TOF-18: Threshold de WARNING (< 200mm)
 */
TEST_F(ToFTest, WarningThreshold_Detection) {
    // Arrange - Objeto a 150mm
    mock_results.distance_mm[0] = 150;
    mock_results.target_status[0] = TOF_STATUS_VALID_5;

    // Act
    uint16_t nearest_dist = ToF_FindNearestDistance(&mock_results, NULL);

    // Assert
    EXPECT_LT(nearest_dist, TOF_THRESHOLD_WARNING);
    EXPECT_GT(nearest_dist, TOF_THRESHOLD_EMERGENCY);
    EXPECT_EQ(nearest_dist, 150);
}

/**
 * TOF-19: Distância acima de WARNING (zona segura)
 */
TEST_F(ToFTest, SafeDistance_Detection) {
    // Arrange - Objeto a 500mm
    mock_results.distance_mm[0] = 500;
    mock_results.target_status[0] = TOF_STATUS_VALID_5;

    // Act
    uint16_t nearest_dist = ToF_FindNearestDistance(&mock_results, NULL);

    // Assert
    EXPECT_GT(nearest_dist, TOF_THRESHOLD_WARNING);
    EXPECT_EQ(nearest_dist, 500);
}

/**
 * TOF-20: Todas as 64 zonas (8x8)
 */
TEST_F(ToFTest, All64Zones_Processing) {
    // Arrange - Preencher todas as 64 zonas
    for (int i = 0; i < 64; i++) {
        mock_results.distance_mm[i] = 1000 + i * 10;  // 1000, 1010, 1020, ...
        mock_results.target_status[i] = TOF_STATUS_VALID_5;
    }

    // Colocar objeto mais próximo na zona 50
    mock_results.distance_mm[50] = 200;

    // Act
    int nearest_zone = -1;
    uint16_t nearest_dist = ToF_FindNearestDistance(&mock_results, &nearest_zone);

    // Assert
    EXPECT_EQ(nearest_dist, 200);
    EXPECT_EQ(nearest_zone, 50);
}

/**
 * TOF-21: FindNearestDistance com NULL pointer
 */
TEST_F(ToFTest, FindNearest_WithNullPointer) {
    // Act
    uint16_t nearest_dist = ToF_FindNearestDistance(NULL, NULL);

    // Assert
    EXPECT_EQ(nearest_dist, TOF_MAX_DISTANCE_MM);
}

/**
 * TOF-22: Distâncias negativas devem ser ignoradas
 */
TEST_F(ToFTest, IgnoreNegativeDistances) {
    // Arrange
    mock_results.distance_mm[0] = -100;  // Negativo
    mock_results.target_status[0] = TOF_STATUS_VALID_5;

    mock_results.distance_mm[10] = 300;  // Válido
    mock_results.target_status[10] = TOF_STATUS_VALID_5;

    // Act
    uint16_t nearest_dist = ToF_FindNearestDistance(&mock_results, NULL);

    // Assert - Deve ignorar distância negativa
    EXPECT_EQ(nearest_dist, 300);
}

/**
 * TOF-23: Zona central (aprox. zona 35) para referência
 */
TEST_F(ToFTest, CenterZone_Reference) {
    // Arrange - Zona central tem objeto
    mock_results.distance_mm[35] = 600;
    mock_results.target_status[35] = TOF_STATUS_VALID_5;

    // Act
    ToF_Init();
    ToF_StartRanging();
    ToF_SimulateData(&mock_results);  // Simular após init

    ToF_ResultsData results;
    ToF_GetData(&results);

    // Assert
    EXPECT_EQ(results.distance_mm[35], 600);
}

/**
 * TOF-24: Múltiplas leituras consecutivas
 */
TEST_F(ToFTest, MultipleConsecutiveReadings) {
    // Arrange
    ToF_Init();
    ToF_StartRanging();

    // Leitura 1 - 500mm
    mock_results.distance_mm[0] = 500;
    mock_results.target_status[0] = TOF_STATUS_VALID_5;
    ToF_SimulateData(&mock_results);

    ToF_ResultsData results1;
    ToF_GetData(&results1);
    EXPECT_EQ(results1.distance_mm[0], 500);

    // Leitura 2 - 300mm (objeto se aproximando)
    mock_results.distance_mm[0] = 300;
    ToF_SimulateData(&mock_results);

    ToF_ResultsData results2;
    ToF_GetData(&results2);
    EXPECT_EQ(results2.distance_mm[0], 300);

    // Leitura 3 - 100mm (objeto muito próximo)
    mock_results.distance_mm[0] = 100;
    ToF_SimulateData(&mock_results);

    ToF_ResultsData results3;
    ToF_GetData(&results3);
    EXPECT_EQ(results3.distance_mm[0], 100);
}

/**
 * TOF-25: Ciclo completo: Init -> Start -> Get -> Stop
 */
TEST_F(ToFTest, CompleteOperationCycle) {
    // Init
    EXPECT_EQ(ToF_Init(), HAL_OK);

    // Start
    EXPECT_EQ(ToF_StartRanging(), HAL_OK);

    // Simulate data
    mock_results.distance_mm[0] = 400;
    mock_results.target_status[0] = TOF_STATUS_VALID_5;
    ToF_SimulateData(&mock_results);

    // Get
    ToF_ResultsData results;
    EXPECT_EQ(ToF_GetData(&results), HAL_OK);
    EXPECT_EQ(results.distance_mm[0], 400);

    // Stop
    EXPECT_EQ(ToF_StopRanging(), HAL_OK);
}
