cmake_minimum_required(VERSION 3.10)
project(SEA_ME_Team6_Tests LANGUAGES C CXX)

# -----------------------
# C++ standard
# -----------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt moc (keep ON if your backend code uses Q_OBJECT)
set(CMAKE_AUTOMOC ON)

# -----------------------
# Options
# -----------------------
option(ENABLE_CPP_TESTS "Enable C++ unit tests" ON)
option(ENABLE_PYTHON_TESTS "Enable Python Unit Tests" ON) # kept for your existing coverage modules
option(ENABLE_COVERAGE "Enable coverage flags" ON)

# -----------------------
# Testing
# -----------------------
include(CTest)
enable_testing()

# -----------------------
# Python tests integration
# -----------------------
if(ENABLE_PYTHON_TESTS)
  # Require an interpreter (we will call pytest via -m pytest)
  find_package(Python COMPONENTS Interpreter REQUIRED)

  # Add a CTest entry that runs pytest for the python unit tests folder.
  # This keeps python tests visible to ctest and to CI that calls ctest.
  add_test(NAME PyTests
    COMMAND ${Python_EXECUTABLE} -m pytest -q ${CMAKE_SOURCE_DIR}/unit/python
  )
  set_tests_properties(PyTests PROPERTIES LABELS "python")

  # Also provide a convenience build target so devs can run: cmake --build . --target pytests
  add_custom_target(pytests
    COMMAND ${Python_EXECUTABLE} -m pytest -q ${CMAKE_SOURCE_DIR}/unit/python
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/unit/python
    VERBATIM
  )
endif()

# -----------------------
# GoogleTest
# -----------------------
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

# ---------------------------------------------------------------------------
# STM unit tests (inlined here so this single CMake controls all tests)
# ---------------------------------------------------------------------------

# Enable UNIT_TESTING define for production sources compiled for host
add_compile_definitions(UNIT_TESTING)

find_package(Threads REQUIRED)

# Shared mocks implementation used by STM tests
set(MOCK_SOURCES
  ${CMAKE_SOURCE_DIR}/unit/mocks/stm32_mocks.cpp
)

# Helper macro to reduce repetition for adding simple STM tests
function(add_stm_test name)
  add_executable(${name} ${ARGN} ${MOCK_SOURCES})
  target_link_libraries(${name} PRIVATE
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
  )
  # default include order: mocks -> unit cpp -> stm core include
  target_include_directories(${name} PRIVATE
    ${CMAKE_SOURCE_DIR}/unit/mocks
    ${CMAKE_SOURCE_DIR}/unit/cpp
    ${CMAKE_SOURCE_DIR}/../stm/Core/Inc
  )
  gtest_discover_tests(${name})
endfunction()

# ==========================================================================
# STM: simple test executables that exercise testable C wrappers
# ==========================================================================
add_stm_test(speedometer_tests
  ${CMAKE_SOURCE_DIR}/unit/cpp/speedometer_testable.c
  ${CMAKE_SOURCE_DIR}/unit/cpp/test_speedometer.cpp
)

add_stm_test(motor_control_tests
  ${CMAKE_SOURCE_DIR}/unit/cpp/motor_control_testable.c
  ${CMAKE_SOURCE_DIR}/unit/cpp/test_motor_control.cpp
)

add_stm_test(emergency_stop_tests
  ${CMAKE_SOURCE_DIR}/unit/cpp/emergency_stop_testable.c
  ${CMAKE_SOURCE_DIR}/unit/cpp/test_emergency_stop.cpp
)

add_stm_test(servo_tests
  ${CMAKE_SOURCE_DIR}/unit/cpp/servo_testable.c
  ${CMAKE_SOURCE_DIR}/unit/cpp/test_servo.cpp
)

add_stm_test(tof_tests
  ${CMAKE_SOURCE_DIR}/unit/cpp/tof_testable.c
  ${CMAKE_SOURCE_DIR}/unit/cpp/test_tof.cpp
)

