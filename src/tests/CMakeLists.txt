cmake_minimum_required(VERSION 3.10)
project(SEA_ME_Team6_Tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

option(ENABLE_COVERAGE "Enable coverage reporting" ON)

if(ENABLE_COVERAGE)
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

# GTEST
# ./SEA_ME_Team6_Tests

enable_testing()

find_package(GTest REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Test)

include_directories(../cluster/src/backend)

file(GLOB BACKEND_SOURCES ../cluster/src/backend/*.cpp)

add_executable(SEA_ME_Team6_Tests
    ${BACKEND_SOURCES}
    unit/speedprovider_test.cpp
)

target_link_libraries(SEA_ME_Team6_Tests
    gtest
    gtest_main
    pthread
    gcov
    Qt6::Core
    Qt6::Test
)

add_test(NAME speedprovider_test COMMAND SEA_ME_Team6_Tests)

# COVERAGE REPORT
# make coverage

if(ENABLE_COVERAGE)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)

    if(NOT LCOV_PATH OR NOT GENHTML_PATH)
        message(FATAL_ERROR "lcov and/or genhtml not found")
    endif()

    add_custom_target(coverage
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        COMMAND ${LCOV_PATH} --capture
            --directory .
            --output-file coverage.info
            --ignore-errors mismatch
            --rc geninfo_unexecuted_blocks=1

        COMMAND ${LCOV_PATH} --remove coverage.info
            '/usr/*'
            '*/tests/*'
            '*/build/*_autogen/*'
            --output-file coverage.filtered.info
            --ignore-errors unused

        COMMAND ${GENHTML_PATH} coverage.filtered.info
                "--output-directory=coverage_report"
                "--title=SEA:ME Team 6 – Coverage Report"
                "--legend"
                "--precision=2"

        COMMAND xdg-open coverage_report/index.html || true
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running tests and generating coverage report"
    )


    add_dependencies(coverage SEA_ME_Team6_Tests)
endif()