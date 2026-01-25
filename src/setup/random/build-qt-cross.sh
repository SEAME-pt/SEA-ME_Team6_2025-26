#!/bin/bash

set -e

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Configuration
DOCKER_REGISTRY="souzitaaaa"
BASE_TAG="bookworm"
QT_VERSION="6.7.3"

# Image Names
    # Cross-Compilation
IMAGE_BASE="${DOCKER_REGISTRY}/team6-base:${BASE_TAG}"
IMAGE_AGL_SDK="${DOCKER_REGISTRY}/team6-agl-sdk:${BASE_TAG}"
IMAGE_QT="${DOCKER_REGISTRY}/team6-cross-qt:${BASE_TAG}"
IMAGE_CLUSTER="team6-cross-cluster:arm64"
    # Development & Testing


# Logger Functions
print_info() {
    echo -e "${BLUE}[INFO]${NC}     $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC}  $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC}  $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC}    $1"
}

print_header() {
    echo -e "${GREEN}================================${NC}"
    echo -e "${GREEN}$1${NC}"
    echo -e "${GREEN}================================${NC}"
}

# Helper Functions
    # Check if Docker is running
check_docker() {
    if ! docker info > /dev/null 2>&1; then
        print_error "Docker is not running."
        exit 1
    fi
    print_success "Docker is running"
}

    # Check if file exists
check_file() {
    if [ ! -f "$1" ]; then
        print_error "Required file not found: $1"
        exit 1
    fi
}

    # Find AGL SDK installer in setup/sdk-agl folder
find_sdk_installer() {
    local sdk_installer=$(find ./setup/sdk-agl -maxdepth 1 -name "poky-agl-glibc-x86_64-agl-image-minimal-crosssdk-*.sh" 2>/dev/null | head -1)
    
    if [ -z "$sdk_installer" ]; then
        print_error "AGL SDK installer not found in ./setup/sdk-agl directory"
        print_error "Expected: ./setup/sdk-agl/poky-agl-glibc-x86_64-agl-image-minimal-crosssdk-*.sh"
        return 1
    fi
    
    # Return the relative path for Docker COPY
    echo "$sdk_installer"
}

    # Check if image exists on Docker Hub
check_remote_image() {
    local image=$1
    print_info "Checking Docker Hub for: $image"

    if docker manifest inspect "$image" > /dev/null 2>&1; then
        print_success "Image found on Docker Hub: $image"
        return 0
    else
        print_warning "Image not found on Docker Hub: $image"
        return 1
    fi
}

    # Pull image from Docker Hub
pull_image() {
    local image=$1
    print_header "Pulling: $image"

    if docker pull "$image"; then
        print_success "Successfully pulled: $image"
        return 0
    else
        print_error "Failed to pull: $image"
        return 1
    fi
}

    # Check if image exists locally
image_exists() {
    docker image inspect "$1" > /dev/null 2>&1
}

    # Handle image retrieval (pull or build)
get_or_build_image() {
    local dockerfile=$1
    local image_name=$2
    local context=${3:-.}
    local description=$4
    local sdk_installer=${5:-}

    # Check if image exists locally
    if image_exists "$image_name" && [ "$CLEAN" = false ]; then
        print_warning "Image already exists locally: $image_name"
        
        if [ "$FORCE_BUILD" = false ] && [ "$PREFER_PULL" = true ]; then
            print_info "Using existing local image"
            return 0
        fi
        
        if [ "$FORCE_BUILD" = false ]; then
            read -p "Use existing local image? (y/n): " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                return 0
            fi
        fi
    fi
    
    # Check Docker Hub if pull is preferred or interactive mode
    if [ "$PREFER_PULL" = true ] || [ "$INTERACTIVE" = true ]; then
        if check_remote_image "$image_name"; then
            if [ "$PREFER_PULL" = true ]; then
                pull_image "$image_name" && return 0
            elif [ "$INTERACTIVE" = true ]; then
                read -p "Pull from Docker Hub instead of building? (y/n): " -n 1 -r
                echo
                if [[ $REPLY =~ ^[Yy]$ ]]; then
                    pull_image "$image_name" && return 0
                fi
            fi
        fi
    fi
    
    build_image "$dockerfile" "$image_name" "$context" "$description" "$sdk_installer"
}

    # Build an image
