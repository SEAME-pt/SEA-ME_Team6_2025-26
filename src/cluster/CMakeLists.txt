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
# QT6 CROSS-COMPILATION SETTINGS
# ============================================================================
# Disable OpenGL for embedded systems
set(QT_FEATURE_opengles2 OFF)
set(QT_FEATURE_opengl OFF)

# Skip Qt6 device checks (fixes Qt6HostInfo missing error)
set(QT_NO_PACKAGE_CONFIG_DEVICE_CHECKS ON)

# Find Qt6 - require version 6.7 or later
find_package(Qt6 6.7 REQUIRED COMPONENTS 
    Core 
    Gui 
    Qml 
    Quick 
    QuickControls2 
    DBus
)

# ============================================================================
# COMPILER FLAGS FOR CROSS-COMPILATION
# ============================================================================
# Use CMAKE_SYSROOT if available (set by toolchain)
if(CMAKE_SYSROOT)
    set(SYSROOT_LIB_PATH "${CMAKE_SYSROOT}/usr/lib")
    
    # Add rpath-link for linker to find dependencies
    string(APPEND CMAKE_C_FLAGS " -fPIC")
    string(APPEND CMAKE_CXX_FLAGS " -fPIC")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -Wl,-rpath-link,${SYSROOT_LIB_PATH}")
else()
    message(STATUS "Not cross-compiling or CMAKE_SYSROOT not set")
endif()

# ============================================================================
# SOURCE FILES
# ============================================================================
add_executable(HelloQt6Qml
    src/main.cpp
    src/backend/timeprovider.cpp
    src/backend/systemstatus.cpp
    src/backend/speedprovider.cpp
    src/backend/temperatureprovider.cpp
    src/backend/ipcclient.cpp
    src/backend/reader.cpp
    qml.qrc
)

# ============================================================================
# LINK LIBRARIES
# ============================================================================
target_link_libraries(HelloQt6Qml
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
)

# ============================================================================
# INCLUDE DIRECTORIES (if needed)
# ============================================================================
target_include_directories(HelloQt6Qml PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/backend
)