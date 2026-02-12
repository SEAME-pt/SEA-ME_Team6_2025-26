#!/bin/bash
# ============================================================
# OTA Update Script v2 - SEA:ME Team 6
# Phase B: Atomic symlinks + improved rollback
# ============================================================

set -e

VERSION="${1:-}"
REPO="SEAME-pt/SEA-ME_Team6_2025-26"

# -------- PATHS --------
WORKDIR="/opt/ota"
LOG="$WORKDIR/logs/ota.log"
DL="$WORKDIR/downloads/update.tar.gz"
HASH_FILE="$WORKDIR/downloads/hash.txt"
RELEASES_DIR="$WORKDIR/releases"
CURRENT_LINK="$WORKDIR/current"

# Target paths for binaries
# NOTE: KUKSA service uses /home/kuksa_RPi5/ (without 'root')
KUKSA_DIR="/home/kuksa_RPi5"
CLUSTER_DIR="/opt/cluster"

mkdir -p "$WORKDIR/logs" "$WORKDIR/downloads" "$RELEASES_DIR"
mkdir -p "$KUKSA_DIR/bin" "$CLUSTER_DIR"

log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a "$LOG"
}

# -------- USAGE --------
if [ -z "$VERSION" ]; then
    echo "Usage: ota-update.sh <VERSION>"
    echo "Example: ota-update.sh v1.5.0"
    echo ""
    echo "Current version: $(cat /etc/ota-version 2>/dev/null || echo 'unknown')"
    echo ""
    echo "Available versions:"
    ls -1 "$RELEASES_DIR" 2>/dev/null || echo "  (none)"
    exit 1
fi

RELEASE_DIR="$RELEASES_DIR/$VERSION"

log "=== OTA Update to $VERSION (Phase B - Atomic) ==="

# -------- CHECK IF ALREADY INSTALLED --------
if [ -d "$RELEASE_DIR" ] && [ "$(cat /etc/ota-version 2>/dev/null)" = "$VERSION" ]; then
    log "Version $VERSION is already installed"
    exit 0
fi

# -------- DETECT PLATFORM --------
detect_platform() {
    local arch=$(uname -m)
    local model=""
    
    # Try to detect Raspberry Pi model
    if [ -f /proc/device-tree/model ]; then
        model=$(cat /proc/device-tree/model 2>/dev/null | tr -d '\0')
    fi
    
    # Log to stderr to not interfere with function return
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] System architecture: $arch" >> "$LOG"
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] Device model: ${model:-unknown}" >> "$LOG"
    
    # RPi5 = aarch64, RPi4 = armv7l (or aarch64 in 64-bit mode)
    if [ "$arch" = "aarch64" ]; then
        # Check if it's RPi5 or RPi4 in 64-bit mode
        if echo "$model" | grep -qi "Raspberry Pi 5"; then
            echo "rpi5"
        elif echo "$model" | grep -qi "Raspberry Pi 4"; then
            # RPi4 running 64-bit OS - but our cluster is 32-bit
            echo "rpi4"
        else
            # Default: assume RPi5 for aarch64 (AGL)
            echo "rpi5"
        fi
    elif [ "$arch" = "armv7l" ] || [ "$arch" = "armv7vet2hf" ]; then
        echo "rpi4"
    else
        echo "[$(date '+%Y-%m-%d %H:%M:%S')] WARNING: Unknown architecture $arch, defaulting to combined package" >> "$LOG"
        echo "combined"
    fi
}

PLATFORM=$(detect_platform)
log "Detected platform: $PLATFORM"

# -------- DOWNLOAD --------
log "[1/10] Downloading package from GitHub Release..."
RELEASE_URL="https://github.com/$REPO/releases/download/$VERSION"

# Select the correct package for the platform
case "$PLATFORM" in
    rpi4)
        PACKAGE_NAME="update-rpi4.tar.gz"
        HASH_NAME="hash-rpi4.txt"
        ;;
    rpi5)
        PACKAGE_NAME="update-rpi5.tar.gz"
        HASH_NAME="hash-rpi5.txt"
        ;;
    *)
        PACKAGE_NAME="update.tar.gz"
        HASH_NAME="hash.txt"
        ;;
esac

log "Downloading $PACKAGE_NAME for $PLATFORM..."
curl -fL "$RELEASE_URL/$PACKAGE_NAME" -o "$DL" || { log "ERROR: Download failed"; exit 1; }
curl -fL "$RELEASE_URL/$HASH_NAME" -o "$HASH_FILE" || { log "ERROR: Hash download failed"; exit 1; }

