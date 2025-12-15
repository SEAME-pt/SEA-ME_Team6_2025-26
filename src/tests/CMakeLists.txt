cmake_minimum_required(VERSION 3.14)
project(SEA_ME_Team6_Tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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