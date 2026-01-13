#!/bin/bash
set -e

# =========================================================
# Usage
# =========================================================
if [ $# -ne 2 ]; then
    echo "Usage: $0 <Dockerfile path> <image_name[:tag]>"
    echo "Example: $0 cross-compiler/Dockerfile.sysroot souzitaaaa/qt-cross-sysroot:bookworm"
    exit 1
fi

DOCKERFILE="$1"
IMAGE="$2"

# =========================================================
# Colors
# =========================================================
RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[1;33m"
BLUE="\033[0;34m"
RESET="\033[0m"

# =========================================================
# Confirm
# =========================================================
echo -e "${BLUE}Dockerfile:${RESET} $DOCKERFILE"
echo -e "${BLUE}Image to build & push:${RESET} $IMAGE"
read -p "Proceed? (y/N): " CONFIRM
if [[ "$CONFIRM" != "y" && "$CONFIRM" != "Y" ]]; then
    echo -e "${RED}Aborted${RESET}"
    exit 1
fi

# =========================================================
# Build without cache
# =========================================================
echo -e "${YELLOW}Building image without cache...${RESET}"
docker build --no-cache -f "$DOCKERFILE" -t "$IMAGE" .

echo -e "${GREEN}Build completed successfully!${RESET}"

# =========================================================
# Push to Docker Hub
# =========================================================
echo -e "${YELLOW}Pushing image to Docker Hub...${RESET}"
docker push "$IMAGE"

echo -e "${GREEN}Image pushed successfully!${RESET}"