# -------- VERIFY HASH --------
log "[2/10] Verifying SHA256 hash..."
EXPECTED_HASH=$(cat "$HASH_FILE" | awk '{print $1}')
ACTUAL_HASH=$(sha256sum "$DL" | awk '{print $1}')

if [ "$EXPECTED_HASH" != "$ACTUAL_HASH" ]; then
    log "ERROR: Hash mismatch!"
    log "Expected: $EXPECTED_HASH"
    log "Actual:   $ACTUAL_HASH"
    exit 1
fi
log "Hash verified OK"

# -------- EXTRACT TO VERSION DIRECTORY --------
log "[3/10] Extracting to $RELEASE_DIR..."
rm -rf "$RELEASE_DIR"
mkdir -p "$RELEASE_DIR"
tar -xzf "$DL" -C "$RELEASE_DIR"

log "Package contents:"
find "$RELEASE_DIR" -type f | tee -a "$LOG"

# -------- STOP SERVICES --------
log "[4/10] Stopping services..."
systemctl stop can-to-kuksa.service 2>/dev/null || true
systemctl stop cluster.service 2>/dev/null || true

# Ensure processes are really stopped (workaround for systemd not always killing properly)
sleep 1
pkill -9 can_to_kuksa 2>/dev/null || true
pkill -9 HelloQt6Qml 2>/dev/null || true
sleep 1

# -------- SAVE PREVIOUS VERSION --------
PREVIOUS_VERSION=$(cat /etc/ota-version 2>/dev/null || echo "none")
log "[5/10] Previous version: $PREVIOUS_VERSION"

# -------- ATOMIC SYMLINK SWITCH --------
log "[6/10] Performing atomic symlink switch..."

# Handle legacy: if current is a directory (not symlink), remove it first
if [ -d "$CURRENT_LINK" ] && [ ! -L "$CURRENT_LINK" ]; then
    log "Converting legacy directory to symlink..."
    rm -rf "$CURRENT_LINK"
fi

# Create new symlink atomically using rename
TEMP_LINK="$WORKDIR/current.new"
rm -f "$TEMP_LINK"
ln -s "$RELEASE_DIR" "$TEMP_LINK"
mv -Tf "$TEMP_LINK" "$CURRENT_LINK" 2>/dev/null || {
    # Fallback for systems without mv -T
    rm -f "$CURRENT_LINK"
    ln -s "$RELEASE_DIR" "$CURRENT_LINK"
}

log "Symlink updated: $CURRENT_LINK -> $RELEASE_DIR"

# -------- VERIFY BINARY ARCHITECTURE --------
log "[7/10] Verifying binary architecture..."

SYSTEM_ARCH=$(uname -m)
ARCH_OK=1

verify_binary_arch() {
    local binary="$1"
    local name="$2"
    
    if [ ! -f "$binary" ]; then
        log "WARN: $name not found in package"
        return 0
    fi
    
    local file_info=$(file "$binary" 2>/dev/null || echo "unknown")
    
    case "$SYSTEM_ARCH" in
        aarch64)
            if echo "$file_info" | grep -q "64-bit.*ARM\|ARM aarch64"; then
                log "$name: architecture OK (64-bit ARM)"
                return 0
            elif echo "$file_info" | grep -q "32-bit.*ARM"; then
                log "ERROR: $name is 32-bit but system is 64-bit (aarch64)"
                return 1
            fi
            ;;
        armv7l|armhf)
            if echo "$file_info" | grep -q "32-bit.*ARM"; then
                log "$name: architecture OK (32-bit ARM)"
                return 0
            elif echo "$file_info" | grep -q "64-bit"; then
                log "ERROR: $name is 64-bit but system is 32-bit (armv7)"
                return 1
            fi
            ;;
    esac
    
    log "WARN: Could not verify $name architecture: $file_info"
    return 0
}

