#!/bin/bash
# OTA check — runs on boot (--install) and every 5min (--notify).
#   --install : detects update, installs if pending flag set, else sets pending flag
#   --notify  : detects update, publishes to KUKSA (no install)

set -e

MODE="${1:---install}"
REPO="SEAME-pt/SEA-ME_Team6_2025-26"
BUNDLE_ASSET_PREFIX="apps-"
BUNDLE_EXT=".raucb"
HASH_ASSET_PREFIX="hash-rauc-rpi5-"
WORK_DIR="/tmp/ota-update"
VERSION_FILE="/opt/seame/version"
PENDING_FILE="/opt/seame/pending_version"
NOTIFY_SCRIPT="/opt/seame/ota/kuksa_ota_notify.py"
API_URL="https://api.github.com/repos/${REPO}/releases/latest"

log() { echo "[OTA] $*"; }

notify() { python3 "$NOTIFY_SCRIPT" "$@" 2>/dev/null || true; }

# ── Get latest release tag ────────────────────────────────────────────────────
log "Checking latest release..."
LATEST=""
for i in 1 2 3 4 5; do
    LATEST=$(curl -fsSL --max-time 10 "$API_URL" 2>/dev/null | grep '"tag_name"' | head -1 | cut -d'"' -f4)
    [ -n "$LATEST" ] && break
    log "GitHub API unreachable (attempt $i/5) — retrying in 10s..."
    sleep 10
done

if [ -z "$LATEST" ]; then
    log "Could not reach GitHub API after 5 attempts — skipping"
    exit 0
fi

log "Latest release: $LATEST"

# ── Compare with installed version ───────────────────────────────────────────
CURRENT=""
[ -f "$VERSION_FILE" ] && CURRENT=$(cat "$VERSION_FILE")
log "Installed version: ${CURRENT:-none}"

# Publish current version on boot
if [ "$MODE" = "--install" ] && [ -n "$CURRENT" ]; then
    notify --current "$CURRENT"
fi

if [ "$LATEST" = "$CURRENT" ]; then
    log "Already up to date — nothing to do"
    notify --current "$CURRENT"
    exit 0
fi

# ── New version detected ──────────────────────────────────────────────────────
log "New version found: ${CURRENT:-none} → $LATEST"

if [ "$MODE" = "--notify" ]; then
    # Just notify cluster — do not install
    echo "$LATEST" > "$PENDING_FILE"
    notify --available "$LATEST"
    log "Notified cluster — install on next boot"
    exit 0
fi

# ── MODE = --install ──────────────────────────────────────────────────────────
BUNDLE_FILE="${BUNDLE_ASSET_PREFIX}${LATEST}${BUNDLE_EXT}"
HASH_FILE="${HASH_ASSET_PREFIX}${LATEST}.txt"
BUNDLE_URL="https://github.com/${REPO}/releases/download/${LATEST}/${BUNDLE_FILE}"
HASH_URL="https://github.com/${REPO}/releases/download/${LATEST}/${HASH_FILE}"

log "Downloading $BUNDLE_FILE..."
rm -rf "$WORK_DIR"
mkdir -p "$WORK_DIR"

curl -fsSL "$BUNDLE_URL" -o "$WORK_DIR/$BUNDLE_FILE"
curl -fsSL "$HASH_URL"   -o "$WORK_DIR/hash.txt"

log "Verifying checksum..."
cd "$WORK_DIR"
sha256sum -c hash.txt
log "Checksum OK"

log "Installing RAUC bundle..."
rauc install "$WORK_DIR/$BUNDLE_FILE"

mkdir -p "$(dirname "$VERSION_FILE")"
echo "$LATEST" > "$VERSION_FILE"
rm -f "$PENDING_FILE"

notify --installed "$LATEST"

ls -dt /data/apps/v* 2>/dev/null | tail -n +3 | xargs rm -rf 2>/dev/null || true
rm -rf "$WORK_DIR"
log "Done — running $LATEST"
