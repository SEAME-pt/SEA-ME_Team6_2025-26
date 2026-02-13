#!/bin/bash
# ===========================================================================
# qt6-build.sh - Helper to compile the Qt cluster
# Use: qt6-build.sh [directory] [build-type]
# ===========================================================================

set -e

source /opt/agl-sdk/environment-setup-aarch64-agl-linux

PROJECT_DIR="${1:-.}"
BUILD_TYPE="${2:-Release}"

# Convert project dir to absolute path
PROJECT_DIR="$(realpath "${PROJECT_DIR}")"
BUILD_DIR="${PROJECT_DIR}/build-arm"

echo "========================================"
echo "Cross-compiling Qt6 project for ARM"
echo "Project: ${PROJECT_DIR}"
echo "Build type: ${BUILD_TYPE}"
echo "Output: ${BUILD_DIR}"
echo "========================================"
echo ""

mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

cmake "${PROJECT_DIR}" \
    -DCMAKE_TOOLCHAIN_FILE=/opt/agl-toolchain.cmake \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -GNinja

ninja -j$(nproc)

echo ""
echo "========================================"
echo "Build Completed!"
echo "Binary in: ${BUILD_DIR}"
echo "========================================"