# Verify binaries based on platform
case "$PLATFORM" in
    rpi5)
        # RPi5: Only verify KUKSA binary
        if [ -f "$CURRENT_LINK/kuksa/bin/can_to_kuksa_publisher" ]; then
            if ! verify_binary_arch "$CURRENT_LINK/kuksa/bin/can_to_kuksa_publisher" "can_to_kuksa_publisher"; then
                ARCH_OK=0
            fi
        fi
        ;;
    rpi4)
        # RPi4: Only verify Cluster binary
        if [ -f "$CURRENT_LINK/cluster/HelloQt6Qml" ]; then
            if ! verify_binary_arch "$CURRENT_LINK/cluster/HelloQt6Qml" "HelloQt6Qml"; then
                ARCH_OK=0
            fi
        fi
        ;;
    *)
        # Combined: Verify all binaries
        if [ -f "$CURRENT_LINK/kuksa/bin/can_to_kuksa_publisher" ]; then
            if ! verify_binary_arch "$CURRENT_LINK/kuksa/bin/can_to_kuksa_publisher" "can_to_kuksa_publisher"; then
                ARCH_OK=0
            fi
        fi
        if [ -f "$CURRENT_LINK/cluster/HelloQt6Qml" ]; then
            if ! verify_binary_arch "$CURRENT_LINK/cluster/HelloQt6Qml" "HelloQt6Qml"; then
                ARCH_OK=0
            fi
        fi
        ;;
esac

if [ $ARCH_OK -eq 0 ]; then
    log "ERROR: Binary architecture mismatch detected!"
    log "System architecture: $SYSTEM_ARCH"
    log "Aborting update to prevent installing incompatible binaries."
    log "Please ensure OTA package is built for $SYSTEM_ARCH"
    exit 1
fi

# -------- INSTALL BINARIES --------
log "[8/10] Installing binaries for $PLATFORM..."

# Install based on platform
case "$PLATFORM" in
    rpi5)
        # RPi5: Only install KUKSA publisher
        if [ -f "$CURRENT_LINK/kuksa/bin/can_to_kuksa_publisher" ]; then
            cp "$CURRENT_LINK/kuksa/bin/can_to_kuksa_publisher" "$KUKSA_DIR/bin/"
            chmod +x "$KUKSA_DIR/bin/can_to_kuksa_publisher"
            log "Installed: can_to_kuksa_publisher"
        fi
        
        if [ -f "$CURRENT_LINK/kuksa/vss_min.json" ]; then
            cp "$CURRENT_LINK/kuksa/vss_min.json" "$KUKSA_DIR/"
            log "Installed: vss_min.json"
        fi
        ;;
    rpi4)
        # RPi4: Only install Qt Cluster
        if [ -f "$CURRENT_LINK/cluster/HelloQt6Qml" ]; then
            cp "$CURRENT_LINK/cluster/HelloQt6Qml" "$CLUSTER_DIR/"
            chmod +x "$CLUSTER_DIR/HelloQt6Qml"
            log "Installed: HelloQt6Qml"
        fi
        ;;
    *)
        # Combined: Install everything available
        if [ -f "$CURRENT_LINK/kuksa/bin/can_to_kuksa_publisher" ]; then
            cp "$CURRENT_LINK/kuksa/bin/can_to_kuksa_publisher" "$KUKSA_DIR/bin/"
            chmod +x "$KUKSA_DIR/bin/can_to_kuksa_publisher"
            log "Installed: can_to_kuksa_publisher"
        fi
        
        if [ -f "$CURRENT_LINK/kuksa/vss_min.json" ]; then
            cp "$CURRENT_LINK/kuksa/vss_min.json" "$KUKSA_DIR/"
            log "Installed: vss_min.json"
        fi
        
        if [ -f "$CURRENT_LINK/cluster/HelloQt6Qml" ]; then
            cp "$CURRENT_LINK/cluster/HelloQt6Qml" "$CLUSTER_DIR/"
            chmod +x "$CLUSTER_DIR/HelloQt6Qml"
            log "Installed: HelloQt6Qml"
        fi
        ;;
esac

# -------- START SERVICES --------
log "[9/10] Starting services for $PLATFORM..."

case "$PLATFORM" in
    rpi5)
        systemctl start can-to-kuksa.service 2>/dev/null || log "WARN: can-to-kuksa.service not found"
        ;;
    rpi4)
        systemctl start cluster.service 2>/dev/null || log "WARN: cluster.service not found"
        ;;
    *)
        systemctl start can-to-kuksa.service 2>/dev/null || log "WARN: can-to-kuksa.service not found"
        systemctl start cluster.service 2>/dev/null || log "WARN: cluster.service not found"
        ;;
esac

# -------- HEALTH CHECK --------
log "[10/10] Performing health check..."

