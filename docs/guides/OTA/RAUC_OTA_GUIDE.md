# RAUC OTA Update Guide — App Layer

## Overview

RAUC delivers signed, atomic app-only updates to the RPi5 (AGL) without touching the OS/rootfs. On boot, the car installs any pending update automatically. Every 5 minutes, the car checks for new releases and notifies the cluster via KUKSA if one is available. The user installs by rebooting.

RPi4 (cluster) is updated as part of the same bundle — RPi5 deploys `ClusterApp` to RPi4 via SSH after its own update succeeds.

---

## Architecture

```
Developer                    GitHub                        Car (RPi5)                RPi4
──────────                   ──────                        ──────────                ────
git tag v1.x.x
    │
    ▼
git push ──────────────► GitHub Actions
                               │
                               ├─ Build adas_manager (ARM64)
                               ├─ Build ClusterApp (ARM32)
                               ├─ Package apps.tar.gz
                               │    adas_manager
                               │    kuksa_bridge.py
                               │    socket_sender.py
                               │    inference/
                               │    cluster/ClusterApp
                               │
                               ├─ Sign bundle with CA key
                               └─ Upload apps-v1.x.x.raucb ──► GitHub Release
```

---

## Boot Flow (install)

```
Boot
 └─► wifi-startup.service
      └─► ota-check.service (--install)
               │
               ├─ GitHub API (retry 5x, 10s apart)
               │   latest == current? ──► exit 0
               │   latest > current?  ──► continue
               │
               ├─ curl apps-v1.x.x.raucb + hash
               ├─ sha256sum verify
               └─ rauc install
                        │
                        ├─ Verify signature (ca.cert.pem)
                        └─ hook.sh slot-install
                                 │
                                 ├─ Extract apps.tar.gz → /data/apps/v1.x.x/
                                 ├─ ln -sfn /data/apps/v1.x.x /data/current
                                 ├─ systemctl restart adas-manager
                                 ├─ sleep 10 (watchdog)
                                 │
                                 ├─ adas-manager OK? ──► start inference
                                 │                        │
                                 │                        └─ ClusterApp in bundle?
                                 │                              │
                                 │                        stop helloqt-app (RPi4)
                                 │                        scp ClusterApp → RPi4
                                 │                        restart helloqt-app
                                 │                        sleep 5 (watchdog)
                                 │                        OK? ──► done ✅
                                 │                        FAIL? ─► restore .bak ↩
                                 │
                                 └─ adas-manager FAIL? ─► rollback
                                                          ln -sfn PREV /data/current
                                                          restart old version ↩
```

---

## Notify Flow (every 5 minutes)

```
ota-check.timer (OnBootSec=2min, OnUnitActiveSec=5min)
 └─► ota-check-notify.service (--notify)
          │
          ├─ GitHub API → latest > current?
          │   No  ──► exit 0
          │   Yes ──► write /opt/seame/pending_version
          │            publish Vehicle.OTA.UpdateAvailable = true
          │            publish Vehicle.OTA.PendingVersion  = v1.x.x
          │
          └─► Cluster shows notification
               User reboots → boot flow installs
```

---

## Directory Structure on Car

```
/data/
├── apps/
│   ├── v1.0.0/                ← previous version (kept for rollback)
│   │   ├── adas_manager
│   │   ├── kuksa_bridge.py
│   │   ├── socket_sender.py
│   │   ├── inference/
│   │   └── cluster/ClusterApp
│   └── v1.0.1/                ← active version
│       ├── adas_manager
│       ├── kuksa_bridge.py
│       ├── socket_sender.py
│       ├── inference/
│       └── cluster/ClusterApp
├── current -> /data/apps/v1.0.1/   ← symlink (services always use this)
└── rauc-slot.img                   ← dummy slot file (required by RAUC)

/opt/seame/
├── version                    ← installed version (e.g. v1.0.1)
└── pending_version            ← available version not yet installed
```

