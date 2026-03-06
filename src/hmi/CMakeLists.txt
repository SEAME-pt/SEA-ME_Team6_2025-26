cmake_minimum_required(VERSION 3.20)
project(HelloQt6Qml LANGUAGES CXX)

# ============================================================================
# CMAKE CONFIGURATION
# ============================================================================
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# DETECT BUILD ENVIRONMENT
# ============================================================================
if(CMAKE_CROSSCOMPILING)
    message(STATUS "Cross-compiling for ARM (AGL SDK)")
    set(IS_CROSS_COMPILE ON)
else()
    message(STATUS "Native x86_64 development build")
    set(IS_CROSS_COMPILE OFF)
endif()

# ============================================================================
# SANITIZER CONFIGURATION
# ============================================================================
# Options: "none", "address", "thread", "undefined"
# NOTE: ASAN and TSAN cannot be used together!
# - Use "address"   to catch memory leaks, use-after-free, buffer overflows
# - Use "thread"    to catch data races (the QMutex race in reader.cpp)
# - Use "undefined" to catch UB (integer overflow in gauge bindings!)
set(SANITIZER "address" CACHE STRING "Sanitizer to use: none, address, thread, undefined")

set(CMAKE_BUILD_TYPE Debug)

if(NOT IS_CROSS_COMPILE)
    if(SANITIZER STREQUAL "address")
        message(STATUS "[HMI] AddressSanitizer ENABLED (catches leaks + memory errors)")
        set(SANITIZER_FLAGS "-fsanitize=address -fno-omit-frame-pointer -g -O1")
        # ASAN options - set via env var when running:
        # ASAN_OPTIONS=detect_leaks=1:halt_on_error=0:log_path=/tmp/asan.log ./HMIApp
    elseif(SANITIZER STREQUAL "thread")
        message(STATUS "[HMI] ThreadSanitizer ENABLED (catches data races in reader/router)")
        set(SANITIZER_FLAGS "-fsanitize=thread -fno-omit-frame-pointer -g -O1")
        # TSAN options:
        # TSAN_OPTIONS=halt_on_error=0:log_path=/tmp/tsan.log ./HMIApp
    elseif(SANITIZER STREQUAL "undefined")
        message(STATUS "[HMI] UBSanitizer ENABLED (catches integer overflow in gauge math)")
        set(SANITIZER_FLAGS "-fsanitize=undefined -fno-omit-frame-pointer -g")
        # UBSAN options:
        # UBSAN_OPTIONS=halt_on_error=0:print_stacktrace=1 ./HMIApp
    else()
        message(STATUS "[HMI] No sanitizer")
        set(SANITIZER_FLAGS "")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZER_FLAGS}")
else()
    message(STATUS "[HMI] Sanitizers disabled for cross-compile")
endif()

# ============================================================================
# QT6 CONFIGURATION
# ============================================================================
if(IS_CROSS_COMPILE)
    # Cross-compilation settings
    set(QT_FEATURE_opengles2 OFF)
    set(QT_FEATURE_opengl OFF)
    set(QT_NO_PACKAGE_CONFIG_DEVICE_CHECKS ON)
endif()

find_package(Qt6 6.7 REQUIRED COMPONENTS 
    Core 
    Gui 
    Qml 
    Quick 
    QuickControls2 
    DBus
    Svg
)

# ============================================================================
# ADD SHARED BACKEND LIBRARY
# ============================================================================
# Add the shared backend subdirectory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../shared/backend 
                 ${CMAKE_CURRENT_BINARY_DIR}/shared_backend)

message(STATUS "[HMI] Using shared backend from: ../shared/backend")

# ============================================================================
# HMI APPLICATION EXECUTABLE
# ============================================================================
add_executable(HMIApp
    # Main entry point
    main.cpp
    # QML resources
    qml.qrc
)

# ============================================================================
# LINK LIBRARIES - QT6
# ============================================================================
target_link_libraries(HMIApp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
    Qt6::Svg
)

# ============================================================================
# LINK SHARED BACKEND LIBRARY
# ============================================================================
target_link_libraries(HMIApp PRIVATE
    Backend
)

# ============================================================================
# CROSS-COMPILATION LINKER FLAGS
# ============================================================================
if(IS_CROSS_COMPILE AND CMAKE_SYSROOT)
    target_link_options(HMIApp PRIVATE
        -Wl,-rpath-link,${CMAKE_SYSROOT}/usr/lib
        -Wl,--as-needed
    )
endif()


# ============================================================================
# BUILD SUMMARY
# ============================================================================
message(STATUS "========================================")
message(STATUS "[HMI] Build Configuration Summary")
message(STATUS "========================================")
message(STATUS "Cross-compiling: ${IS_CROSS_COMPILE}")
message(STATUS "Qt6 Version: ${Qt6_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Sanitizer:        ${SANITIZER}")
message(STATUS "Sanitizer Flags:  ${SANITIZER_FLAGS}")
if(CMAKE_SYSROOT)
    message(STATUS "Sysroot: ${CMAKE_SYSROOT}")
endif()
message(STATUS "Linking to: Backend")
message(STATUS "========================================")

# cmake -DSANITIZER=address .. && make && ASAN_OPTIONS=detect_leaks=1:halt_on_error=0:print_stacktrace=1:log_path=/tmp/asan.log ./HMIApp 2>&1 | tee /tmp/hmi_run.log
