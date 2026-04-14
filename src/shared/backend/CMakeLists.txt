cmake_minimum_required(VERSION 3.20)
project(Backend LANGUAGES CXX)

# ============================================================================
# CMAKE CONFIGURATION
# ============================================================================
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# DETECT BUILD ENVIRONMENT
# ============================================================================
if(CMAKE_CROSSCOMPILING)
    message(STATUS "[Backend] Cross-compiling for ARM (AGL SDK)")
    set(IS_CROSS_COMPILE ON)
else()
    message(STATUS "[Backend] Native x86_64 development build")
    set(IS_CROSS_COMPILE OFF)
endif()

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
    DBus
    Multimedia
)

# ============================================================================
# GRPC/PROTOBUF CONFIGURATION
# ============================================================================
if(NOT IS_CROSS_COMPILE)
    find_package(Protobuf CONFIG REQUIRED)
    find_package(gRPC CONFIG REQUIRED)
    message(STATUS "[Backend] Protobuf version: ${Protobuf_VERSION}")
    message(STATUS "[Backend] gRPC version: ${gRPC_VERSION}")
else()
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(PROTOBUF QUIET protobuf)
        pkg_check_modules(GRPC QUIET grpc++)
    endif()
    if(NOT PROTOBUF_FOUND)
        message(STATUS "[Backend] Using manual library specification for Protobuf/gRPC")
    endif()
endif()

# ============================================================================
# SHARED BACKEND LIBRARY
# ============================================================================
add_library(Backend STATIC
    # Core components
    core/reader.cpp
    core/signalrouter.cpp
    core/baseprovider.cpp
    
    # Providers
    providers/adasprovider.cpp
    providers/exteriorprovider.cpp
    providers/powertrainprovider.cpp
    providers/vehicleprovider.cpp
    providers/currentlocationprovider.cpp
    providers/chassisprovider.cpp
    providers/extraprovider.cpp
    providers/cameraprovider.cpp
    
    # Kuksa generated protobuf/gRPC stubs
    kuksa/val.pb.cc
    kuksa/val.grpc.pb.cc
    kuksa/types.pb.cc
    kuksa/types.grpc.pb.cc
)

# ============================================================================
# INCLUDE DIRECTORIES (Public - visible to cluster/hmi)
# ============================================================================
target_include_directories(Backend PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/providers
)

# ============================================================================
# LINK LIBRARIES - QT6
# ============================================================================
target_link_libraries(Backend PUBLIC
    Qt6::Core
    Qt6::DBus
    Qt6::Multimedia
)

# ============================================================================
# LINK LIBRARIES - GRPC/PROTOBUF
# ============================================================================
if(IS_CROSS_COMPILE)
    target_link_libraries(Backend PUBLIC
        grpc++
        grpc
        gpr
        protobuf
        upb
        re2
        utf8_range_lib
        # Abseil libraries
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
else()
    target_link_libraries(Backend PUBLIC
        gRPC::grpc++
        protobuf::libprotobuf
    )
endif()

# ============================================================================
# BUILD SUMMARY
# ============================================================================
message(STATUS "========================================")
message(STATUS "[Backend] Configuration Summary")
message(STATUS "========================================")
message(STATUS "Cross-compiling: ${IS_CROSS_COMPILE}")
message(STATUS "Qt6 Version: ${Qt6_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "========================================")