build_image() {
    local dockerfile=$1
    local image_name=$2
    local context=${3:-.}
    local description=$4
    local sdk_installer=${5:-}

    print_header "Building: $description"
    print_info "Dockerfile: $dockerfile"
    print_info "Image:      $image_name"
    print_info "Context:    $context"
    
    if [ -n "$sdk_installer" ]; then
        print_info "SDK Installer: $sdk_installer"
        if docker build $SKIP_CACHE -f "$dockerfile" --build-arg SDK_INSTALLER="$sdk_installer" -t "$image_name" "$context"; then
            print_success "Successfully built: $image_name"
            return 0
        else
            print_error "Failed to build: $image_name"
            return 1
        fi
    else
        if docker build $SKIP_CACHE -f "$dockerfile" -t "$image_name" "$context"; then
            print_success "Successfully built: $image_name"
            return 0
        else
            print_error "Failed to build: $image_name"
            return 1
        fi
    fi
}

    # Extract artifacts from cluster image
extract_artifacts() {
    print_header "Extracting Artifacts"
    
    local output_dir=${1:-./artifacts}
    mkdir -p "$output_dir"
    
    print_info "Creating temporary container..."
    docker create --name tmpbuild "$IMAGE_CLUSTER" > /dev/null 2>&1
    
    print_info "Extracting application binary..."
    if docker cp tmpbuild:/build/project/HelloQt6Qml "$output_dir/HelloQt6Qml" 2>/dev/null; then
        print_success "Application extracted: HelloQt6Qml"
    else
        print_warning "HelloQt6Qml not found in image"
    fi
    
    print_info "Extracting Qt binaries..."
    if docker cp tmpbuild:/output/qt-pi-binaries.tar.gz "$output_dir/qt-pi-binaries.tar.gz" 2>/dev/null; then
        print_success "Qt binaries extracted: qt-pi-binaries.tar.gz"
    else
        print_warning "qt-pi-binaries.tar.gz not found"
    fi
    
    print_info "Cleaning up temporary container..."
    docker rm tmpbuild > /dev/null 2>&1
    
    print_success "Artifacts extracted to: $output_dir"
    echo ""
    echo "Contents:"
    ls -lh "$output_dir"
}

    # Display usage
usage() {
    cat << EOF
Usage: $0 [OPTIONS] [TARGETS]

Build Docker images for Qt cross-compilation using AGL SDK.

OPTIONS:
    -h, --help          Show this help message
    -c, --clean         Remove existing images before building
    -p, --push          Push images to Docker Hub after building
    -e, --extract       Extract artifacts from cluster image
    -o, --output DIR    Output directory for artifacts (default: ./artifacts)
    --skip-cache        Build without using cache
    --dev-only          Build only development environment
    --cross-only        Build only cross-compilation chain
    --pull              Prefer pulling from Docker Hub over building
    --force-build       Force rebuild even if images exist locally
    --no-interactive    Don't prompt for user input (use with --pull)

TARGETS (build specific images):
    base                Build base image
    agl-sdk             Build AGL SDK image (replaces sysroot)
    qt                  Build Qt cross-compiled image
    dev                 Build development image
    cluster             Build cluster application
    all                 Build all images (default)

EXAMPLES:
    $0                          # Build all images (interactive)
    $0 --pull                   # Pull all images from Docker Hub
    $0 --pull --no-interactive  # Pull without prompts (fail if not on Hub)
    $0 base agl-sdk qt          # Build base, AGL SDK, and Qt
    $0 --clean all              # Clean and rebuild all images
    $0 --force-build all        # Force rebuild all images
    $0 cluster --extract        # Build cluster and extract artifacts
    $0 --pull --push            # Pull images, then push (useful for mirrors)

REQUIREMENTS:
    - AGL SDK installer must be in the setup/sdk-agl folder:
      ./setup/sdk-agl/poky-agl-glibc-x86_64-agl-image-minimal-crosssdk-*.sh
EOF
}

