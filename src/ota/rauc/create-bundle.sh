#!/bin/bash
# RAUC Bundle Creator for SEA:ME Team 6
# Creates a .raucb bundle from a rootfs image
#
# Usage: ./create-bundle.sh <platform> <version>
# Example: ./create-bundle.sh rpi4 v1.10.0

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PLATFORM="${1:-rpi4}"
VERSION="${2:-v1.0.0}"

# Configuration
CERT="$SCRIPT_DIR/dev-cert.pem"
KEY="$SCRIPT_DIR/dev-key.pem"
OUTPUT_DIR="$SCRIPT_DIR/bundles"

log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

error() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] ERROR: $1" >&2
    exit 1
}

# Validate inputs
if [ "$PLATFORM" != "rpi4" ] && [ "$PLATFORM" != "rpi5" ]; then
    error "Platform must be 'rpi4' or 'rpi5'"
fi

# Check for rauc
if ! command -v rauc &> /dev/null; then
    error "RAUC is not installed. Install with: brew install rauc (macOS) or apt install rauc (Linux)"
fi

# Check certificates
if [ ! -f "$CERT" ] || [ ! -f "$KEY" ]; then
    error "Certificates not found. Run this from the rauc directory."
fi

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Create temporary directory for bundle content
TEMP_DIR=$(mktemp -d)
trap "rm -rf $TEMP_DIR" EXIT

log "Creating RAUC bundle for $PLATFORM $VERSION"

# Create manifest
cat > "$TEMP_DIR/manifest.raucm" << EOF
[update]
compatible=seame-team6-$PLATFORM
version=$VERSION
description=SEA:ME Team 6 OTA Update $VERSION for $PLATFORM
build=$(date +%Y%m%d-%H%M%S)

[image.rootfs]
filename=rootfs.img
sha256=$(sha256sum /dev/null | cut -d' ' -f1)
EOF

log "Created manifest"

# Note: In a real scenario, you would include the actual rootfs image
# For now, we create a placeholder
log "NOTE: This is a template bundle creator."
log "To create a real bundle, you need to:"
log "1. Create a rootfs image (e.g., from Yocto/AGL build)"
log "2. Update the manifest with the correct sha256"
log "3. Include the rootfs.img in the bundle"

# Create a dummy rootfs for testing (small file)
dd if=/dev/zero of="$TEMP_DIR/rootfs.img" bs=1M count=1 2>/dev/null

# Update sha256 in manifest
SHA256=$(sha256sum "$TEMP_DIR/rootfs.img" | cut -d' ' -f1)
sed -i.bak "s/sha256=.*/sha256=$SHA256/" "$TEMP_DIR/manifest.raucm" 2>/dev/null || \
sed -i '' "s/sha256=.*/sha256=$SHA256/" "$TEMP_DIR/manifest.raucm"

log "Manifest content:"
cat "$TEMP_DIR/manifest.raucm"

# Create the bundle
BUNDLE_NAME="update-$PLATFORM-$VERSION.raucb"
BUNDLE_PATH="$OUTPUT_DIR/$BUNDLE_NAME"

log "Creating bundle: $BUNDLE_PATH"

rauc bundle \
    --cert="$CERT" \
    --key="$KEY" \
    "$TEMP_DIR" \
    "$BUNDLE_PATH"

log "Bundle created successfully!"
log ""
log "Bundle info:"
rauc info "$BUNDLE_PATH"

log ""
log "To install on device:"
log "  scp $BUNDLE_PATH root@<IP>:/tmp/"
log "  ssh root@<IP> 'rauc install /tmp/$BUNDLE_NAME'"
