cmake_minimum_required(VERSION 3.20)
project(ClusterApp LANGUAGES CXX)

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
    message(STATUS "[Cluster] Cross-compiling for ARM (AGL SDK)")
    set(IS_CROSS_COMPILE ON)
else()
    message(STATUS "[Cluster] Native x86_64 development build")
    set(IS_CROSS_COMPILE OFF)
endif()

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

# ============================================================================
# QT6 CONFIGURATION
# ============================================================================
if(IS_CROSS_COMPILE)
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

message(STATUS "[Cluster] Using shared backend from: ../shared/backend")

# ============================================================================
# CLUSTER APPLICATION EXECUTABLE
# ============================================================================
add_executable(ClusterApp
    # Main entry point
    main.cpp
    
    # Legacy backend (keep for now - TimeProvider, SystemStatus)
    # src/backend/timeprovider.cpp
    # src/backend/systemstatus.cpp
    
    # QML resources
    qml.qrc
)

# ============================================================================
# INCLUDE DIRECTORIES
# ============================================================================
# target_include_directories(ClusterApp PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/src
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/backend
# )

# ============================================================================
# LINK LIBRARIES - QT6
# ============================================================================
target_link_libraries(ClusterApp PRIVATE
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
target_link_libraries(ClusterApp PRIVATE
    Backend
)

# ============================================================================
# CROSS-COMPILATION LINKER FLAGS
# ============================================================================
if(IS_CROSS_COMPILE AND CMAKE_SYSROOT)
    target_link_options(ClusterApp PRIVATE
        -Wl,-rpath-link,${CMAKE_SYSROOT}/usr/lib
        -Wl,--as-needed
    )
endif()

# ============================================================================
# BUILD SUMMARY
# ============================================================================
message(STATUS "========================================")
message(STATUS "[Cluster] Build Configuration Summary")
message(STATUS "========================================")
message(STATUS "Cross-compiling: ${IS_CROSS_COMPILE}")
message(STATUS "Qt6 Version: ${Qt6_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
if(CMAKE_SYSROOT)
    message(STATUS "Sysroot: ${CMAKE_SYSROOT}")
endif()
message(STATUS "Linking to: Backend")
message(STATUS "========================================")