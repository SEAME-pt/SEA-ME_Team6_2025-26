#!/bin/bash
# OTA check — runs on boot via systemd.
# Checks GitHub for a new release, downloads and installs if newer.

set -e

REPO="SEAME-pt/SEA-ME_Team6_2025-26"
ASSET="update-rpi5.tar.gz"
HASH_ASSET="hash-rpi5.txt"
WORK_DIR="/tmp/ota-update"
VERSION_FILE="/opt/seame/version"
API_URL="https://api.github.com/repos/${REPO}/releases/latest"

log() { echo "[OTA] $*"; }

# ── Get latest release tag from GitHub API ────────────────────────────────────
log "Checking latest release..."
LATEST=$(curl -fsSL "$API_URL" | grep '"tag_name"' | head -1 | cut -d'"' -f4)

if [ -z "$LATEST" ]; then
    log "Could not reach GitHub API — skipping update"
    exit 0
fi

log "Latest release: $LATEST"

# ── Compare with installed version ───────────────────────────────────────────
CURRENT=""
[ -f "$VERSION_FILE" ] && CURRENT=$(cat "$VERSION_FILE")

log "Installed version: ${CURRENT:-none}"

if [ "$LATEST" = "$CURRENT" ]; then
    log "Already up to date — nothing to do"
    exit 0
fi

# ── Download package ──────────────────────────────────────────────────────────
log "New version found: $CURRENT → $LATEST"
log "Downloading $ASSET..."

DOWNLOAD_URL="https://github.com/${REPO}/releases/download/${LATEST}/${ASSET}"
HASH_URL="https://github.com/${REPO}/releases/download/${LATEST}/${HASH_ASSET}"

rm -rf "$WORK_DIR"
mkdir -p "$WORK_DIR"

curl -fsSL "$DOWNLOAD_URL" -o "$WORK_DIR/$ASSET"
curl -fsSL "$HASH_URL"     -o "$WORK_DIR/$HASH_ASSET"

# ── Verify checksum ───────────────────────────────────────────────────────────
log "Verifying checksum..."
cd "$WORK_DIR"
sha256sum -c "$HASH_ASSET"
log "Checksum OK"

# ── Extract and install ───────────────────────────────────────────────────────
log "Extracting..."
tar -xzf "$ASSET"

log "Running install.sh..."
bash install.sh

# ── Save installed version ────────────────────────────────────────────────────
mkdir -p "$(dirname "$VERSION_FILE")"
echo "$LATEST" > "$VERSION_FILE"
log "Updated version file: $LATEST"

# ── Cleanup ───────────────────────────────────────────────────────────────────
rm -rf "$WORK_DIR"
log "Done — running $LATEST"
