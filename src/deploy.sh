#!/bin/bash
# Run this in your Docker container

cd /workspace
rm -rf deploy
mkdir -p deploy/lib

# Copy executable
cp /workspace/cluster/build-arm/HelloQt6Qml deploy/

SYSROOT="/opt/agl-sdk/sysroots/aarch64-agl-linux/usr/lib"

echo "Copying required libraries..."

# Direct dependencies from readelf
cp -L "$SYSROOT/libgrpc++.so.1.60" deploy/lib/ 2>/dev/null || cp -P "$SYSROOT"/libgrpc++.so* deploy/lib/
cp -L "$SYSROOT/libgrpc.so.37" deploy/lib/ 2>/dev/null || cp -P "$SYSROOT"/libgrpc.so* deploy/lib/
cp -L "$SYSROOT/libgpr.so.37" deploy/lib/ 2>/dev/null || cp -P "$SYSROOT"/libgpr.so* deploy/lib/
cp -L "$SYSROOT/libprotobuf.so.25.8.0" deploy/lib/ 2>/dev/null || cp -P "$SYSROOT"/libprotobuf.so* deploy/lib/
cp -L "$SYSROOT/libupb.so.37" deploy/lib/ 2>/dev/null || cp -P "$SYSROOT"/libupb.so* deploy/lib/

# Abseil libraries (direct dependencies)
cp -P "$SYSROOT"/libabsl_cord.so* deploy/lib/
cp -P "$SYSROOT"/libabsl_cordz_functions.so* deploy/lib/
cp -P "$SYSROOT"/libabsl_cordz_info.so* deploy/lib/
cp -P "$SYSROOT"/libabsl_hash.so* deploy/lib/
cp -P "$SYSROOT"/libabsl_log_internal_message.so* deploy/lib/
cp -P "$SYSROOT"/libabsl_log_internal_nullguard.so* deploy/lib/
cp -P "$SYSROOT"/libabsl_synchronization.so* deploy/lib/

# Qt6 libraries
cp -P "$SYSROOT"/libQt6Gui.so* deploy/lib/
cp -P "$SYSROOT"/libQt6Qml.so* deploy/lib/
cp -P "$SYSROOT"/libQt6Network.so* deploy/lib/
cp -P "$SYSROOT"/libQt6Core.so* deploy/lib/
cp -P "$SYSROOT"/libQt6Quick*.so* deploy/lib/
cp -P "$SYSROOT"/libQt6QmlModels.so* deploy/lib/
cp -P "$SYSROOT"/libQt6OpenGL.so* deploy/lib/
cp -P "$SYSROOT"/libQt6DBus.so* deploy/lib/

# System libraries (newer versions from SDK)
cp -P "$SYSROOT"/libstdc++.so* deploy/lib/
cp -P "$SYSROOT"/libgcc_s.so* deploy/lib/

# Now copy ALL transitive dependencies that these libraries need
echo "Copying transitive dependencies..."

# All abseil libraries (they depend on each other)
cp -P "$SYSROOT"/libabsl*.so* deploy/lib/

# Additional gRPC dependencies
cp -P "$SYSROOT"/libaddress_sorting.so* deploy/lib/ 2>/dev/null
cp -P "$SYSROOT"/libre2.so* deploy/lib/
cp -P "$SYSROOT"/libutf8*.so* deploy/lib/
cp -P "$SYSROOT"/libupb*.so* deploy/lib/

# SSL/Crypto (often needed by gRPC)
cp -P "$SYSROOT"/libssl.so* deploy/lib/ 2>/dev/null
cp -P "$SYSROOT"/libcrypto.so* deploy/lib/ 2>/dev/null

# c-ares (DNS resolution for gRPC)
cp -P "$SYSROOT"/libcares*.so* deploy/lib/ 2>/dev/null

# zlib
cp -P "$SYSROOT"/libz.so* deploy/lib/ 2>/dev/null

# OpenGL/EGL (for Qt6 GUI)
cp -P "$SYSROOT"/libGLESv2.so* deploy/lib/
cp -P "$SYSROOT"/libEGL.so* deploy/lib/
cp -P "$SYSROOT"/libgbm.so* deploy/lib/ 2>/dev/null
cp -P "$SYSROOT"/libdrm.so* deploy/lib/ 2>/dev/null

# Graphics dependencies
cp -P "$SYSROOT"/libwayland*.so* deploy/lib/ 2>/dev/null
cp -P "$SYSROOT"/libxkbcommon.so* deploy/lib/ 2>/dev/null

# DBus (for Qt6DBus)
cp -P "$SYSROOT"/libdbus-1.so* deploy/lib/ 2>/dev/null

echo "Total libraries copied: $(ls deploy/lib | wc -l) files"

# Create launch script with better environment setup
cat > deploy/run.sh << 'RUNEOF'
#!/bin/bash
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Add our libraries to the library path
export LD_LIBRARY_PATH="${DIR}/lib:${LD_LIBRARY_PATH}"

# Qt6 platform settings for embedded
export QT_QPA_PLATFORM=eglfs
export QT_QPA_EGLFS_INTEGRATION=eglfs_kms

echo "Starting HelloQt6Qml..."
echo "Using libraries from: ${DIR}/lib"

exec "${DIR}/HelloQt6Qml" "$@"
RUNEOF

chmod +x deploy/run.sh

# Create a debug script to check dependencies
cat > deploy/check-deps.sh << 'CHECKEOF'
#!/bin/bash
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="${DIR}/lib:${LD_LIBRARY_PATH}"
ldd "${DIR}/HelloQt6Qml"
CHECKEOF

chmod +x deploy/check-deps.sh

echo ""
echo "Deployment package ready!"
echo "Location: /workspace/deploy/"
echo "Size: $(du -sh deploy | cut -f1)"
echo ""
echo "To check dependencies: ./check-deps.sh"
echo "To run: ./run.sh"