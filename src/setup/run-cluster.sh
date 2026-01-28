#!/bin/bash
# ===========================================================================
# run-cluster.sh - Build e executar Cluster Qt6
# ===========================================================================

set -e

# Configurar DISPLAY (WSL)
if grep -qi microsoft /proc/version 2>/dev/null; then
    export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0
fi

# Verificar se imagem existe, se não criar
if ! docker images | grep -q "team6-agl-sdk"; then
    echo "Imagem team6-agl-sdk não encontrada. A criar..."
    cd setup
    DOCKER_BUILDKIT=1 docker build -f Dockerfile.agl-sdk -t team6-agl-sdk:latest .
    cd ..
fi

# Executar container com GUI
docker run -it --rm \
    -e DISPLAY=$DISPLAY \
    -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
    -v $(pwd)/cluster:/workspace/cluster \
    --network=host \
    team6-agl-sdk:latest \
    bash -c '
        cd /workspace/cluster
        mkdir -p build-native
        cd build-native
        
        echo ">>> Compiling..."
        cmake .. -DCMAKE_BUILD_TYPE=Debug -GNinja
        ninja -j$(nproc)
        
        echo ""
        echo ">>> Running application..."
        ./HelloQt6Qml
    '