#!/bin/bash
# ===========================================================================
# verify-sdk.sh - Verification of the AGL SDK
# ===========================================================================

source /opt/agl-sdk/environment-setup-armv7vet2hf-neon-vfpv4-agl-linux-gnueabi

SDK_VERSION="${AGL_SDK_VERSION:-20.0.2}"

echo "==========================================================="
echo "AGL SDK ${SDK_VERSION} - Verification"
echo "==========================================================="
echo ""

echo ">>> Toolchain:"
echo "CC:  $CC"
echo "CXX: $CXX"
$CC --version | head -1
echo ""

echo ">>> Protobuf:"
which protoc && protoc --version
echo ""

echo ">>> gRPC:"
which grpc_cpp_plugin
echo ""

echo ">>> Qt6 Libraries:"
ls /opt/agl-sdk/sysroots/armv7vet2hf-neon-vfpv4-agl-linux-gnueabi/usr/lib/libQt6Core.so* 2>/dev/null
echo ""

echo ">>> Qt6 Modules disponíveis:"
ls /opt/agl-sdk/sysroots/armv7vet2hf-neon-vfpv4-agl-linux-gnueabi/usr/lib/cmake/ 2>/dev/null | grep -i "^Qt6" | head -15
echo ""

echo "==========================================================="
echo "SDK ready for cross-compilation!"
echo "==========================================================="