Services always reference `/data/current/`. To rollback manually: update the symlink and restart services.

---

## KUKSA Signals

| Signal | Type | Description |
|--------|------|-------------|
| `Vehicle.OTA.UpdateAvailable` | boolean | `true` when new version detected |
| `Vehicle.OTA.PendingVersion` | string | Version available but not installed |
| `Vehicle.OTA.InstalledVersion` | string | Currently installed version |

---

## Components

### On Car

| File | Location | Purpose |
|------|----------|---------|
| `system.conf` | `/etc/rauc/system.conf` | RAUC slot configuration |
| `ca.cert.pem` | `/etc/rauc/ca.cert.pem` | Certificate for signature verification |
| `bootloader-noop.sh` | `/usr/lib/rauc/bootloader-noop.sh` | No-op bootloader backend (required by RAUC) |
| `ota-check.sh` | `/opt/seame/ota/ota-check.sh` | Version check + download + install script |
| `kuksa_ota_notify.py` | `/opt/seame/ota/kuksa_ota_notify.py` | Publishes OTA signals to KUKSA |
| `ota-check.service` | `/etc/systemd/system/` | Runs `--install` on boot |
| `ota-check-notify.service` | `/etc/systemd/system/` | Runs `--notify` via timer |
| `ota-check.timer` | `/etc/systemd/system/` | Triggers notify every 5min |

### In Repo

| File | Location | Purpose |
|------|----------|---------|
| `manifest.raucm` | `scripts/rauc-bundle/` | Bundle manifest template (`@@VERSION@@` placeholder) |
| `hook.sh` | `scripts/rauc-bundle/` | Install hook: extract, symlink, watchdog, rollback, ClusterApp deploy |
| `bootloader-noop.sh` | `scripts/rauc-bundle/` | No-op bootloader backend source |
| `rauc-system.conf` | `docs/guides/OTA/` | system.conf to deploy on car |
| `ota-check.sh` | `scripts/` | OTA check script source |
| `ota-check-notify.service` | `scripts/` | Notify service unit |
| `ota-check.service` | `scripts/` | Install service unit |
| `ota-check.timer` | `src/ota/systemd/` | Timer unit |
| `kuksa_ota_notify.py` | `scripts/` | KUKSA notification script |
| `vss.json` | `src/kuksa/` | VSS signal definitions including `Vehicle.OTA.*` |
| `ota.yml` | `.github/workflows/` | CI/CD: build + sign + upload bundle |

---

## RAUC System Configuration

`/etc/rauc/system.conf` on car:

```ini
[system]
compatible=seame-rpi5
bootloader=custom

[keyring]
path=/etc/rauc/ca.cert.pem

[handlers]
bootloader-custom-backend=/usr/lib/rauc/bootloader-noop.sh

[slot.rootfs.0]
device=/dev/mmcblk0p2
type=ext4
bootname=A

[slot.appfs.0]
device=/data/rauc-slot.img
type=raw
```

> `rootfs.0` is defined only so RAUC can identify the booted slot. It is never updated.  
> `appfs.0` uses a dummy raw file — actual installation is handled entirely by `hook.sh`.  
> No real bootloader is used — `bootloader-noop.sh` satisfies RAUC's requirement without doing anything.

---

## Bundle Structure

```
apps-v1.x.x.raucb
├── manifest.raucm          ← version, compatible, hook declaration
├── hook.sh                 ← install hook
└── apps.tar.gz
    ├── adas_manager
    ├── kuksa_bridge.py
    ├── socket_sender.py
    ├── inference/
    └── cluster/ClusterApp  ← only present when platform=both
```

---

## Rollback

**Automatic — RPi5** (watchdog in hook.sh):
- Triggered if `adas-manager` fails within 10s of start after update
- Reverts symlink to previous version, restarts services with old version

**Automatic — RPi4** (watchdog in hook.sh):
- Triggered if `helloqt-app.service` fails within 5s after ClusterApp update
- Restores `ClusterApp.bak` and restarts service

