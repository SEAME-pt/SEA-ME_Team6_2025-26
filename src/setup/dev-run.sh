#!/bin/bash
# ===========================================================================
# dev-run.sh - Start development environment
# ===========================================================================

set -e

IMAGE_NAME="cluster-dev:latest"
CONTAINER_NAME="cluster-dev"
CLUSTER_DIR="$(pwd)/cluster"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_header() {
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}$1${NC}"  
    echo -e "${GREEN}========================================${NC}"
}

# Check if cluster directory exists
if [ ! -d "$CLUSTER_DIR" ]; then
    echo -e "${RED}Error: cluster/ directory not found!${NC}"
    echo "Run this script from the project root (where cluster/ is located)"
    exit 1
fi

# Build image if it doesn't exist
if ! docker image inspect $IMAGE_NAME >/dev/null 2>&1; then
    print_header "Building development image..."
    docker build -f setup/Dockerfile.dev -t $IMAGE_NAME setup/
fi

# X11 forwarding setup for GUI
XSOCK=/tmp/.X11-unix
XAUTH=/tmp/.docker.xauth
touch $XAUTH
xauth nlist $DISPLAY | sed -e 's/^..../ffff/' | xauth -f $XAUTH nmerge -

print_header "Starting development container"

docker run -it --rm \
    --name $CONTAINER_NAME \
    -v $CLUSTER_DIR:/workspace \
    -v $XSOCK:$XSOCK:rw \
    -v $XAUTH:$XAUTH:rw \
    -e DISPLAY=$DISPLAY \
    -e XAUTHORITY=$XAUTH \
    --network host \
    $IMAGE_NAME \
    bash

# Cleanup
rm -f $XAUTH