# Command line arguments
CLEAN=false
PUSH=false
EXTRACT=false
OUTPUT_DIR="./artifacts"
SKIP_CACHE=""
TARGETS=()
DEV_ONLY=false
CROSS_ONLY=false
PREFER_PULL=false
FORCE_BUILD=false
INTERACTIVE=true

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            usage
            exit 0
            ;;
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -p|--push)
            PUSH=true
            shift
            ;;
        -e|--extract)
            EXTRACT=true
            shift
            ;;
        -o|--output)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        --skip-cache)
            SKIP_CACHE="--no-cache"
            shift
            ;;
        --dev-only)
            DEV_ONLY=true
            shift
            ;;
        --cross-only)
            CROSS_ONLY=true
            shift
            ;;
        --pull)
            PREFER_PULL=true
            shift
            ;;
        --force-build)
            FORCE_BUILD=true
            shift
            ;;
        --no-interactive)
            INTERACTIVE=false
            shift
            ;;
        base|agl-sdk|qt|dev|cluster|all)
            TARGETS+=("$1")
            shift
            ;;
        *)
            print_error "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# If no targets specified, build all
if [ ${#TARGETS[@]} -eq 0 ]; then
    TARGETS=("all")
fi

check_docker

# Find AGL SDK installer before checking files
SDK_INSTALLER=""
if [ "$PREFER_PULL" = false ] || [ "$INTERACTIVE" = true ]; then
    # Check if we'll need the SDK installer (not just pulling)
    for target in "${TARGETS[@]}"; do
        if [[ "$target" == "agl-sdk" ]] || [[ "$target" == "all" ]] || [[ "$target" == "qt" ]] || [[ "$target" == "cluster" ]]; then
            if [ "$PREFER_PULL" = false ]; then
                print_info "Looking for AGL SDK installer..."
                SDK_INSTALLER=$(find_sdk_installer) || exit 1
                print_success "Found SDK installer: $SDK_INSTALLER"
                break
            fi
        fi
    done
fi

# Check required files (only if we're going to build)
if [ "$PREFER_PULL" = false ] || [ "$INTERACTIVE" = true ]; then
    print_info "Checking required files..."
    check_file "setup/Dockerfile.base"
    check_file "setup/Dockerfile.agl-sdk"
    check_file "setup/Dockerfile.qt"
    check_file "setup/Dockerfile.cluster"

    if [ -f "setup/Dockerfile.dev" ]; then
        print_success "Development Dockerfile found"
    else
        print_warning "Dockerfile.dev not found - dev build will be skipped"
    fi
fi

# Clean images if true
if [ "$CLEAN" = true ]; then
    print_header "Cleaning existing images"
    docker rmi -f "$IMAGE_BASE" 2>/dev/null || true
    docker rmi -f "$IMAGE_AGL_SDK" 2>/dev/null || true
    docker rmi -f "$IMAGE_QT" 2>/dev/null || true
    docker rmi -f "$IMAGE_DEV" 2>/dev/null || true
    docker rmi -f "$IMAGE_CLUSTER" 2>/dev/null || true
    print_success "Cleaned existing images"
fi

START_TIME=$(date +%s)

# Determine what to build
BUILD_BASE=false
BUILD_AGL_SDK=false
BUILD_QT=false
BUILD_DEV=false
BUILD_CLUSTER=false

if [ "$DEV_ONLY" = true ]; then
    BUILD_BASE=true
    BUILD_DEV=true
elif [ "$CROSS_ONLY" = true ]; then
    BUILD_BASE=true
    BUILD_AGL_SDK=true
    BUILD_QT=true
    BUILD_CLUSTER=true
else
    for target in "${TARGETS[@]}"; do
        case $target in
            all)
                BUILD_BASE=true
                BUILD_AGL_SDK=true
                BUILD_QT=true
                BUILD_DEV=true
                BUILD_CLUSTER=true
                ;;
            base)
                BUILD_BASE=true
                ;;
            agl-sdk)
                BUILD_BASE=true
                BUILD_AGL_SDK=true
                ;;
            qt)
                BUILD_BASE=true
                BUILD_AGL_SDK=true
                BUILD_QT=true
                ;;
            dev)
                BUILD_BASE=true
                BUILD_DEV=true
                ;;
            cluster)
                BUILD_BASE=true
                BUILD_AGL_SDK=true
                BUILD_QT=true
                BUILD_CLUSTER=true
                ;;
        esac
    done
