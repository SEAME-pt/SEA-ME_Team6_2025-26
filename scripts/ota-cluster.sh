#!/bin/bash
# Deploys ClusterApp update to RPi4 from RPi5 (hub-and-spoke OTA).
# Called by ota-check.sh --install after RAUC install succeeds.
# Usage: ota-cluster.sh <version>

set -e

VERSION="$1"
if [ -z "$VERSION" ]; then
    echo "[OTA-CLUSTER] ERROR: version required"
    exit 1
fi

REPO="SEAME-pt/SEA-ME_Team6_2025-26"
CLUSTER_IP="10.21.220.192"
CLUSTER_SSH_KEY="/root/.ssh/id_cluster"
CLUSTER_APP_PATH="/home/ClusterApp"
CLUSTER_SERVICE="helloqt-app.service"
CLUSTER_VERSION_FILE="/opt/seame/cluster-version"
WORK_DIR="/tmp/ota-cluster"

ASSET="update-rpi4.tar.gz"
HASH_ASSET="hash-rpi4.txt"
ASSET_URL="https://github.com/${REPO}/releases/download/${VERSION}/${ASSET}"
HASH_URL="https://github.com/${REPO}/releases/download/${VERSION}/${HASH_ASSET}"

log() { echo "[OTA-CLUSTER] $*"; }

# ── Check if cluster package exists in this release ───────────────────────────
if ! curl -fsSL --head "$ASSET_URL" > /dev/null 2>&1; then
    log "No cluster package in release $VERSION — skipping"
    exit 0
fi

log "Updating cluster to $VERSION..."

rm -rf "$WORK_DIR"
mkdir -p "$WORK_DIR"

curl -fsSL "$ASSET_URL" -o "$WORK_DIR/$ASSET"
curl -fsSL "$HASH_URL"  -o "$WORK_DIR/hash.txt"

log "Verifying checksum..."
cd "$WORK_DIR"
sha256sum -c hash.txt
log "Checksum OK"

tar -xzf "$ASSET" -C "$WORK_DIR"

CLUSTER_BIN=$(find "$WORK_DIR" -name "ClusterApp" -type f | head -1)
if [ -z "$CLUSTER_BIN" ]; then
    log "ClusterApp not found in package — skipping"
    exit 0
fi

chmod +x "$CLUSTER_BIN"

# ── Deploy to RPi4 ────────────────────────────────────────────────────────────
log "Copying ClusterApp to RPi4 ($CLUSTER_IP)..."
scp -i "$CLUSTER_SSH_KEY" "$CLUSTER_BIN" "root@${CLUSTER_IP}:${CLUSTER_APP_PATH}"

log "Restarting $CLUSTER_SERVICE on RPi4..."
ssh -i "$CLUSTER_SSH_KEY" "root@${CLUSTER_IP}" "systemctl restart ${CLUSTER_SERVICE}"

# ── Save version ──────────────────────────────────────────────────────────────
mkdir -p "$(dirname "$CLUSTER_VERSION_FILE")"
echo "$VERSION" > "$CLUSTER_VERSION_FILE"

rm -rf "$WORK_DIR"
log "Cluster updated to $VERSION"
