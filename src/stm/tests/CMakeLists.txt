cmake_minimum_required(VERSION 3.14)
project(STM32_UnitTests)

# Configurar C++ 11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Adicionar flag de compilação para testes
add_definitions(-DUNIT_TESTING)

# Adicionar Google Test
add_subdirectory(external/googletest)

# Incluir diretórios
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    ${CMAKE_CURRENT_SOURCE_DIR}/unit
    ${gtest_SOURCE_DIR}/include
    ${gtest_SOURCE_DIR}
)

# Arquivos de mock (compartilhados por todos os testes)
set(MOCK_SOURCES
    mocks/stm32_mocks.cpp
)

# ============================================================================
# TESTE 1: Speedometer
# ============================================================================
add_executable(speedometer_tests
    unit/speedometer_testable.c
    ${MOCK_SOURCES}
    unit/test_speedometer.cpp
)

target_link_libraries(speedometer_tests
    gtest
    gtest_main
    pthread
)

# ============================================================================
# TESTE 2: Motor Control
# ============================================================================
add_executable(motor_control_tests
    unit/motor_control_testable.c
    ${MOCK_SOURCES}
    unit/test_motor_control.cpp
)

target_link_libraries(motor_control_tests
    gtest
    gtest_main
    pthread
)

# ============================================================================
# TESTE 3: Emergency Stop
# ============================================================================
add_executable(emergency_stop_tests
    unit/emergency_stop_testable.c
    ${MOCK_SOURCES}
    unit/test_emergency_stop.cpp
)

target_link_libraries(emergency_stop_tests
    gtest
    gtest_main
    pthread
)

# ============================================================================
# TESTE 4: Servo Control
# ============================================================================
add_executable(servo_tests
    unit/servo_testable.c
    ${MOCK_SOURCES}
    unit/test_servo.cpp
)

target_link_libraries(servo_tests
    gtest
    gtest_main
    pthread
)

# ============================================================================
# TESTE 5: ToF Sensor (VL53L5CX)
# ============================================================================
add_executable(tof_tests
    unit/tof_testable.c
    ${MOCK_SOURCES}
    unit/test_tof.cpp
)

target_link_libraries(tof_tests
    gtest
    gtest_main
    pthread
)

# ============================================================================
# TESTE 6: Integration Tests
# ============================================================================
add_executable(integration_tests
    unit/speedometer_testable.c
    unit/motor_control_testable.c
    unit/emergency_stop_testable.c
    unit/servo_testable.c
    unit/tof_testable.c
    ${MOCK_SOURCES}
    unit/test_integration.cpp
)

target_link_libraries(integration_tests
    gtest
    gtest_main
    pthread
)

# ============================================================================
# TESTE COMPLETO: Todos os testes juntos
# ============================================================================
add_executable(all_tests
    unit/speedometer_testable.c
    unit/motor_control_testable.c
    unit/emergency_stop_testable.c
    unit/servo_testable.c
    unit/tof_testable.c
    ${MOCK_SOURCES}
    unit/test_speedometer.cpp
    unit/test_motor_control.cpp
    unit/test_emergency_stop.cpp
    unit/test_servo.cpp
    unit/test_tof.cpp
    unit/test_integration.cpp
)

target_link_libraries(all_tests
    gtest
    gtest_main
    pthread
)

# Habilitar testes
enable_testing()
add_test(NAME speedometer_tests COMMAND speedometer_tests)
add_test(NAME motor_control_tests COMMAND motor_control_tests)
add_test(NAME emergency_stop_tests COMMAND emergency_stop_tests)
add_test(NAME servo_tests COMMAND servo_tests)
add_test(NAME tof_tests COMMAND tof_tests)
add_test(NAME integration_tests COMMAND integration_tests)
add_test(NAME all_tests COMMAND all_tests)
