cmake_minimum_required(VERSION 3.14)
project(SEA_ME_Team6_Tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")

enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# include_directories(
#     ${CMAKE_CURRENT_SOURCE_DIR}/../cluster
#     ${CMAKE_CURRENT_SOURCE_DIR}/../CAN
#     ${CMAKE_CURRENT_SOURCE_DIR}/../stm
#     ${CMAKE_CURRENT_SOURCE_DIR}/mocks
#     ${CMAKE_CURRENT_SOURCE_DIR}/fixtures
# )

# add_subdirectory(integration)

add_executable(all_tests
    main.cpp
)

target_link_libraries(all_tests
    gtest
    gtest_main
    gmock
)

add_subdirectory(unit)

add_test(NAME AllTests COMMAND all_tests)

# Coverage report generation (always available)
find_program(LCOV_PATH lcov)
find_program(GENHTML_PATH genhtml)

if(LCOV_PATH AND GENHTML_PATH)
    message(STATUS "lcov found: ${LCOV_PATH}")
    message(STATUS "genhtml found: ${GENHTML_PATH}")
    
    add_custom_target(coverage
        # Clean old coverage data
        COMMAND ${LCOV_PATH} --directory . --zerocounters
        
        # Run tests
        COMMAND ${CMAKE_CTEST_COMMAND} --verbose
        
        # Capture coverage data (with error handling)
        COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage.info 
            --rc geninfo_unexecuted_blocks=1
            --ignore-errors mismatch
            --ignore-errors inconsistent
        
        # Remove unwanted files from coverage (system headers and googletest only)
        COMMAND ${LCOV_PATH} --remove coverage.info 
            '/usr/*' 
            '*/_deps/*'
            '*/googletest/*'
            --output-file coverage.info.cleaned
            --ignore-errors unused
        
        # Generate HTML report
        COMMAND ${GENHTML_PATH} -o coverage coverage.info.cleaned
            --ignore-errors unmapped
        
        # Print summary
        COMMAND ${LCOV_PATH} --list coverage.info.cleaned
        
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating code coverage report..."
    )
    
    add_custom_command(TARGET coverage POST_BUILD
        COMMAND ;
        COMMENT "Open ${CMAKE_BINARY_DIR}/coverage/index.html in your browser to view the coverage report."
    )
else()
    if(NOT LCOV_PATH)
        message(WARNING "lcov not found! Coverage target will not be available.")
        message(WARNING "Install with: sudo apt-get install lcov")
    endif()
    if(NOT GENHTML_PATH)
        message(WARNING "genhtml not found! Coverage target will not be available.")
    endif()
endif()