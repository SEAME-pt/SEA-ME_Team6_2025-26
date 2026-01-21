find_program(BASH_EXECUTABLE bash)
if(NOT BASH_EXECUTABLE)
    message(FATAL_ERROR "bash not found")
endif()

set(PYTHON_TEST_DIR ${CMAKE_SOURCE_DIR}/unit/python)
set(PYTHON_COVERAGE_DIR ${CMAKE_SOURCE_DIR}/coverage/python)

add_custom_target(python_coverage
    COMMAND ${BASH_EXECUTABLE} 
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/scripts/run_python_coverage.sh
        ${PYTHON_TEST_DIR}
        ${PYTHON_COVERAGE_DIR}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating Python Coverage Report"
)