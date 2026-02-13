cmake_minimum_required(VERSION 3.14)
project(STM32_UnitTests)

# Configurar C++ 11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Adicionar flag de compilação para testes
add_definitions(-DUNIT_TESTING)

## Prefer system-installed GoogleTest; fall back to external/googletest if needed
find_package(GTest QUIET)
if (GTest_FOUND)
    message(STATUS "Found system GTest: ${GTest_VERSION}")
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/mocks
        ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Inc
        ${GTEST_INCLUDE_DIRS}
    )
else()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/googletest/CMakeLists.txt")
        add_subdirectory(external/googletest)
        include_directories(
            ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/mocks
            ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp
            ${gtest_SOURCE_DIR}/include
            ${gtest_SOURCE_DIR}
        )
    else()
        message(FATAL_ERROR "GoogleTest not found. Run tests/setup_gtest.sh to download it or install system package 'libgtest-dev'.")
    endif()
endif()

# Arquivos de mock (compartilhados por todos os testes)
set(MOCK_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/mocks/stm32_mocks.cpp
)

# ============================================================================
# TESTE 1: Speedometer
# ============================================================================
add_executable(speedometer_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/speedometer_testable.c
    ${MOCK_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/test_speedometer.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/motor_control_testable.c
    ${MOCK_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/test_motor_control.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/emergency_stop_testable.c
    ${MOCK_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/test_emergency_stop.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/servo_testable.c
    ${MOCK_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/test_servo.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/tof_testable.c
    ${MOCK_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/test_tof.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/speedometer_testable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/motor_control_testable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/emergency_stop_testable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/servo_testable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/tof_testable.c
    ${MOCK_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/test_integration.cpp
)

target_link_libraries(integration_tests
    gtest
    gtest_main
    pthread
)

# ============================================================================
# TESTE: IIS2MDC
# ============================================================================
add_executable(iis2mdc_tests
    ${MOCK_SOURCES}
    ../Core/Src/iis2mdc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/test_iis2mdc.cpp
)

target_link_libraries(iis2mdc_tests
    gtest
    gtest_main
    pthread
)

add_test(NAME iis2mdc_tests COMMAND iis2mdc_tests)

# Ensure mocks are visible first when compiling the production source
target_include_directories(iis2mdc_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/mocks
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Inc
)

# ============================================================================
# TESTE: ISM330DHCX
# ============================================================================
add_executable(ism330dhcx_tests
    ${MOCK_SOURCES}
    ../Core/Src/ism330dhcx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/test_ism330dhcx.cpp
)

target_link_libraries(ism330dhcx_tests
    gtest
    gtest_main
    pthread
)

add_test(NAME ism330dhcx_tests COMMAND ism330dhcx_tests)

# Ensure mocks are visible first when compiling the production source
target_include_directories(ism330dhcx_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/mocks
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Inc
)

# ============================================================================
# TESTE: HTS221
# ============================================================================
add_executable(hts221_tests
    ${MOCK_SOURCES}
    ../Core/Src/hts221.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/test_hts221.cpp
)

target_link_libraries(hts221_tests
    gtest
    gtest_main
    pthread
)

add_test(NAME hts221_tests COMMAND hts221_tests)

target_include_directories(hts221_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/mocks
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Inc
)

# ============================================================================
# TESTE: LPS22HH
# ============================================================================
add_executable(lps22hh_tests
    ${MOCK_SOURCES}
    ../Core/Src/lps22hh.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/test_lps22hh.cpp
)

target_link_libraries(lps22hh_tests
    gtest
    gtest_main
    pthread
)

add_test(NAME lps22hh_tests COMMAND lps22hh_tests)

target_include_directories(lps22hh_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/mocks
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Inc
)

# ============================================================================
# TESTE: VEML6030
# ============================================================================
add_executable(veml6030_tests
    ${MOCK_SOURCES}
    ../Core/Src/veml6030.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/test_veml6030.cpp
)

target_link_libraries(veml6030_tests
    gtest
    gtest_main
    pthread
)

add_test(NAME veml6030_tests COMMAND veml6030_tests)

target_include_directories(veml6030_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/mocks
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Inc
)

# ============================================================================
# TESTE COMPLETO: Todos os testes juntos
# ============================================================================
add_executable(all_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/speedometer_testable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/motor_control_testable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/emergency_stop_testable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/servo_testable.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/tof_testable.c
    ${MOCK_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/test_speedometer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/test_motor_control.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/test_emergency_stop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/test_servo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/test_tof.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/unit/cpp/test_integration.cpp
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
