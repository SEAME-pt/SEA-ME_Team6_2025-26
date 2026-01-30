cmake_minimum_required(VERSION 3.10)
project(SEA_ME_Team6_Tests LANGUAGES CXX)

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