# Integration / all_tests that compile multiple "testable" C files together
add_executable(integration_tests
  ${CMAKE_SOURCE_DIR}/unit/cpp/speedometer_testable.c
  ${CMAKE_SOURCE_DIR}/unit/cpp/motor_control_testable.c
  ${CMAKE_SOURCE_DIR}/unit/cpp/emergency_stop_testable.c
  ${CMAKE_SOURCE_DIR}/unit/cpp/servo_testable.c
  ${CMAKE_SOURCE_DIR}/unit/cpp/tof_testable.c
  ${MOCK_SOURCES}
  ${CMAKE_SOURCE_DIR}/unit/cpp/test_integration.cpp
)
target_link_libraries(integration_tests PRIVATE
  GTest::gtest
  GTest::gtest_main
  Threads::Threads
)
target_include_directories(integration_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/unit/mocks
  ${CMAKE_SOURCE_DIR}/unit/cpp
  ${CMAKE_SOURCE_DIR}/../stm/Core/Inc
)
gtest_discover_tests(integration_tests)

# ==========================================================================
# STM sensor driver unit tests (compile production C driver with mocks)
# ==========================================================================
add_executable(iis2mdc_tests
  ${MOCK_SOURCES}
  ${CMAKE_SOURCE_DIR}/../stm/Core/Src/iis2mdc.c
  ${CMAKE_SOURCE_DIR}/unit/cpp/test_iis2mdc.cpp
)
target_link_libraries(iis2mdc_tests PRIVATE
  GTest::gtest
  GTest::gtest_main
  Threads::Threads
)
target_include_directories(iis2mdc_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/unit/mocks
  ${CMAKE_SOURCE_DIR}/unit/cpp
  ${CMAKE_SOURCE_DIR}/../stm/Core/Inc
)
gtest_discover_tests(iis2mdc_tests)

add_executable(ism330dhcx_tests
  ${MOCK_SOURCES}
  ${CMAKE_SOURCE_DIR}/../stm/Core/Src/ism330dhcx.c
  ${CMAKE_SOURCE_DIR}/unit/cpp/test_ism330dhcx.cpp
)
target_link_libraries(ism330dhcx_tests PRIVATE
  GTest::gtest
  GTest::gtest_main
  Threads::Threads
)
target_include_directories(ism330dhcx_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/unit/mocks
  ${CMAKE_SOURCE_DIR}/unit/cpp
  ${CMAKE_SOURCE_DIR}/../stm/Core/Inc
)
gtest_discover_tests(ism330dhcx_tests)

add_executable(hts221_tests
  ${MOCK_SOURCES}
  ${CMAKE_SOURCE_DIR}/../stm/Core/Src/hts221.c
  ${CMAKE_SOURCE_DIR}/unit/cpp/test_hts221.cpp
)
target_link_libraries(hts221_tests PRIVATE
  GTest::gtest
  GTest::gtest_main
  Threads::Threads
)
target_include_directories(hts221_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/unit/mocks
  ${CMAKE_SOURCE_DIR}/unit/cpp
  ${CMAKE_SOURCE_DIR}/../stm/Core/Inc
)
gtest_discover_tests(hts221_tests)

add_executable(lps22hh_tests
  ${MOCK_SOURCES}
  ${CMAKE_SOURCE_DIR}/../stm/Core/Src/lps22hh.c
  ${CMAKE_SOURCE_DIR}/unit/cpp/test_lps22hh.cpp
)
target_link_libraries(lps22hh_tests PRIVATE
  GTest::gtest
  GTest::gtest_main
  Threads::Threads
)
target_include_directories(lps22hh_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/unit/mocks
  ${CMAKE_SOURCE_DIR}/unit/cpp
  ${CMAKE_SOURCE_DIR}/../stm/Core/Inc
)
gtest_discover_tests(lps22hh_tests)

add_executable(veml6030_tests
  ${MOCK_SOURCES}
  ${CMAKE_SOURCE_DIR}/../stm/Core/Src/veml6030.c
  ${CMAKE_SOURCE_DIR}/unit/cpp/test_veml6030.cpp
)
target_link_libraries(veml6030_tests PRIVATE
  GTest::gtest
  GTest::gtest_main
  Threads::Threads
)
target_include_directories(veml6030_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/unit/mocks
  ${CMAKE_SOURCE_DIR}/unit/cpp
  ${CMAKE_SOURCE_DIR}/../stm/Core/Inc
)
gtest_discover_tests(veml6030_tests)

