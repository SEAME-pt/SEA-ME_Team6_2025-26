# RAUC OTA Update Guide — App Layer (RPi5)

## Overview

RAUC is used to deliver signed, atomic app-only updates to the RPi5 (AGL) without rebooting or touching the OS/rootfs. Updates are triggered automatically on boot and every 15 minutes via a systemd timer.

---

## Update Flow

```
Developer                    GitHub                        Car (RPi5)
──────────                   ──────                        ──────────
git tag v1.x.x
    │
    ▼
git push origin v1.x.x ──► GitHub Actions
                               │
                               ├─ Build adas_manager (ARM64)
                               ├─ Package apps.tar.gz
                               │    adas_manager
                               │    kuksa_bridge.py
                               │    socket_sender.py
                               │    inference/
                               │
                               ├─ Sign bundle with CA key
                               │    rauc bundle --cert --key
                               │
                               └─ Upload apps-v1.x.x.raucb ──► GitHub Release
                                                                      │
                                                           (boot + every 15min)
                                                                      │
                                                              ota-check.service
                                                                      │
                                                           ┌──────────▼──────────┐
                                                           │  GitHub API check   │
                                                           │  latest > current?  │
                                                           └──────────┬──────────┘
                                                                No    │ Yes
                                                              exit 0  │
                                                                      ▼
                                                           Download apps-v1.x.x.raucb
                                                           Verify sha256 checksum
                                                                      │
                                                                      ▼
                                                              rauc install
                                                                      │
                                                           ┌──────────▼──────────┐
                                                           │  Verify signature   │
                                                           │  (ca.cert.pem)      │
                                                           └──────────┬──────────┘
                                                                      │
                                                                      ▼
                                                           hook.sh slot-install
                                                           ├─ Extract tar → /data/apps/v1.x.x/
                                                           ├─ ln -sfn /data/apps/v1.x.x /data/current
                                                           ├─ systemctl restart adas-manager
                                                           ├─ sleep 10 (watchdog)
                                                           │
                                                           ├─ adas-manager OK? ──► start inference
                                                           │                        Update done ✅
                                                           │
                                                           └─ adas-manager FAIL? ─► rollback
                                                                                    ln -sfn PREV /data/current
                                                                                    restart old version ↩
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
│   │   ├── lka_config.conf
│   │   └── inference/
│   └── v1.0.1/                ← active version
│       ├── adas_manager
│       ├── kuksa_bridge.py
│       ├── socket_sender.py
│       └── inference/
├── current -> /data/apps/v1.0.1/   ← symlink (services always use this)
└── rauc-slot.img                   ← dummy slot file (required by RAUC)
```

Services always reference `/data/current/`. To rollback manually: update the symlink and restart services.

---

## Components

### On Car

| File | Location | Purpose |
|------|----------|---------|
| `system.conf` | `/etc/rauc/system.conf` | RAUC slot configuration |
| `ca.cert.pem` | `/etc/rauc/ca.cert.pem` | Certificate for signature verification |
| `bootloader-noop.sh` | `/usr/lib/rauc/bootloader-noop.sh` | No-op bootloader backend (required by RAUC) |
| `ota-check.sh` | `/opt/seame/ota/ota-check.sh` | Version check + download + install script |
| `ota-check.service` | `/etc/systemd/system/ota-check.service` | Runs ota-check.sh, waits for network |
| `ota-check.timer` | `/usr/lib/systemd/system/ota-check.timer` | Triggers service on boot + every 15min |

### In Repo

| File | Purpose |
|------|---------|
| `scripts/rauc-bundle/manifest.raucm` | Bundle manifest template (`@@VERSION@@` placeholder) |
| `scripts/rauc-bundle/hook.sh` | Install hook: extract, symlink, watchdog, rollback |
| `scripts/rauc-bundle/bootloader-noop.sh` | No-op bootloader backend source |
| `docs/guides/OTA/rauc-system.conf` | system.conf to deploy on car |
| `scripts/ota-check.sh` | OTA check script source |
| `.github/workflows/ota.yml` | CI/CD: build + sign + upload bundle |

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

---

## Bundle Structure

```
apps-v1.x.x.raucb
├── manifest.raucm      ← version, compatible, hook declaration
├── hook.sh             ← install hook (extract + symlink + watchdog)
└── apps.tar.gz         ← adas_manager + scripts + inference/
```

---

## Rollback

**Automatic** (watchdog in hook.sh):
- Triggered if `adas-manager` fails within 10s of start after update
- Reverts symlink to previous version
- Restarts services with old version

**Manual**:
```bash
systemctl stop adas-manager inference
ln -sfn /data/apps/v1.0.0 /data/current
systemctl start adas-manager inference
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

# 2. Deploy no-op bootloader backend
cp /path/to/bootloader-noop.sh /usr/lib/rauc/bootloader-noop.sh
chmod +x /usr/lib/rauc/bootloader-noop.sh

# 3. Create dummy slot file
dd if=/dev/zero of=/data/rauc-slot.img bs=1M count=1

# 4. Create initial versioned directory
mkdir -p /data/apps/v0.0.0
cp <current_binaries> /data/apps/v0.0.0/
ln -sfn /data/apps/v0.0.0 /data/current

# 5. Enable RAUC and OTA timer
systemctl enable rauc
systemctl enable --now ota-check.timer
```

---

## Releasing an Update

```bash
git tag v1.x.x
git push origin v1.x.x
```

GitHub Actions builds the bundle automatically. The car picks it up on the next timer trigger (boot + every 15min).

---

## Notes

- `lka_config.conf` is NOT included in the bundle — tuned values on car are preserved in `/data/apps/v0.0.0/`
- Old versions accumulate in `/data/apps/` — hook keeps last 2, older ones are deleted automatically
- `RAUC_SIGNING_KEY` must be set in GitHub repository secrets before first release
- RAUC on the car (1.15.1) calls install hooks as `slot-install` — bundle creation uses `install` in the manifest (RAUC 1.5 on runner)
