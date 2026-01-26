#!/bin/bash
# ===========================================================================
# qt6-build.sh - Helper para compilar projectos Qt6
# Uso: qt6-build.sh [directório-do-projecto] [build-type]
# ===========================================================================

source /opt/agl-sdk/environment-setup-aarch64-agl-linux

PROJECT_DIR="${1:-.}"
BUILD_TYPE="${2:-Release}"
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
    --debug-find-pkg=Qt6CoreTools \
    -GNinja

ninja -j$(nproc)

echo ""
echo "========================================"
echo "Build completo!"
echo "Binários em: ${BUILD_DIR}"
echo "========================================"