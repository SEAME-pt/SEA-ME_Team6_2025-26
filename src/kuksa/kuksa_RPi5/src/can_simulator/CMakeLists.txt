cmake_minimum_required(VERSION 3.16)
project(can_emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(can_emulator
  src/main.cpp
  src/can_sender.cpp
  src/helpers.cpp
  src/scenario_loader.cpp
  src/frames_builder.cpp
  src/cli.cpp
)

target_include_directories(can_emulator PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/inc
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../inc
)

find_package(Threads REQUIRED)
target_link_libraries(can_emulator PRIVATE Threads::Threads)

target_compile_options(can_emulator PRIVATE -Wall -Wextra -Wpedantic)