fi

# Build/Pull base image
if [ "$BUILD_BASE" = true ]; then
    get_or_build_image "setup/Dockerfile.base" "$IMAGE_BASE" "." "Base Build Tools" "" || exit 1
fi

# Build/Pull AGL SDK image (replaces sysroot)
if [ "$BUILD_AGL_SDK" = true ]; then
    get_or_build_image "setup/Dockerfile.agl-sdk" "$IMAGE_AGL_SDK" "." "AGL SDK Cross-Compilation" "$SDK_INSTALLER" || exit 1
fi

# Build/Pull Qt cross-compiled image
if [ "$BUILD_QT" = true ]; then
    get_or_build_image "setup/Dockerfile.qt" "$IMAGE_QT" "." "Qt 6.7.3 Cross-Compilation" "" || exit 1
fi

# Build/Pull development image
if [ "$BUILD_DEV" = true ]; then
    if [ -f "setup/Dockerfile.dev" ]; then
        get_or_build_image "setup/Dockerfile.dev" "$IMAGE_DEV" "." "Development Environment" "" || exit 1
    else
        print_warning "setup/Dockerfile.dev not found, skipping development build"
    fi
fi

# Build/Pull cluster application
if [ "$BUILD_CLUSTER" = true ]; then
    get_or_build_image "setup/Dockerfile.cluster" "$IMAGE_CLUSTER" "." "Cluster Application" "" || exit 1
fi

# Calculate total build time
END_TIME=$(date +%s)
DURATION=$((END_TIME - START_TIME))
MINUTES=$((DURATION / 60))
SECONDS=$((DURATION % 60))

print_header "Build Complete!"
echo "Total time: ${MINUTES}m ${SECONDS}s"
echo ""
echo "Available images:"
if [ "$BUILD_BASE" = true ]; then
    echo "  ✓ $IMAGE_BASE"
fi
if [ "$BUILD_AGL_SDK" = true ]; then
    echo "  ✓ $IMAGE_AGL_SDK"
fi
if [ "$BUILD_QT" = true ]; then
    echo "  ✓ $IMAGE_QT"
fi
if [ "$BUILD_DEV" = true ] && [ -f "setup/Dockerfile.dev" ]; then
    echo "  ✓ $IMAGE_DEV"
fi
if [ "$BUILD_CLUSTER" = true ]; then
    echo "  ✓ $IMAGE_CLUSTER"
fi
echo ""

# Extract artifacts if requested
if [ "$EXTRACT" = true ]; then
    if image_exists "$IMAGE_CLUSTER"; then
        extract_artifacts "$OUTPUT_DIR"
    else
        print_error "Cluster image not found. Build it first with: $0 cluster"
        exit 1
    fi
fi

# Push to Docker Hub if true
if [ "$PUSH" = true ]; then
    print_header "Pushing images to Docker Hub"
    
    if [ "$BUILD_BASE" = true ]; then
        docker push "$IMAGE_BASE"
    fi
    if [ "$BUILD_AGL_SDK" = true ]; then
        docker push "$IMAGE_AGL_SDK"
    fi
    if [ "$BUILD_QT" = true ]; then
        docker push "$IMAGE_QT"
    fi
    if [ "$BUILD_DEV" = true ] && [ -f "setup/Dockerfile.dev" ]; then
        docker push "$IMAGE_DEV"
    fi
    
    print_success "All images pushed successfully"
fi

# Final message
if [ "$EXTRACT" = true ]; then
    print_header "Deployment Instructions"
    echo "On your Raspberry Pi 5:"
    echo "  1. Copy artifacts to the Pi:"
    echo "     scp -r $OUTPUT_DIR pi@raspberrypi:~/"
    echo ""
    echo "  2. Extract Qt binaries:"
    echo "     sudo tar -xzf qt-pi-binaries.tar.gz -C /usr/local/qt6"
    echo ""
    echo "  3. Set library path and run:"
    echo "     export LD_LIBRARY_PATH=/usr/local/qt6/lib"
    echo "     ./HelloQt6Qml"
fi