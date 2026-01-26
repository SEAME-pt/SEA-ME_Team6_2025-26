# ===========================================================================
# AGL SDK Toolchain for Cross-Compilation
# Inclui: Toolchain ARM + Qt6 + gRPC/Protobuf
# Projeto: SEA:ME
# ===========================================================================

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# ---------------------------------------------------------------------------
# SDK Paths
# ---------------------------------------------------------------------------
set(AGL_SDK_PATH "/opt/agl-sdk")
set(AGL_SYSROOT "${AGL_SDK_PATH}/sysroots/aarch64-agl-linux")
set(AGL_HOST_TOOLS "${AGL_SDK_PATH}/sysroots/x86_64-aglsdk-linux")

# ---------------------------------------------------------------------------
# Sysroot & Compilers
# ---------------------------------------------------------------------------
set(CMAKE_SYSROOT ${AGL_SYSROOT})
set(CMAKE_FIND_ROOT_PATH ${AGL_SYSROOT})

set(CMAKE_C_COMPILER ${AGL_HOST_TOOLS}/usr/bin/aarch64-agl-linux/aarch64-agl-linux-gcc)
set(CMAKE_CXX_COMPILER ${AGL_HOST_TOOLS}/usr/bin/aarch64-agl-linux/aarch64-agl-linux-g++)

# ---------------------------------------------------------------------------
# Compiler/Linker Flags
# ---------------------------------------------------------------------------
set(CMAKE_C_FLAGS_INIT "--sysroot=${AGL_SYSROOT}")
set(CMAKE_CXX_FLAGS_INIT "--sysroot=${AGL_SYSROOT}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "--sysroot=${AGL_SYSROOT}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "--sysroot=${AGL_SYSROOT}")

# ---------------------------------------------------------------------------
# Search Paths
# ---------------------------------------------------------------------------
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# ---------------------------------------------------------------------------
# Qt6 Configuration
# ---------------------------------------------------------------------------
set(QT_HOST_PATH "${AGL_HOST_TOOLS}/usr")
set(QT_HOST_PATH_CMAKE_DIR "${AGL_HOST_TOOLS}/usr/lib/cmake")

# Qt6 módulos principais
set(Qt6_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6")
set(Qt6Core_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6Core")
set(Qt6CoreTools_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6Core")
set(Qt6Gui_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6Gui")
set(Qt6Widgets_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6Widgets")
set(Qt6Qml_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6Qml")
set(Qt6Quick_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6Quick")
set(Qt6Network_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6Network")
set(Qt6HostInfo_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6HostInfo")

# Qt6 módulos adicionais
set(Qt6SerialPort_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6SerialPort")
set(Qt6Mqtt_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6Mqtt")
set(Qt6Multimedia_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6Multimedia")
set(Qt6WebSockets_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6WebSockets")
set(Qt6WebChannel_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6WebChannel")
set(Qt6Sensors_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6Sensors")
set(Qt6Svg_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6Svg")
set(Qt6Charts_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6Charts")
set(Qt63D_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt63D")
set(Qt6Positioning_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6Positioning")
set(Qt6Location_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6Location")
set(Qt6Connectivity_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6Connectivity")
set(Qt6NetworkAuth_DIR "${AGL_SYSROOT}/usr/lib/cmake/Qt6NetworkAuth")

# ---------------------------------------------------------------------------
# Protobuf / gRPC
# ---------------------------------------------------------------------------
set(Protobuf_PROTOC_EXECUTABLE "${AGL_HOST_TOOLS}/usr/bin/protoc")
set(gRPC_CPP_PLUGIN_EXECUTABLE "${AGL_HOST_TOOLS}/usr/bin/grpc_cpp_plugin")

# ---------------------------------------------------------------------------
# pkg-config
# ---------------------------------------------------------------------------
set(ENV{PKG_CONFIG_PATH} "${AGL_SYSROOT}/usr/lib/pkgconfig:${AGL_SYSROOT}/usr/share/pkgconfig")
set(ENV{PKG_CONFIG_SYSROOT_DIR} "${AGL_SYSROOT}")