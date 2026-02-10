# 📡 OTA System - SEA:ME Team 6

Over-the-Air update system for the AGL-based vehicle platform.

## Directory Structure

```
src/ota/
├── install.sh              # Installation script for AGL
├── scripts/
│   ├── ota-update.sh       # Main OTA update script (Phase B)
│   └── ota-check.sh        # Automatic update check script
└── systemd/
    ├── cluster.service     # Qt Cluster Dashboard service
    ├── ota-check.service   # OTA check oneshot service
    └── ota-check.timer     # Timer for periodic checks
```

## Installation on AGL

1. Copy the `ota` folder to the Raspberry Pi 5:
```bash
scp -r src/ota root@<AGL_IP>:/tmp/
```

2. SSH into AGL and run the installer:
```bash
ssh root@<AGL_IP>
cd /tmp/ota
chmod +x install.sh
./install.sh
```

## Usage

### Manual Update
```bash
/opt/ota/ota-update.sh v1.5.0
```

### Check for Updates
```bash
/opt/ota/ota-check.sh
```

### Enable Automatic Updates
```bash
echo "enabled" > /etc/ota-auto-update
systemctl start ota-check.timer
```

### View Timer Status
```bash
systemctl list-timers | grep ota
```

### View Logs
```bash
# OTA check logs
journalctl -u ota-check -f

# Full OTA log
tail -f /opt/ota/logs/ota.log
```

## Features

### Phase A (Complete)
- ✅ Download from GitHub Releases
- ✅ SHA256 hash verification
- ✅ Backup before update
- ✅ Service restart
- ✅ Basic rollback

### Phase B (Current)
- ✅ Atomic symlink switching
- ✅ Automatic polling (systemd timer)
- ✅ Qt Cluster service
- ✅ Version history (multiple releases kept)
- ✅ Improved rollback

### Phase C (Planned)
- ⬜ RAUC integration
- ⬜ A/B partition switching
- ⬜ Signed updates

## Systemd Services

| Service | Description | Status |
|---------|-------------|--------|
| `can-to-kuksa.service` | KUKSA CAN publisher | Existing |
| `cluster.service` | Qt6 Dashboard UI | New |
| `ota-check.timer` | Periodic update check | New |

## Configuration

### Auto-update Toggle
```bash
# Enable
echo "enabled" > /etc/ota-auto-update

# Disable
echo "disabled" > /etc/ota-auto-update
```

### Check Interval
Edit `/etc/systemd/system/ota-check.timer`:
```ini
[Timer]
OnUnitActiveSec=15min  # Change this value
```

Then reload: `systemctl daemon-reload && systemctl restart ota-check.timer`

## Rollback

If an update fails, the system automatically rolls back. For manual rollback:

```bash
# List available versions
ls /opt/ota/releases/

# Rollback to specific version
/opt/ota/ota-update.sh v1.4.0
```

## Troubleshooting

### Service won't start
```bash
systemctl status cluster.service
journalctl -u cluster.service -n 50
```

### Download fails
```bash
# Check network
ping github.com

# Check GitHub API
curl -s https://api.github.com/repos/SEAME-pt/SEA-ME_Team6_2025-26/releases/latest
```

### Hash mismatch
```bash
# Re-download and verify manually
cd /opt/ota/downloads
curl -fL https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/releases/download/v1.5.0/update.tar.gz -o update.tar.gz
curl -fL https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/releases/download/v1.5.0/hash.txt -o hash.txt
sha256sum -c hash.txt
```