# Composite test that compiles all testable C files together
add_executable(all_tests
  ${CMAKE_SOURCE_DIR}/unit/cpp/speedometer_testable.c
  ${CMAKE_SOURCE_DIR}/unit/cpp/motor_control_testable.c
  ${CMAKE_SOURCE_DIR}/unit/cpp/emergency_stop_testable.c
  ${CMAKE_SOURCE_DIR}/unit/cpp/servo_testable.c
  ${CMAKE_SOURCE_DIR}/unit/cpp/tof_testable.c
  ${MOCK_SOURCES}
  ${CMAKE_SOURCE_DIR}/unit/cpp/test_speedometer.cpp
  ${CMAKE_SOURCE_DIR}/unit/cpp/test_motor_control.cpp
  ${CMAKE_SOURCE_DIR}/unit/cpp/test_emergency_stop.cpp
  ${CMAKE_SOURCE_DIR}/unit/cpp/test_servo.cpp
  ${CMAKE_SOURCE_DIR}/unit/cpp/test_tof.cpp
  ${CMAKE_SOURCE_DIR}/unit/cpp/test_integration.cpp
)
target_link_libraries(all_tests PRIVATE
  GTest::gtest
  GTest::gtest_main
  Threads::Threads
)
target_include_directories(all_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/unit/mocks
  ${CMAKE_SOURCE_DIR}/unit/cpp
  ${CMAKE_SOURCE_DIR}/../stm/Core/Inc
)
gtest_discover_tests(all_tests)

# -----------------------
# Qt6
# -----------------------
find_package(Qt6 REQUIRED COMPONENTS Core Test Network)

# -----------------------
# Paths
# This CMakeLists.txt is expected to live in: tests/
# and the repo layout has a sibling folder: ../kuksa
# -----------------------
set(KUKSA_ROOT "${CMAKE_SOURCE_DIR}/../kuksa" CACHE PATH "Path to kuksa repo root")

set(KUKSA_RPI5_SRC "${KUKSA_ROOT}/kuksa_RPi5/src")
set(KUKSA_RPI5_INC "${KUKSA_ROOT}/kuksa_RPi5/inc")

message(STATUS "KUKSA_ROOT     = ${KUKSA_ROOT}")
message(STATUS "KUKSA_RPI5_SRC = ${KUKSA_RPI5_SRC}")
message(STATUS "KUKSA_RPI5_INC = ${KUKSA_RPI5_INC}")

# -----------------------
# Coverage helper (per-target)
# -----------------------
function(enable_coverage target)
  target_compile_options(${target} PRIVATE --coverage -O0 -g)
  target_link_options(${target} PRIVATE --coverage)
endfunction()

# -----------------------
# Production modules under test
# -----------------------
add_library(dispatch_module STATIC
  "${KUKSA_RPI5_SRC}/dispatch_frames.cpp"
)
target_include_directories(dispatch_module PUBLIC
  "${KUKSA_RPI5_INC}"
  "${KUKSA_RPI5_SRC}"
)

add_library(handlers_module STATIC
  "${KUKSA_RPI5_SRC}/handlers/environment.cpp"
  "${KUKSA_RPI5_SRC}/handlers/speed.cpp"
  "${KUKSA_RPI5_SRC}/handlers/battery.cpp"
  "${KUKSA_RPI5_SRC}/handlers/heartbeat_stm.cpp"
  "${KUKSA_RPI5_SRC}/handlers/tof_distance.cpp"
  "${KUKSA_RPI5_SRC}/handlers/imu_accel.cpp"
  "${KUKSA_RPI5_SRC}/handlers/imu_gyro.cpp"
  "${KUKSA_RPI5_SRC}/handlers/emergency_stop.cpp"
  "${KUKSA_RPI5_SRC}/handlers/joystick.cpp"
  "${KUKSA_RPI5_SRC}/is_stm_connected.cpp"
)
target_include_directories(handlers_module PUBLIC
  "${KUKSA_RPI5_INC}"
  "${KUKSA_RPI5_SRC}"
  "${KUKSA_RPI5_SRC}/handlers"
)

