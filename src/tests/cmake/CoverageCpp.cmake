find_program(LCOV_PATH lcov)
find_program(GENHTML_PATH genhtml)

if(NOT LCOV_PATH)
    message(FATAL_ERROR "lcov not found")
endif()

if(NOT GENHTML_PATH)
    message(FATAL_ERROR "genhtml not found")
endif()

set(CPP_COVERAGE_DIR ${CMAKE_SOURCE_DIR}/coverage/cpp)

add_custom_target(cpp_coverage
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure

    COMMAND ${LCOV_PATH} --capture
        --directory .
        --output-file coverage_cpp.info
        --ignore-errors mismatch
        --rc geninfo_unexecuted_blocks=1

    COMMAND ${LCOV_PATH} --remove coverage_cpp.info
        '/usr/*'
        '*/tests/unit/cpp/*'
        '*/build/*_autogen/*'
        --output-file coverage_cpp.filtered.info
        --ignore-errors unused

    COMMAND ${GENHTML_PATH} coverage_cpp.filtered.info
        --output-directory ${CPP_COVERAGE_DIR}
        --title "SEA:ME Team 6 - C++ Coverage Report"
        --legend
        --precision=2

    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating C++ Coverage Report"
)

add_dependencies(cpp_coverage SEA_ME_Team6_Tests)