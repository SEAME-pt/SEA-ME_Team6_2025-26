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
# GRPC/PROTOBUF CONFIGURATION - BYPASS BROKEN CMAKE CONFIGS
# ============================================================================
# The AGL SDK has broken CMake config files for utf8_range that look for
# static libraries (.a) but only shared libraries (.so) are installed.
# We'll use pkg-config and manual library specification instead.

# Try to find PkgConfig
find_package(PkgConfig)

if(PKG_CONFIG_FOUND)
    # Use pkg-config to find protobuf
    pkg_check_modules(PROTOBUF QUIET protobuf)
    pkg_check_modules(GRPC QUIET grpc++)
    pkg_check_modules(UTF8_RANGE QUIET utf8_range)
endif()

# If pkg-config didn't work, we'll manually specify libraries below
if(NOT PROTOBUF_FOUND)
    message(STATUS "Using manual library specification for Protobuf/gRPC")
endif()

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
    
    # Generated Protobuf/gRPC stubs
    src/backend/kuksa/val.pb.cc
    src/backend/kuksa/val.grpc.pb.cc
    src/backend/kuksa/types.pb.cc
    src/backend/kuksa/types.grpc.pb.cc
    
    qml.qrc
)

# ============================================================================
# INCLUDE DIRECTORIES
# ============================================================================
target_include_directories(HelloQt6Qml PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/backend
)

# Add protobuf include directories if found via pkg-config
if(PROTOBUF_FOUND)
    target_include_directories(HelloQt6Qml PRIVATE ${PROTOBUF_INCLUDE_DIRS})
endif()

# ============================================================================
# LINK LIBRARIES
# ============================================================================
target_link_libraries(HelloQt6Qml PRIVATE
    # Qt6 libraries
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::DBus
)

# Link gRPC/Protobuf libraries manually
# We specify them directly instead of using broken CMake configs
target_link_libraries(HelloQt6Qml PRIVATE
    # Core gRPC/Protobuf libraries (order matters!)
    grpc++
    grpc
    gpr
    protobuf
    
    # Additional gRPC dependencies
    upb
    re2
    
    # utf8_range - use the actual library name that exists
    utf8_range_lib
    
    # Abseil libraries (required by gRPC/Protobuf)
    # Complete list to avoid missing dependency errors
    absl_bad_any_cast_impl
    absl_bad_optional_access
    absl_bad_variant_access
    absl_base
    absl_city
    absl_civil_time
    absl_cord
    absl_cord_internal
    absl_cordz_functions
    absl_cordz_handle
    absl_cordz_info
    absl_cordz_sample_token
    absl_crc32c
    absl_crc_cord_state
    absl_crc_cpu_detect
    absl_crc_internal
    absl_debugging_internal
    absl_demangle_internal
    absl_die_if_null
    absl_examine_stack
    absl_exponential_biased
    absl_failure_signal_handler
    absl_flags_commandlineflag
    absl_flags_commandlineflag_internal
    absl_flags_config
    absl_flags_internal
    absl_flags_marshalling
    absl_flags_parse
    absl_flags_private_handle_accessor
    absl_flags_program_name
    absl_flags_reflection
    absl_flags_usage
    absl_flags_usage_internal
    absl_graphcycles_internal
    absl_hash
    absl_hashtablez_sampler
    absl_int128
    absl_kernel_timeout_internal
    absl_leak_check
    absl_log_entry
    absl_log_flags
    absl_log_globals
    absl_log_initialize
    absl_log_internal_check_op
    absl_log_internal_conditions
    absl_log_internal_fnmatch
    absl_log_internal_format
    absl_log_internal_globals
    absl_log_internal_log_sink_set
    absl_log_internal_message
    absl_log_internal_nullguard
    absl_log_internal_proto
    absl_log_severity
    absl_log_sink
    absl_low_level_hash
    absl_malloc_internal
    absl_periodic_sampler
    absl_random_distributions
    absl_random_internal_distribution_test_util
    absl_random_internal_platform
    absl_random_internal_pool_urbg
    absl_random_internal_randen
    absl_random_internal_randen_hwaes
    absl_random_internal_randen_hwaes_impl
    absl_random_internal_randen_slow
    absl_random_internal_seed_material
    absl_random_seed_gen_exception
    absl_random_seed_sequences
    absl_raw_hash_set
    absl_raw_logging_internal
    absl_scoped_set_env
    absl_spinlock_wait
    absl_stacktrace
    absl_status
    absl_statusor
    absl_str_format_internal
    absl_strerror
    absl_string_view
    absl_strings
    absl_strings_internal
    absl_symbolize
    absl_synchronization
    absl_throw_delegate
    absl_time
    absl_time_zone
    absl_vlog_config_internal
)

# ============================================================================
# ADDITIONAL LINKER FLAGS
# ============================================================================
# Ensure the linker can find all shared library dependencies
if(CMAKE_SYSROOT)
    target_link_options(HelloQt6Qml PRIVATE
        -Wl,-rpath-link,${CMAKE_SYSROOT}/usr/lib
        -Wl,--as-needed
    )
endif()

message(STATUS "========================================")
message(STATUS "Build Configuration Summary")
message(STATUS "========================================")
message(STATUS "Qt6 Version: ${Qt6_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Sysroot: ${CMAKE_SYSROOT}")
message(STATUS "========================================")
