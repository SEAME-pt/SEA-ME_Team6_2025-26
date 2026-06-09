#!/bin/bash
# OTA check — runs on boot via systemd.
# Checks GitHub for a new release, downloads and installs if newer.

set -e

REPO="SEAME-pt/SEA-ME_Team6_2025-26"
BUNDLE_ASSET_PREFIX="apps-"
BUNDLE_EXT=".raucb"
HASH_ASSET_PREFIX="hash-rauc-rpi5-"
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

# ── Download RAUC bundle ──────────────────────────────────────────────────────
log "New version found: $CURRENT → $LATEST"

BUNDLE_FILE="${BUNDLE_ASSET_PREFIX}${LATEST}${BUNDLE_EXT}"
HASH_FILE="${HASH_ASSET_PREFIX}${LATEST}.txt"
BUNDLE_URL="https://github.com/${REPO}/releases/download/${LATEST}/${BUNDLE_FILE}"
HASH_URL="https://github.com/${REPO}/releases/download/${LATEST}/${HASH_FILE}"

log "Downloading $BUNDLE_FILE..."

rm -rf "$WORK_DIR"
mkdir -p "$WORK_DIR"

curl -fsSL "$BUNDLE_URL" -o "$WORK_DIR/apps.raucb"
curl -fsSL "$HASH_URL"   -o "$WORK_DIR/hash.txt"

# ── Verify checksum ───────────────────────────────────────────────────────────
log "Verifying checksum..."
cd "$WORK_DIR"
sha256sum -c hash.txt
log "Checksum OK"

# ── Install via RAUC ──────────────────────────────────────────────────────────
log "Installing RAUC bundle..."
rauc install "$WORK_DIR/apps.raucb"

# ── Save installed version ────────────────────────────────────────────────────
mkdir -p "$(dirname "$VERSION_FILE")"
echo "$LATEST" > "$VERSION_FILE"
log "Updated version file: $LATEST"

# ── Cleanup old versions (keep last 2) ───────────────────────────────────────
ls -dt /data/apps/v* 2>/dev/null | tail -n +3 | xargs rm -rf 2>/dev/null || true

# ── Cleanup ───────────────────────────────────────────────────────────────────
rm -rf "$WORK_DIR"
log "Done — running $LATEST"
