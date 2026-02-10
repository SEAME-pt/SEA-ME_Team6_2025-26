#!/bin/bash
# ============================================================
# OTA Phase C Installation Script
# Run this on AGL (Raspberry Pi 5) to install OTA components
# ============================================================

set -e

echo "=== OTA Phase C Installation ==="

# Create directories
echo "[1/6] Creating directories..."
mkdir -p /opt/ota/{logs,downloads,releases}
mkdir -p /opt/cluster
mkdir -p /home/root/kuksa_RPi5/bin

# Install scripts
echo "[2/6] Installing OTA scripts..."
cp scripts/ota-update.sh /opt/ota/ota-update.sh
cp scripts/ota-check.sh /opt/ota/ota-check.sh
chmod +x /opt/ota/ota-update.sh
chmod +x /opt/ota/ota-check.sh

# Install systemd services
echo "[3/6] Installing systemd services..."
cp systemd/cluster.service /etc/systemd/system/
cp systemd/ota-check.service /etc/systemd/system/
cp systemd/ota-check.timer /etc/systemd/system/

# Reload systemd
echo "[4/6] Reloading systemd..."
systemctl daemon-reload

# Enable services
echo "[5/6] Enabling services..."
systemctl enable cluster.service || echo "WARN: cluster.service enable failed (may need display)"
systemctl enable ota-check.timer

# Configure auto-update (disabled by default)
echo "[6/6] Configuring auto-update..."
echo "disabled" > /etc/ota-auto-update
echo "To enable auto-updates: echo 'enabled' > /etc/ota-auto-update"

echo ""
echo "=== Installation Complete ==="
echo ""
echo "Services installed:"
echo "  - cluster.service (Qt Cluster Dashboard)"
echo "  - ota-check.service (OTA check script)"
echo "  - ota-check.timer (runs every 15 minutes)"
echo ""
echo "Commands:"
echo "  Start timer:    systemctl start ota-check.timer"
echo "  Check status:   systemctl list-timers"
echo "  Manual update:  /opt/ota/ota-update.sh v1.5.0"
echo "  View logs:      journalctl -u ota-check -f"
echo ""