# Function to check if service is healthy (not in restart loop)
check_service_health() {
    local service="$1"
    local max_restarts=3
    local check_interval=2
    local total_wait=10
    
    if ! systemctl is-enabled "$service" 2>/dev/null; then
        log "SKIP: $service is not enabled"
        return 0
    fi
    
    log "Checking $service health..."
    
    # Wait a bit for service to stabilize
    sleep $check_interval
    
    # Get initial restart count
    local initial_restarts=$(systemctl show "$service" --property=NRestarts --value 2>/dev/null || echo "0")
    
    # Wait and check for restart loop
    local elapsed=0
    while [ $elapsed -lt $total_wait ]; do
        sleep $check_interval
        elapsed=$((elapsed + check_interval))
        
        local current_restarts=$(systemctl show "$service" --property=NRestarts --value 2>/dev/null || echo "0")
        local restart_diff=$((current_restarts - initial_restarts))
        
        if [ $restart_diff -ge $max_restarts ]; then
            log "ERROR: $service is in restart loop ($restart_diff restarts in ${elapsed}s)"
            return 1
        fi
        
        if systemctl is-active --quiet "$service"; then
            log "$service: active and stable (restarts: $restart_diff)"
            return 0
        fi
    done
    
    # Final check
    if systemctl is-active --quiet "$service"; then
        log "$service: active"
        return 0
    else
        local status=$(systemctl is-active "$service" 2>/dev/null || echo "unknown")
        log "ERROR: $service is not running (status: $status)"
        return 1
    fi
}

FAILED=0

# Check services based on platform
case "$PLATFORM" in
    rpi5)
        if ! check_service_health "can-to-kuksa.service"; then
            log "CRITICAL: can-to-kuksa.service failed health check"
            FAILED=1
        fi
        ;;
    rpi4)
        if ! check_service_health "cluster.service"; then
            log "CRITICAL: cluster.service failed health check"
            FAILED=1
        fi
        ;;
    *)
        if ! check_service_health "can-to-kuksa.service"; then
            log "CRITICAL: can-to-kuksa.service failed health check"
            FAILED=1
        fi
        if ! check_service_health "cluster.service"; then
            log "CRITICAL: cluster.service failed health check"
            FAILED=1
        fi
        ;;
esac

# -------- ROLLBACK IF FAILED --------
if [ $FAILED -eq 1 ] && [ "$PREVIOUS_VERSION" != "none" ]; then
    log "Rolling back to $PREVIOUS_VERSION..."
    
    systemctl stop can-to-kuksa.service 2>/dev/null || true
    systemctl stop cluster.service 2>/dev/null || true
    
    # Atomic rollback - just switch symlink back
    PREV_RELEASE_DIR="$RELEASES_DIR/$PREVIOUS_VERSION"
    if [ -d "$PREV_RELEASE_DIR" ]; then
        rm -f "$CURRENT_LINK"
        ln -s "$PREV_RELEASE_DIR" "$CURRENT_LINK"
        
        # Re-install previous binaries
        [ -f "$CURRENT_LINK/kuksa/bin/can_to_kuksa_publisher" ] && \
            cp "$CURRENT_LINK/kuksa/bin/can_to_kuksa_publisher" "$KUKSA_DIR/bin/"
        [ -f "$CURRENT_LINK/kuksa/vss_min.json" ] && \
            cp "$CURRENT_LINK/kuksa/vss_min.json" "$KUKSA_DIR/"
        [ -f "$CURRENT_LINK/cluster/HelloQt6Qml" ] && \
            cp "$CURRENT_LINK/cluster/HelloQt6Qml" "$CLUSTER_DIR/"
        
        systemctl start can-to-kuksa.service 2>/dev/null || true
        systemctl start cluster.service 2>/dev/null || true
        
        echo "$PREVIOUS_VERSION" > /etc/ota-version
        log "Rollback to $PREVIOUS_VERSION complete"
    else
        log "ERROR: Previous version directory not found, cannot rollback"
    fi
    
    exit 1
fi

# -------- SUCCESS --------
echo "$VERSION" > /etc/ota-version
log "=== Update to $VERSION successful ==="
log ""
log "Platform: $PLATFORM"
log "Installed from: $CURRENT_LINK"
log "Binaries installed:"
case "$PLATFORM" in
    rpi5)
        log "  - $KUKSA_DIR/bin/can_to_kuksa_publisher"
        log "  - $KUKSA_DIR/vss_min.json"
        ;;
    rpi4)
        log "  - $CLUSTER_DIR/HelloQt6Qml"
        ;;
    *)
        log "  - $KUKSA_DIR/bin/can_to_kuksa_publisher"
        log "  - $KUKSA_DIR/vss_min.json"
        log "  - $CLUSTER_DIR/HelloQt6Qml"
        ;;
esac
log ""
log "Available versions for rollback:"
ls -1 "$RELEASES_DIR" | tee -a "$LOG"
