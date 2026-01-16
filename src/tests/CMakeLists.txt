cmake_minimum_required(VERSION 3.10)
project(SEA_ME_Team6_Tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

option(ENABLE_CPP_TESTS "Enable coverage reporting" ON)
option(ENABLE_PYTHON_TESTS "Enable Python Unit Tests" ON)

# Coverage Flags
# -----------------------------------------
if(ENABLE_CPP_TESTS)
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

# Testing
# -----------------------------------------
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
# Add here Qt6 Packages as they appear
find_package(Qt6 REQUIRED COMPONENTS Core Test Network)

include_directories(../cluster/src/backend)

file(GLOB BACKEND_SOURCES ../cluster/src/backend/*.cpp)

add_executable(SEA_ME_Team6_Tests
${BACKEND_SOURCES}
unit/cpp/speedprovider_test.cpp
unit/cpp/temperatureprovider_test.cpp
unit/cpp/timeprovider_test.cpp
)

# Add here Qt6 Packages as they appear
target_link_libraries(SEA_ME_Team6_Tests
    gtest
    gtest_main
    pthread
    gcov
    Qt6::Core
    Qt6::Test
    Qt6::Network
)

add_test(NAME cpp_unit_test COMMAND SEA_ME_Team6_Tests)

# CMake Modules
# -----------------------------------------
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if (ENABLE_CPP_TESTS)
    include(CoverageCpp)
    if (ENABLE_PYTHON_TESTS)
        include(CoveragePython)
    endif()
    include(CoverageIndex)
endif()
