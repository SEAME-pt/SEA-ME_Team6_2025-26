#!/bin/bash
set -e

# =========================================================
# Colors
# =========================================================
RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[1;33m"
BLUE="\033[0;34m"
RESET="\033[0m"

# =========================================================
# Configuration
# =========================================================
DOCKER_USERNAME="souzitaaaa"
BASE_TAG="bookworm"
QT_VERSION="6.7.3"

IMAGE_BASE="${DOCKER_USERNAME}/qt-cross-base:${BASE_TAG}"
IMAGE_SYSROOT="${DOCKER_USERNAME}/qt-cross-sysroot:${BASE_TAG}"
IMAGE_QT="${DOCKER_USERNAME}/qt-cross-compiled:${QT_VERSION}-${BASE_TAG}"
IMAGE_PROJECT="${DOCKER_USERNAME}/qt-cross-project:latest"

# =========================================================
# Helper functions
# =========================================================
image_exists_local() {
    docker image inspect "$1" > /dev/null 2>&1
}

image_exists_remote() {
    docker manifest inspect "$1" > /dev/null 2>&1
}

build_or_pull() {
    local image="$1"
    local dockerfile="$2"
    local label="$3"

    echo -e "${BLUE}${label}${RESET}"

    if image_exists_local "$image"; then
        echo -e "${GREEN}Using local image:${RESET} $image"
        return
    fi

    if image_exists_remote "$image"; then
        echo -e "${GREEN}Found on Docker Hub:${RESET} $image"
        echo -e "${BLUE}Pulling image...${RESET}"
        docker pull "$image" > /dev/null
        return
    fi

    echo -e "${YELLOW}Not found. Building...${RESET}"
    docker build -f "$dockerfile" -t "$image" . > /dev/null
    echo -e "${GREEN}Build completed:${RESET} $image"
}

# =========================================================
# Header
# =========================================================
echo -e "${BLUE}========================================${RESET}"
echo -e "${BLUE}Qt Cross-Compilation Docker Build Script${RESET}"
echo -e "${BLUE}========================================${RESET}"
echo ""

# =========================================================
# Pre-checks
# =========================================================
if [ ! -f "./cross-compiler/rasp.tar.gz" ]; then
    echo -e "${RED}ERROR:${RESET} rasp.tar.gz not found in current directory"
    exit 1
fi

echo -e "${GREEN}OK:${RESET} rasp.tar.gz found"
echo ""

# =========================================================
# Step 1: Base image
# =========================================================
build_or_pull "$IMAGE_BASE" "cross-compiler/Dockerfile.base" "[1/4] Base image"
echo ""

# =========================================================
# Step 2: Sysroot image
# =========================================================
build_or_pull "$IMAGE_SYSROOT" "cross-compiler/Dockerfile.sysroot" "[2/4] Sysroot image"
echo ""

# =========================================================
# Step 3: Qt cross-compiled image
# =========================================================
echo -e "${BLUE}[3/4] Qt cross-compiled image${RESET}"
echo -e "${YELLOW}WARNING:${RESET} This step may take 2–3 hours if build is required"

if image_exists_local "$IMAGE_QT"; then
    echo -e "${GREEN}Using local image:${RESET} $IMAGE_QT"
elif image_exists_remote "$IMAGE_QT"; then
    echo -e "${GREEN}Found on Docker Hub:${RESET} $IMAGE_QT"
    docker pull "$IMAGE_QT" > /dev/null
else
    echo -e "${YELLOW}Not found. Building Qt image...${RESET}"
    docker build -f cross-compiler/Dockerfile.qt -t "$IMAGE_QT" . > /dev/null
    echo -e "${GREEN}Qt image built successfully${RESET}"
fi
echo ""

# =========================================================
# Step 4: Project build (always local)
# =========================================================
echo -e "${BLUE}[4/4] Building project image${RESET}"
docker build -f cross-compiler/Dockerfile.project -t "$IMAGE_PROJECT" . > /dev/null
echo -e "${GREEN}Project image built${RESET}"
echo ""

# =========================================================
# Extract artifacts
# =========================================================
echo -e "${BLUE}Extracting binaries...${RESET}"

docker create --name tmpbuild "$IMAGE_PROJECT" > /dev/null

if docker cp tmpbuild:/build/project/HelloQt6Qml ./HelloQt6Qml 2>/dev/null; then
    echo -e "${GREEN}Application extracted:${RESET} HelloQt6Qml"
else
    echo -e "${YELLOW}Note:${RESET} HelloQt6Qml not found"
fi

docker cp tmpbuild:/build/qt-pi-binaries.tar.gz ./qt-pi-binaries.tar.gz
docker rm tmpbuild > /dev/null

# =========================================================
# Summary
# =========================================================
echo -e "${GREEN}========================================${RESET}"
echo -e "${GREEN}Build Complete${RESET}"
echo -e "${GREEN}========================================${RESET}"
echo ""
echo "Artifacts:"
echo "  - HelloQt6Qml"
echo "  - qt-pi-binaries.tar.gz"
echo ""
echo "Next steps on Raspberry Pi:"
echo "  sudo tar -xzf qt-pi-binaries.tar.gz -C /usr/local/qt6"
echo "  export LD_LIBRARY_PATH=/usr/local/qt6/lib"
echo "  ./HelloQt6Qml"