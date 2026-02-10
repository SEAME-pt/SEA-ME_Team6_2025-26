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

# Target paths (symlinks to current version)
KUKSA_DIR="/home/root/kuksa_RPi5"
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

# -------- DOWNLOAD --------
log "[1/8] Downloading package from GitHub Release..."
RELEASE_URL="https://github.com/$REPO/releases/download/$VERSION"
curl -fL "$RELEASE_URL/update.tar.gz" -o "$DL" || { log "ERROR: Download failed"; exit 1; }
curl -fL "$RELEASE_URL/hash.txt" -o "$HASH_FILE" || { log "ERROR: Hash download failed"; exit 1; }

# -------- VERIFY HASH --------
log "[2/8] Verifying SHA256 hash..."
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
log "[3/8] Extracting to $RELEASE_DIR..."
rm -rf "$RELEASE_DIR"
mkdir -p "$RELEASE_DIR"
tar -xzf "$DL" -C "$RELEASE_DIR"

log "Package contents:"
find "$RELEASE_DIR" -type f | tee -a "$LOG"

# -------- STOP SERVICES --------
log "[4/8] Stopping services..."
systemctl stop can-to-kuksa.service 2>/dev/null || true
systemctl stop cluster.service 2>/dev/null || true

# -------- SAVE PREVIOUS VERSION --------
PREVIOUS_VERSION=$(cat /etc/ota-version 2>/dev/null || echo "none")
log "[5/8] Previous version: $PREVIOUS_VERSION"

# -------- ATOMIC SYMLINK SWITCH --------
log "[6/8] Performing atomic symlink switch..."

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

# -------- INSTALL BINARIES --------
log "[7/8] Installing binaries from current symlink..."

# KUKSA publisher
if [ -f "$CURRENT_LINK/kuksa/bin/can_to_kuksa_publisher" ]; then
    cp "$CURRENT_LINK/kuksa/bin/can_to_kuksa_publisher" "$KUKSA_DIR/bin/"
    chmod +x "$KUKSA_DIR/bin/can_to_kuksa_publisher"
    log "Installed: can_to_kuksa_publisher"
fi

if [ -f "$CURRENT_LINK/kuksa/vss_min.json" ]; then
    cp "$CURRENT_LINK/kuksa/vss_min.json" "$KUKSA_DIR/"
    log "Installed: vss_min.json"
fi

# Qt Cluster
if [ -f "$CURRENT_LINK/cluster/HelloQt6Qml" ]; then
    cp "$CURRENT_LINK/cluster/HelloQt6Qml" "$CLUSTER_DIR/"
    chmod +x "$CLUSTER_DIR/HelloQt6Qml"
    log "Installed: HelloQt6Qml"
fi

# -------- START SERVICES --------
log "[8/8] Starting services..."
systemctl start can-to-kuksa.service 2>/dev/null || log "WARN: can-to-kuksa.service not found"
systemctl start cluster.service 2>/dev/null || log "WARN: cluster.service not found"

# -------- HEALTH CHECK --------
sleep 3
FAILED=0

if systemctl is-enabled can-to-kuksa.service 2>/dev/null; then
    if ! systemctl is-active --quiet can-to-kuksa.service; then
        log "ERROR: can-to-kuksa.service failed to start"
        FAILED=1
    else
        log "can-to-kuksa.service: active"
    fi
fi

if systemctl is-enabled cluster.service 2>/dev/null; then
    if ! systemctl is-active --quiet cluster.service; then
        log "WARN: cluster.service failed to start"
        # Don't fail on cluster, it might need display
    else
        log "cluster.service: active"
    fi
fi

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
log "Installed from: $CURRENT_LINK"
log "Binaries:"
log "  - $KUKSA_DIR/bin/can_to_kuksa_publisher"
log "  - $KUKSA_DIR/vss_min.json"
log "  - $CLUSTER_DIR/HelloQt6Qml"
log ""
log "Available versions for rollback:"
ls -1 "$RELEASES_DIR" | tee -a "$LOG"
