cmake_minimum_required(VERSION 3.25)
include_guard(GLOBAL)

# Set the system name and processor for cross-compilation
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Set the target sysroot and architecture
set(TARGET_SYSROOT /build/sysroot)
set(TARGET_ARCHITECTURE aarch64-linux-gnu)
set(CMAKE_SYSROOT ${TARGET_SYSROOT})

# Configure the pkg-config environment variables
set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${CMAKE_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}/pkgconfig:${CMAKE_SYSROOT}/usr/lib/pkgconfig")
set(ENV{PKG_CONFIG_LIBDIR} "/usr/lib/pkgconfig:/usr/share/pkgconfig:${CMAKE_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}/pkgconfig:${CMAKE_SYSROOT}/usr/lib/pkgconfig")
set(ENV{PKG_CONFIG_SYSROOT_DIR} "${CMAKE_SYSROOT}")

# Set the C and C++ compilers
set(CMAKE_C_COMPILER /usr/bin/${TARGET_ARCHITECTURE}-gcc-12)
set(CMAKE_CXX_COMPILER /usr/bin/${TARGET_ARCHITECTURE}-g++-12)

# Define additional compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -isystem=/usr/include -isystem=/usr/local/include -isystem=/usr/include/${TARGET_ARCHITECTURE}")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")

# Set Qt-specific compiler and linker flags
set(QT_COMPILER_FLAGS "-march=armv8-a -mtune=generic -ftree-vectorize")
set(QT_COMPILER_FLAGS_RELEASE "-O2 -pipe")
set(QT_LINKER_FLAGS "-Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed -Wl,-rpath-link=${TARGET_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE} -Wl,-rpath-link=${TARGET_SYSROOT}/usr/lib -Wl,-rpath-link=${TARGET_SYSROOT}/lib")

# Configure CMake find root path modes
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Set the install RPATH use and build RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_RPATH ${TARGET_SYSROOT})

# Initialize CMake configuration variables with the specified flags
set(CMAKE_C_FLAGS_INIT "${QT_COMPILER_FLAGS} -march=armv8-a")
set(CMAKE_CXX_FLAGS_INIT "${QT_COMPILER_FLAGS} -march=armv8-a")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${QT_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${QT_LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "${QT_LINKER_FLAGS}")

# Set paths for XCB and OpenGL libraries
set(XCB_PATH_VARIABLE ${TARGET_SYSROOT})
set(GL_INC_DIR ${TARGET_SYSROOT}/usr/include)

# IMPORTANT: Your libraries are in /usr/lib, NOT /usr/lib/aarch64-linux-gnu
# So we need to check both locations
set(GL_LIB_DIR ${TARGET_SYSROOT}:${TARGET_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}:${TARGET_SYSROOT}/usr/lib:${TARGET_SYSROOT}/lib:${TARGET_SYSROOT}/usr)

# OpenGL ES / EGL configuration - check both possible locations
if(EXISTS "${TARGET_SYSROOT}/usr/lib/libEGL.so")
    set(EGL_LIBRARY ${TARGET_SYSROOT}/usr/lib/libEGL.so)
elseif(EXISTS "${TARGET_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}/libEGL.so")
    set(EGL_LIBRARY ${TARGET_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}/libEGL.so)
endif()

if(EXISTS "${TARGET_SYSROOT}/usr/lib/libGLESv2.so")
    set(GLESv2_LIBRARY ${TARGET_SYSROOT}/usr/lib/libGLESv2.so)
elseif(EXISTS "${TARGET_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}/libGLESv2.so")
    set(GLESv2_LIBRARY ${TARGET_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}/libGLESv2.so)
endif()

set(EGL_INCLUDE_DIR ${GL_INC_DIR})
set(OPENGL_INCLUDE_DIR ${GL_INC_DIR})
set(GLESv2_INCLUDE_DIR ${GL_INC_DIR})

# Set OpenGL library (might be in either location)
if(EXISTS "${TARGET_SYSROOT}/usr/lib/libOpenGL.so")
    set(OPENGL_opengl_LIBRARY ${TARGET_SYSROOT}/usr/lib/libOpenGL.so)
elseif(EXISTS "${TARGET_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}/libOpenGL.so")
    set(OPENGL_opengl_LIBRARY ${TARGET_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}/libOpenGL.so)
endif()

# GBM and DRM libraries
if(EXISTS "${TARGET_SYSROOT}/usr/lib/libgbm.so")
    set(gbm_LIBRARY ${TARGET_SYSROOT}/usr/lib/libgbm.so)
elseif(EXISTS "${TARGET_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}/libgbm.so")
    set(gbm_LIBRARY ${TARGET_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}/libgbm.so)
endif()

if(EXISTS "${TARGET_SYSROOT}/usr/lib/libdrm.so")
    set(Libdrm_LIBRARY ${TARGET_SYSROOT}/usr/lib/libdrm.so)
elseif(EXISTS "${TARGET_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}/libdrm.so")
    set(Libdrm_LIBRARY ${TARGET_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}/libdrm.so)
endif()

set(gbm_INCLUDE_DIR ${GL_INC_DIR})
set(Libdrm_INCLUDE_DIR ${GL_INC_DIR})

# XCB library
if(EXISTS "${TARGET_SYSROOT}/usr/lib/libxcb.so")
    set(XCB_XCB_LIBRARY ${TARGET_SYSROOT}/usr/lib/libxcb.so)
elseif(EXISTS "${TARGET_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}/libxcb.so")
    set(XCB_XCB_LIBRARY ${TARGET_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}/libxcb.so)
endif()

set(XCB_XCB_INCLUDE_DIR ${GL_INC_DIR})

# Append to CMake library and prefix paths - INCLUDE BOTH LOCATIONS
list(APPEND CMAKE_LIBRARY_PATH 
    ${CMAKE_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}
    ${CMAKE_SYSROOT}/usr/lib
    ${CMAKE_SYSROOT}/lib
)

list(APPEND CMAKE_PREFIX_PATH 
    "/usr/lib/${TARGET_ARCHITECTURE}/cmake"
    "${CMAKE_SYSROOT}/usr/lib/cmake"
    "${CMAKE_SYSROOT}/usr"
)

# Debug output (optional - remove in production)
message(STATUS "=== Toolchain Configuration ===")
message(STATUS "GLESv2_LIBRARY: ${GLESv2_LIBRARY}")
message(STATUS "EGL_LIBRARY: ${EGL_LIBRARY}")
message(STATUS "CMAKE_LIBRARY_PATH: ${CMAKE_LIBRARY_PATH}")