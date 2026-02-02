#!/bin/bash

BUILD_TYPE="${1:-Debug}"
BUILD_DIR="build-dev"

echo "========================================"
echo "Building Qt6 Cluster (x86_64 native)"
echo "Build type: ${BUILD_TYPE}"
echo "Output: ${BUILD_DIR}"
echo "========================================"

mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

cmake .. \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DCMAKE_PREFIX_PATH=/opt/qt6 \
    -GNinja

ninja -j$(nproc)

echo ""
echo "========================================"
echo "Build complete!"
echo "Run: ./${BUILD_DIR}/HelloQt6Qml"
echo "========================================"