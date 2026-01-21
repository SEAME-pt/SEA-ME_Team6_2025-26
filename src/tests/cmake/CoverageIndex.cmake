set(UNIFIED_COVERAGE_DIR ${CMAKE_SOURCE_DIR}/coverage)

find_program(BASH_EXECUTABLE bash)
if(NOT BASH_EXECUTABLE)
    message(FATAL_ERROR "bash not found")
endif()

add_custom_target(coverage
    COMMAND ${BASH_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/utils/scripts/generate_index.sh ${UNIFIED_COVERAGE_DIR}
    COMMENT "Generating unified coverage index"
)

add_dependencies(coverage cpp_coverage)
if(ENABLE_PYTHON_TESTS)
    add_dependencies(coverage python_coverage)
endif()