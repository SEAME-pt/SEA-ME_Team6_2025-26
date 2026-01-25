#!/bin/bash
# ============================================================================
# build-cluster.sh
# Helper script to build and deploy the cluster application
# 
# Usage:
#   ./build-cluster.sh              # Build only
#   ./build-cluster.sh deploy       # Build & copy to Pi
#   ./build-cluster.sh run          # Build & run locally (if possible)
#   ./build-cluster.sh clean        # Remove build artifacts
# ============================================================================

set -e

# Configuration
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DOCKER_IMAGE="team6-cluster:latest"
DOCKERFILE="setup/Dockerfile.cluster"
ARTIFACTS_DIR="${PROJECT_ROOT}/artifacts"
PI_USER="${PI_USER:-pi}"
PI_HOST="${PI_HOST:-raspberrypi.local}"
PI_DEST="${PI_DEST:-/home/pi}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# ============================================================================
# FUNCTIONS
# ============================================================================

print_header() {
    echo -e "\n${BLUE}========================================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}========================================================${NC}\n"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_info() {
    echo -e "${YELLOW}ℹ $1${NC}"
}

check_docker() {
    if ! command -v docker &> /dev/null; then
        print_error "Docker is not installed"
        exit 1
    fi
    print_success "Docker found"
}

check_dependencies() {
    print_info "Checking dependencies..."
    check_docker
    
    if [ "$1" == "deploy" ] && ! command -v scp &> /dev/null; then
        print_error "scp is required for deployment"
        exit 1
    fi
}

build_image() {
    print_header "Building Docker Image"
    
    if [ ! -f "$DOCKERFILE" ]; then
        print_error "Dockerfile not found: $DOCKERFILE"
        exit 1
    fi
    
    docker build \
        -f "$DOCKERFILE" \
        -t "$DOCKER_IMAGE" \
        "$PROJECT_ROOT"
    
    print_success "Docker image built: $DOCKER_IMAGE"
}

extract_binary() {
    print_header "Extracting Binary"
    
    mkdir -p "$ARTIFACTS_DIR"
    
    docker run --rm \
        -v "$ARTIFACTS_DIR":/artifacts \
        "$DOCKER_IMAGE"
    
    if [ ! -f "$ARTIFACTS_DIR/cluster-arm64" ]; then
        print_error "Binary extraction failed"
        exit 1
    fi
    
    print_success "Binary extracted to: $ARTIFACTS_DIR/cluster-arm64"
    
    # Show binary info
    echo ""
    echo "Binary Information:"
    file "$ARTIFACTS_DIR/cluster-arm64"
    ls -lh "$ARTIFACTS_DIR/cluster-arm64"
}

verify_binary() {
    print_header "Verifying Binary"
    
    if [ ! -f "$ARTIFACTS_DIR/cluster-arm64" ]; then
        print_error "Binary not found at $ARTIFACTS_DIR/cluster-arm64"
        exit 1
    fi
    
    BINARY_TYPE=$(file "$ARTIFACTS_DIR/cluster-arm64" | grep -o "ARM aarch64" || echo "UNKNOWN")
    
    if [ "$BINARY_TYPE" == "ARM aarch64" ]; then
        print_success "Binary is ARM64 (correct architecture)"
    else
        print_error "Binary is not ARM64: $BINARY_TYPE"
        exit 1
    fi
}

deploy_to_pi() {
    print_header "Deploying to Raspberry Pi"
    
    if [ ! -f "$ARTIFACTS_DIR/cluster-arm64" ]; then
        print_error "Binary not found. Run build first."
        exit 1
    fi
    
    # Check if Pi is reachable
    if ! ping -c 1 "$PI_HOST" &> /dev/null; then
        print_error "Cannot reach Pi at $PI_HOST"
        echo "Update PI_HOST environment variable:"
        echo "  export PI_HOST=192.168.1.100"
        exit 1
    fi
    
    print_info "Copying to $PI_USER@$PI_HOST:$PI_DEST"
    
    scp "$ARTIFACTS_DIR/cluster-arm64" "$PI_USER@$PI_HOST:$PI_DEST/"
    
    print_success "Binary deployed to Pi"
    
    echo ""
    echo "Next steps:"
    echo "  1. ssh $PI_USER@$PI_HOST"
    echo "  2. chmod +x $PI_DEST/cluster-arm64"
    echo "  3. $PI_DEST/cluster-arm64"
}

run_on_pi() {
    print_header "Running Application on Pi"
    
    if [ -z "$PI_HOST" ] || [ -z "$PI_USER" ]; then
        print_error "PI_HOST and PI_USER not set"
        echo "Set them with:"
        echo "  export PI_HOST=raspberrypi.local"
        echo "  export PI_USER=pi"
        exit 1
    fi
    
    print_info "Connecting to $PI_USER@$PI_HOST..."
    
    ssh "$PI_USER@$PI_HOST" << 'EOF'
chmod +x ~/cluster-arm64
echo "Starting cluster application..."
~/cluster-arm64
EOF
}

clean_artifacts() {
    print_header "Cleaning Artifacts"
    
    if [ -d "$ARTIFACTS_DIR" ]; then
        rm -rf "$ARTIFACTS_DIR"
        print_success "Artifacts cleaned"
    fi
    
    # Optionally remove Docker image
    read -p "Remove Docker image '$DOCKER_IMAGE'? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        docker rmi "$DOCKER_IMAGE" || true
        print_success "Docker image removed"
    fi
}

show_usage() {
    cat << EOF
${BLUE}Cluster Application Build & Deploy Helper${NC}

Usage: $(basename "$0") [COMMAND]

Commands:
  (empty)      Build Docker image and extract binary
  deploy       Build and deploy to Raspberry Pi
  run          Build and run on Raspberry Pi
  clean        Remove build artifacts and images
  help         Show this message

Environment Variables:
  PI_HOST      Raspberry Pi hostname/IP (default: raspberrypi.local)
  PI_USER      Raspberry Pi username (default: pi)
  PI_DEST      Destination directory on Pi (default: /home/pi)

Examples:
  # Build for the first time
  ./build-cluster.sh

  # Deploy to a specific Pi
  export PI_HOST=192.168.1.100
  ./build-cluster.sh deploy

  # Build and run directly
  PI_HOST=mypi.local PI_USER=pi ./build-cluster.sh run

  # Clean everything
  ./build-cluster.sh clean
EOF
}

# ============================================================================
# MAIN
# ============================================================================

main() {
    local command="${1:-build}"
    
    case "$command" in
        build)
            check_dependencies "build"
            build_image
            extract_binary
            verify_binary
            echo ""
            print_success "Build completed successfully!"
            echo "Binary ready: $ARTIFACTS_DIR/cluster-arm64"
            ;;
        deploy)
            check_dependencies "deploy"
            build_image
            extract_binary
            verify_binary
            deploy_to_pi
            echo ""
            print_success "Deployment completed!"
            ;;
        run)
            check_dependencies "run"
            build_image
            extract_binary
            verify_binary
            deploy_to_pi
            run_on_pi
            ;;
        clean)
            clean_artifacts
            ;;
        help|--help|-h)
            show_usage
            ;;
        *)
            print_error "Unknown command: $command"
            echo ""
            show_usage
            exit 1
            ;;
    esac
}

# Run main function
main "$@"