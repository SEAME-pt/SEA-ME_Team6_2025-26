#!/bin/bash
# ============================================================
# OTA Check Script - SEA:ME Team 6
# Checks GitHub for new releases and triggers update if found
# ============================================================

set -e

REPO="SEAME-pt/SEA-ME_Team6_2025-26"
VERSION_FILE="/etc/ota-version"
LOG="/opt/ota/logs/ota-check.log"
LOCK_FILE="/tmp/ota-update.lock"

log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a "$LOG"
}

# Prevent concurrent runs
if [ -f "$LOCK_FILE" ]; then
    log "Another OTA check is running, exiting"
    exit 0
fi
trap "rm -f $LOCK_FILE" EXIT
touch "$LOCK_FILE"

# Get current version
CURRENT_VERSION=$(cat "$VERSION_FILE" 2>/dev/null || echo "unknown")
log "Current version: $CURRENT_VERSION"

# Get latest release from GitHub API
log "Checking GitHub for latest release..."
LATEST_RELEASE=$(curl -s "https://api.github.com/repos/$REPO/releases/latest" 2>/dev/null)

if [ -z "$LATEST_RELEASE" ] || echo "$LATEST_RELEASE" | grep -q "Not Found"; then
    log "ERROR: Could not fetch release info from GitHub"
    exit 1
fi

LATEST_VERSION=$(echo "$LATEST_RELEASE" | grep -o '"tag_name": "[^"]*"' | head -1 | cut -d'"' -f4)

if [ -z "$LATEST_VERSION" ]; then
    log "ERROR: Could not parse latest version"
    exit 1
fi

log "Latest version: $LATEST_VERSION"

# Compare versions
if [ "$CURRENT_VERSION" = "$LATEST_VERSION" ]; then
    log "Already up to date"
    exit 0
fi

# New version available!
log "New version available: $LATEST_VERSION (current: $CURRENT_VERSION)"

# Check if auto-update is enabled
AUTO_UPDATE_FILE="/etc/ota-auto-update"
if [ -f "$AUTO_UPDATE_FILE" ] && [ "$(cat $AUTO_UPDATE_FILE)" = "enabled" ]; then
    log "Auto-update is enabled, triggering update..."
    /opt/ota/ota-update.sh "$LATEST_VERSION"
else
    log "Auto-update is disabled. Run manually: /opt/ota/ota-update.sh $LATEST_VERSION"
    
    # Optional: Send notification (if notification system available)
    # notify-send "OTA Update Available" "Version $LATEST_VERSION is available"
fi

log "Check complete"