# -----------------------
# Backend sources (Qt providers)
# This CMakeLists lives in tests/, so cluster is sibling: ../cluster/...
# -----------------------
set(BACKEND_SOURCES
  "${CMAKE_SOURCE_DIR}/../cluster/src/backend/speedprovider.cpp"
  "${CMAKE_SOURCE_DIR}/../cluster/src/backend/temperatureprovider.cpp"
  "${CMAKE_SOURCE_DIR}/../cluster/src/backend/timeprovider.cpp"
)

# ADDED THIS - DAVID

add_library(backend_module STATIC
  ${BACKEND_SOURCES}
)

target_include_directories(backend_module PUBLIC
  "${CMAKE_SOURCE_DIR}/../cluster/src/backend"
)

target_link_libraries(backend_module PUBLIC
  Qt6::Core
  Qt6::Network
)

# -----------------------
# Test executables
# All tests are in: tests/unit/cpp/
# -----------------------

# 1 Dispatch tests
add_executable(dispatch_tests
  "${CMAKE_SOURCE_DIR}/unit/cpp/dispatcher_test.cpp"
)
target_include_directories(dispatch_tests PRIVATE
  "${KUKSA_RPI5_INC}"
  "${KUKSA_RPI5_SRC}"
  "${CMAKE_SOURCE_DIR}/unit/cpp"
)
target_link_libraries(dispatch_tests PRIVATE
  dispatch_module
  handlers_module
  GTest::gtest
  GTest::gtest_main
)
gtest_discover_tests(dispatch_tests)

# 2 Handler tests (CAN handlers)
add_executable(handlers_tests
  "${CMAKE_SOURCE_DIR}/unit/cpp/can_decode_test.cpp"
  "${CMAKE_SOURCE_DIR}/unit/cpp/environment_test.cpp"
  "${CMAKE_SOURCE_DIR}/unit/cpp/speed_test.cpp"
  "${CMAKE_SOURCE_DIR}/unit/cpp/battery_test.cpp"
  "${CMAKE_SOURCE_DIR}/unit/cpp/heartbeat_stm_test.cpp"
  "${CMAKE_SOURCE_DIR}/unit/cpp/tof_distance_test.cpp"
  "${CMAKE_SOURCE_DIR}/unit/cpp/is_stm_connected_test.cpp"
  "${CMAKE_SOURCE_DIR}/unit/cpp/emergency_stop_test.cpp"
  "${CMAKE_SOURCE_DIR}/unit/cpp/imu_accel_test.cpp"
  "${CMAKE_SOURCE_DIR}/unit/cpp/imu_gyro_test.cpp"
  "${CMAKE_SOURCE_DIR}/unit/cpp/joystick_test.cpp"
)

target_include_directories(handlers_tests PRIVATE
  "${KUKSA_RPI5_INC}"
  "${KUKSA_RPI5_SRC}"
  "${KUKSA_RPI5_SRC}/handlers"
  "${CMAKE_SOURCE_DIR}/unit/cpp"
)
target_link_libraries(handlers_tests PRIVATE
  handlers_module
  GTest::gtest
  GTest::gtest_main
)
gtest_discover_tests(handlers_tests)

# -----------------------------
# Cluster tests
# -----------------------------

# 3 Backend provider tests (Qt)
add_executable(backend_tests
  ${BACKEND_SOURCES}
  "${CMAKE_SOURCE_DIR}/unit/cpp/speedprovider_test.cpp"
  "${CMAKE_SOURCE_DIR}/unit/cpp/temperatureprovider_test.cpp"
  "${CMAKE_SOURCE_DIR}/unit/cpp/timeprovider_test.cpp"
)
target_include_directories(backend_tests PRIVATE
  "${CMAKE_SOURCE_DIR}/../cluster/src/backend"
)
target_link_libraries(backend_tests PRIVATE
  backend_module
  GTest::gtest
  GTest::gtest_main
  Qt6::Core
  Qt6::Test
  Qt6::Network
)
gtest_discover_tests(backend_tests)

# -----------------------
# Coverage (per-target)
# -----------------------
if(ENABLE_COVERAGE)
  enable_coverage(dispatch_module)
  enable_coverage(handlers_module)
  enable_coverage(dispatch_tests)
  enable_coverage(handlers_tests)
  enable_coverage(backend_module)
  enable_coverage(backend_tests)
endif()

