cmake_minimum_required(VERSION 3.16)
project(kuksa_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

# -----------------------
# GoogleTest
# -----------------------
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# -----------------------
# Paths
# -----------------------
set(KUKSA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../kuksa" CACHE PATH "Path to kuksa repo root")

set(KUKSA_RPI5_SRC "${KUKSA_ROOT}/kuksa_RPi5/src")
set(KUKSA_RPI5_INC "${KUKSA_ROOT}/kuksa_RPi5/inc")

message(STATUS "KUKSA_ROOT     = ${KUKSA_ROOT}")
message(STATUS "KUKSA_RPI5_SRC = ${KUKSA_RPI5_SRC}")
message(STATUS "KUKSA_RPI5_INC = ${KUKSA_RPI5_INC}")

# -----------------------
# Coverage (per-target)
# -----------------------
option(ENABLE_COVERAGE "Enable coverage flags" OFF)

function(enable_coverage target)
  target_compile_options(${target} PRIVATE --coverage -O0 -g)
  target_link_options(${target} PRIVATE --coverage)
endfunction()

# -----------------------
# Dispatcher library (real dispatcher implementation)
# no real handler files, the test use spies
# -----------------------
add_library(dispatch_module STATIC
  "${KUKSA_RPI5_SRC}/dispatch_frames.cpp"
)

target_include_directories(dispatch_module PUBLIC
  "${KUKSA_RPI5_INC}"
  "${KUKSA_RPI5_SRC}"
)

# -----------------------
# DISPATCH TESTS
# -----------------------
add_executable(dispatch_tests
  "${CMAKE_CURRENT_SOURCE_DIR}/tests/dispatcher_test.cpp"
)

target_include_directories(dispatch_tests PRIVATE
  "${KUKSA_RPI5_INC}"
  "${KUKSA_RPI5_SRC}"
)

target_link_libraries(dispatch_tests PRIVATE
  dispatch_module
  GTest::gtest
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(dispatch_tests)

# -----------------------
# HANDLER MODULE (real handler implementations)
# -----------------------
add_library(handlers_module STATIC
  "${KUKSA_RPI5_SRC}/handlers/environment.cpp"
  "${KUKSA_RPI5_SRC}/handlers/speed.cpp"
  "${KUKSA_RPI5_SRC}/handlers/battery.cpp"
  "${KUKSA_RPI5_SRC}/handlers/heartbeat_stm.cpp"
  "${KUKSA_RPI5_SRC}/handlers/tof_distance.cpp"
  "${KUKSA_RPI5_SRC}/is_stm_connected.cpp"
)

target_include_directories(handlers_module PUBLIC
  "${KUKSA_RPI5_INC}"
  "${KUKSA_RPI5_SRC}"
  "${KUKSA_RPI5_SRC}/handlers"
)

add_executable(handler_tests
  "${CMAKE_CURRENT_SOURCE_DIR}/tests/can_decode_test.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tests/environment_test.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tests/speed_test.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tests/battery_test.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tests/heartbeat_stm_test.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tests/tof_distance_test.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/tests/is_stm_connected_test.cpp"
)

target_include_directories(handler_tests PRIVATE
  "${KUKSA_RPI5_INC}"
  "${KUKSA_RPI5_SRC}"
  "${KUKSA_RPI5_SRC}/handlers"
)

target_link_libraries(handler_tests PRIVATE
  handlers_module
  GTest::gtest
  GTest::gtest_main
)

gtest_discover_tests(handler_tests)

# -----------------------
# Enable coverage on the targets that matter
# -----------------------
if(ENABLE_COVERAGE)
  enable_coverage(dispatch_module)
  enable_coverage(handlers_module)
  enable_coverage(dispatch_tests)
  enable_coverage(handler_tests)
endif()