**Manual — RPi5**:
```bash
systemctl stop adas-manager inference
ln -sfn /data/apps/v1.0.0 /data/current
systemctl start adas-manager inference
```

**Manual — RPi4**:
```bash
ssh -i /root/.ssh/id_cluster root@10.21.220.192 \
  "mv /home/ClusterApp.bak /home/ClusterApp && systemctl restart helloqt-app.service"
```

---

## Certificate Setup (One-Time)

```bash
# Generate CA key and certificate (on developer machine)
openssl genrsa -out ca.key.pem 4096
openssl req -new -x509 -key ca.key.pem -out ca.cert.pem -days 3650 \
  -subj "/CN=SEAME Team6 RAUC CA"

# ca.key.pem → add to GitHub Secrets as RAUC_SIGNING_KEY (never commit)
# ca.cert.pem → commit to repo at src/ota/rauc/ca.cert.pem
# ca.cert.pem → deploy to car at /etc/rauc/ca.cert.pem
```

---

## Car Setup (One-Time)

```bash
# 1. Deploy RAUC config
cp /path/to/rauc-system.conf /etc/rauc/system.conf
cp /path/to/ca.cert.pem /etc/rauc/ca.cert.pem

# 2. Deploy no-op bootloader backend
cp /path/to/bootloader-noop.sh /usr/lib/rauc/bootloader-noop.sh
chmod +x /usr/lib/rauc/bootloader-noop.sh

# 3. Create dummy slot file
dd if=/dev/zero of=/data/rauc-slot.img bs=1M count=1

# 4. Create initial versioned directory
mkdir -p /data/apps/v0.0.0
cp <current_binaries> /data/apps/v0.0.0/
ln -sfn /data/apps/v0.0.0 /data/current

# 5. Deploy OTA scripts
mkdir -p /opt/seame/ota
cp ota-check.sh /opt/seame/ota/
cp kuksa_ota_notify.py /opt/seame/ota/
chmod +x /opt/seame/ota/ota-check.sh

# 6. Deploy and enable systemd units
cp ota-check.service ota-check-notify.service ota-check.timer /etc/systemd/system/
systemctl daemon-reload
systemctl enable --now ota-check.timer
systemctl enable ota-check.service

# 7. Setup SSH key for RPi4 access
ssh-keygen -t ed25519 -f /root/.ssh/id_cluster -N ""
ssh-copy-id -i /root/.ssh/id_cluster.pub root@10.21.220.192
```

---

## Releasing an Update

```bash
git tag v1.x.x
git push origin v1.x.x
```

GitHub Actions builds the bundle automatically (both RPi5 and RPi4). Car installs on next reboot.

For RPi5-only update (no ClusterApp):
```bash
# workflow_dispatch with platform=rpi5
```

---

## Monitoring

```bash
# Check OTA service logs
journalctl -u ota-check --no-pager

# Check RAUC install logs (hook output)
journalctl -u rauc --no-pager

# Check timer status
systemctl list-timers | grep ota

# Check installed version
cat /opt/seame/version

# Check current symlink
readlink /data/current

# Check ClusterApp on RPi4
ssh -i /root/.ssh/id_cluster root@10.21.220.192 \
  "ls -la /home/ClusterApp && systemctl status helloqt-app.service"
```

---

## Notes

- `lka_config.conf` is NOT included in the bundle — tuned values on car are preserved
- Hook keeps last 2 versions in `/data/apps/`, older ones deleted automatically
- `RAUC_SIGNING_KEY` must be set in GitHub repository secrets before first release
- RAUC on car (1.15.1) calls install hooks as `slot-install`; bundle manifest uses `install` (RAUC 1.5 on runner) — hook handles both
- WiFi takes ~15s to connect on boot; OTA service retries GitHub API 5 times with 10s delay to compensate
- ClusterApp deployment requires `platform=both` build (tag push triggers this automatically)
