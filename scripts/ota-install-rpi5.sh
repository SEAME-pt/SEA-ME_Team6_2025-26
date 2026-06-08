#!/bin/bash
# OTA install script for RPi5 — runs on the car after downloading the release package.
# Usage: sudo bash ota-install-rpi5.sh

set -e

ADAS_DIR="/data/ADAS-Manager-GITHUB"
INFERENCE_DIR="/opt/seame/adas/inference"
OTA_DIR="/opt/seame/ota"
SERVICE_DIR="/etc/systemd/system"
BACKUP_DIR="/opt/seame/backup"

echo "=== SEAME OTA Install (RPi5) ==="

mkdir -p "$ADAS_DIR" "$INFERENCE_DIR" "$OTA_DIR" "$BACKUP_DIR"

# ── Backup current binaries ───────────────────────────────────────────────────
[ -f "$ADAS_DIR/adas_manager" ] && \
    cp "$ADAS_DIR/adas_manager" "$BACKUP_DIR/adas_manager.bak" && \
    echo "Backed up adas_manager"

# ── Stop services ─────────────────────────────────────────────────────────────
echo "Stopping services..."
systemctl stop inference.service   2>/dev/null || true
systemctl stop adas-manager.service 2>/dev/null || true

# ── Install ADAS Manager ──────────────────────────────────────────────────────
echo "Installing ADAS Manager..."
install -m 755 adas-manager/adas_manager      "$ADAS_DIR/adas_manager"
install -m 644 adas-manager/kuksa_bridge.py   "$ADAS_DIR/kuksa_bridge.py"
install -m 644 adas-manager/socket_sender.py  "$ADAS_DIR/socket_sender.py"
# Only copy config if it doesn't already exist (preserve tuned values)
[ ! -f "$ADAS_DIR/lka_config.conf" ] && \
    install -m 644 adas-manager/lka_config.conf "$ADAS_DIR/lka_config.conf" && \
    echo "Installed default lka_config.conf"

# ── Install Inference ─────────────────────────────────────────────────────────
echo "Installing inference scripts..."
cp -r inference/. "$INFERENCE_DIR/"

# ── Install service files ─────────────────────────────────────────────────────
echo "Installing systemd services..."
install -m 644 services/adas-manager.service "$SERVICE_DIR/adas-manager.service"
install -m 644 services/inference.service    "$SERVICE_DIR/inference.service"

systemctl daemon-reload
systemctl enable adas-manager.service inference.service

# ── Start and verify ──────────────────────────────────────────────────────────
echo "Starting adas-manager..."
systemctl start adas-manager.service
sleep 4

if systemctl is-active --quiet adas-manager.service; then
    echo "✅ adas-manager running"
else
    echo "❌ adas-manager failed — rolling back"
    [ -f "$BACKUP_DIR/adas_manager.bak" ] && \
        cp "$BACKUP_DIR/adas_manager.bak" "$ADAS_DIR/adas_manager"
    systemctl start adas-manager.service 2>/dev/null || true
    exit 1
fi

echo "Starting inference..."
systemctl start inference.service
sleep 4

if systemctl is-active --quiet inference.service; then
    echo "✅ inference running"
else
    echo "⚠️  inference failed to start (non-fatal — check: journalctl -u inference)"
fi

# ── Install OTA check script ──────────────────────────────────────────────────
echo "Installing OTA check..."
install -m 755 ota/ota-check.sh               "$OTA_DIR/ota-check.sh"
install -m 644 services/ota-check.service     "$SERVICE_DIR/ota-check.service"
systemctl daemon-reload
systemctl enable ota-check.service

echo ""
echo "=== Install complete ==="
echo "  adas-manager: $(systemctl is-active adas-manager.service)"
echo "  inference:    $(systemctl is-active inference.service)"
echo "  ota-check:    enabled (runs on next boot)"
