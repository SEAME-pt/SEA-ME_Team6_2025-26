#!/bin/bash
set -e

# Configuration
BASE_IMAGE="souzitaaaa/team6-base:bookworm"
AGL_SDK_IMAGE="team6-agl-sdk:latest"
PROJECT_DIR="cluster"
TARGET_HOST="root@10.21.220.137"
TARGET_PATH="/home"
BINARY_NAME="HelloQt6Qml"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# ===========================================================================
# Helper Functions
# ===========================================================================

print_header() {
    echo ""
    echo -e "${BLUE}========================================"
    echo -e "$1"
    echo -e "========================================${NC}"
    echo ""
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

print_info() {
    echo -e "${BLUE}ℹ $1${NC}"
}

check_command() {
    if ! command -v "$1" &> /dev/null; then
        print_error "$1 not found. Please install it first."
        exit 1
    fi
}

# ===========================================================================
# Step 1: Check Prerequisites
# ===========================================================================

print_header "Step 1: Checking Prerequisites"

check_command docker
check_command scp

# Check if docker daemon is running
if ! docker info &> /dev/null; then
    print_error "Docker daemon is not running. Please start Docker."
    exit 1
fi

print_success "All prerequisites met"

# ===========================================================================
# Step 2: Check Base Image
# ===========================================================================

print_header "Step 2: Checking Base Image"

if docker image inspect "${BASE_IMAGE}" &> /dev/null; then
    print_success "Base image '${BASE_IMAGE}' exists"
else
    print_warning "Base image '${BASE_IMAGE}' not found locally"
    print_info "Attempting to pull from Docker Hub..."
    
    if docker pull "${BASE_IMAGE}"; then
        print_success "Successfully pulled '${BASE_IMAGE}'"
    else
        print_error "Failed to pull '${BASE_IMAGE}'. Please build it manually."
        exit 1
    fi
fi

# ===========================================================================
# Step 3: Check/Build AGL SDK Image
# ===========================================================================

print_header "Step 3: Checking AGL SDK Image"

if docker image inspect "${AGL_SDK_IMAGE}" &> /dev/null; then
    print_success "AGL SDK image '${AGL_SDK_IMAGE}' exists"
else
    print_warning "AGL SDK image '${AGL_SDK_IMAGE}' not found"
    print_info "Building AGL SDK image..."
    
    if [ ! -f "Dockerfile.agl-sdk" ]; then
        print_error "Dockerfile.agl-sdk not found in current directory"
        exit 1
    fi
    
    # Check if SDK installer exists
    SDK_INSTALLER=$(ls poky-agl-glibc-x86_64-agl-image-minimal-crosssdk-aarch64-raspberrypi5-toolchain-*.sh 2>/dev/null | head -n 1)
    if [ -z "$SDK_INSTALLER" ]; then
        print_error "SDK installer not found. Please download the AGL SDK installer first."
        exit 1
    fi
    
    print_info "Using SDK installer: ${SDK_INSTALLER}"
    
    # Build the image
    if DOCKER_BUILDKIT=1 docker build -f Dockerfile.agl-sdk -t "${AGL_SDK_IMAGE}" .; then
        print_success "Successfully built '${AGL_SDK_IMAGE}'"
    else
        print_error "Failed to build AGL SDK image"
        exit 1
    fi
fi

# ===========================================================================
# Step 4: Build Project in Container
# ===========================================================================

print_header "Step 4: Building Project in Container"

print_info "Starting Docker container..."
print_info "Project directory: ${PROJECT_DIR}"

# Run the container and execute the build
docker run -it --rm \
    -v "$(pwd):/workspace" \
    "${AGL_SDK_IMAGE}" \
    bash -c "
        set -e
        echo '========================================'
        echo 'Building Qt6 project...'
        echo '========================================'
        echo ''
        
        # Source the environment
        source /opt/agl-sdk/environment-setup-aarch64-agl-linux
        
        # Run the build script
        qt6-build.sh /workspace/${PROJECT_DIR}
        
        echo ''
        echo '========================================'
        echo 'Build completed successfully!'
        echo '========================================'
    "

if [ $? -ne 0 ]; then
    print_error "Build failed inside container"
    exit 1
fi

print_success "Build completed successfully"

# ===========================================================================
# Step 5: Wait and Verify Build Output
# ===========================================================================

print_header "Step 5: Verifying Build Output"

BUILD_DIR="${PROJECT_DIR}/build-arm"
BINARY_PATH="${BUILD_DIR}/${BINARY_NAME}"

print_info "Checking for binary at: ${BINARY_PATH}"

# Wait a moment to ensure filesystem is synced
sleep 2

if [ ! -f "${BINARY_PATH}" ]; then
    print_error "Binary '${BINARY_NAME}' not found in ${BUILD_DIR}"
    print_info "Available files in build directory:"
    ls -la "${BUILD_DIR}" 2>/dev/null || echo "Build directory not found"
    exit 1
fi

print_success "Binary found: ${BINARY_PATH}"

# Get file info
FILE_SIZE=$(du -h "${BINARY_PATH}" | cut -f1)
print_info "Binary size: ${FILE_SIZE}"

# ===========================================================================
# Step 6: Deploy to Target Device
# ===========================================================================

print_header "Step 6: Deploying to Target Device"

print_info "Copying ${BINARY_NAME} to ${TARGET_HOST}:${TARGET_PATH}"

# Try to copy the binary
if scp -o StrictHostKeyChecking=no "${BINARY_PATH}" "${TARGET_HOST}:${TARGET_PATH}"; then
    print_success "Successfully deployed ${BINARY_NAME} to ${TARGET_HOST}:${TARGET_PATH}"
else
    print_error "Failed to deploy binary to target device"
    print_warning "Please check:"
    echo "  - SSH access to ${TARGET_HOST}"
    echo "  - Network connectivity"
    echo "  - Target path permissions"
    exit 1
fi