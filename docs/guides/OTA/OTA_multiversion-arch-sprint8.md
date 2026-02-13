# OTA Multi-Version Architecture - Sprint 8

**Last Updated:** 13 February 2026

---

## Complete OTA Workflow (100% Automatic)

```
Developer                    GitHub                         Dispositivos
    │                           │                               │
    │  git tag v1.9.0           │                               │
    │  git push origin v1.9.0   │                               │
    │ ─────────────────────────►│                               │
    │                           │                               │
    │                     [Build & Release]                     │
    │                     (3-5 minutos)                         │
    │                           │                               │
    │                           │◄──────────────────────────────│
    │                           │    ota-check.timer            │
    │                           │    (cada 15 min)              │
    │                           │                               │
    │                           │  "Nova versão v1.9.0!"        │
    │                           │──────────────────────────────►│
    │                           │                               │
    │                           │                    [Auto-Update]
    │                           │                    [Health Check]
    │                           │                    [Rollback se falhar]
    │                           │                               │
    │                           │               ✅ Atualizado!   │
```

---

## System Architecture

```
┌─────────────────────────────────────┐         ┌─────────────────────────────────────┐
│            RPi4                     │         │           RPi5 (AGL)                │
│           (32-bit)                  │  WiFi/  │           (64-bit)                  │
│                                     │ Network │                                     │
│  ┌─────────────────────────────┐   │◄───────►│   ┌─────────────────────────────┐   │
│  │                             │   │         │   │                             │   │
│  │         Cluster             │   │         │   │          KUKSA              │   │
│  │        (Qt6 UI)             │   │         │   │        (CAN→VSS)            │   │
│  │                             │   │         │   │                             │   │
│  └─────────────────────────────┘   │         │   └─────────────────────────────┘   │
│                                     │         │                                     │
│  ┌─────────────────────────────┐   │         │   ┌─────────────────────────────┐   │
│  │     OTA: 32-bit binaries    │   │         │   │     OTA: 64-bit binaries    │   │
│  │     (armv7-raspberrypi4)    │   │         │   │     (aarch64-raspberrypi5)  │   │
│  └─────────────────────────────┘   │         │   └─────────────────────────────┘   │
│                                     │         │                                     │
│  ┌─────────────────────────────┐   │         │   ┌─────────────────────────────┐   │
│  │  ota-check.timer (15 min)   │   │         │   │  ota-check.timer (15 min)   │   │
│  │  auto-update: enabled       │   │         │   │  auto-update: enabled       │   │
│  └─────────────────────────────┘   │         │   └─────────────────────────────┘   │
│                                     │         │                                     │
└─────────────────────────────────────┘         └─────────────────────────────────────┘
                │                                               │
                │                                               │
                └───────────────────┬───────────────────────────┘
                                    │
                                    ▼
                    ┌───────────────────────────────┐
                    │                               │
                    │      GitHub Releases          │
                    │                               │
                    │  ┌─────────────────────────┐  │
                    │  │  v1.9.0                 │  │
                    │  │  ├── update-rpi4.tar.gz │  │
                    │  │  ├── hash-rpi4.txt      │  │
                    │  │  ├── update-rpi5.tar.gz │  │
                    │  │  ├── hash-rpi5.txt      │  │
                    │  │  ├── update.tar.gz      │  │
                    │  │  └── hash.txt           │  │
                    │  └─────────────────────────┘  │
                    │                               │
                    └───────────────────────────────┘
```

---

## Device Configuration (13 February 2026)

| Dispositivo | IP | Arquitetura | Plataforma | Timer | Auto-Update |
|-------------|-----|-------------|------------|-------|-------------|
| **RPi5** | 10.21.220.191 | `aarch64` | rpi5 | ✅ Ativo | ✅ Enabled |
| **RPi4** | 10.21.220.192 | `armv7l` | rpi4 | ✅ Ativo | ✅ Enabled |

---

## Components

| Device | Architecture | Components | Service | OTA Package |
|--------|-------------|------------|---------|-------------|
| **RPi4** | ARM 32-bit (armv7l) | Qt6 Cluster UI | `helloqt-app.service` | `update-rpi4.tar.gz` |
| **RPi5** | ARM 64-bit (aarch64) | KUKSA CAN→VSS | `can-to-kuksa.service` | `update-rpi5.tar.gz` |

---

## OTA Scripts Inventory

| Script | Location | Description |
|--------|----------|-------------|
| **ota-update.sh** | `/opt/ota/ota-update.sh` | Main 10-step update script |
| **ota-check.sh** | `/opt/ota/ota-check.sh` | GitHub API polling script |
| **ota-check.timer** | `/etc/systemd/system/` | systemd timer (15 min) |
| **ota-check.service** | `/etc/systemd/system/` | systemd service unit |
| **setup-ota-device.sh** | `src/ota/scripts/` | One-time device setup |

---

## Detailed OTA Flow

### CI/CD (GitHub Actions)

```
┌─────────────────────────────────────────────────────────────────────────┐
│ GitHub Actions (triggered by: git push origin v1.9.0)                   │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│   ┌─────────────────────┐    ┌─────────────────────┐                   │
│   │ build-cluster-rpi4  │    │  build-kuksa-rpi5   │                   │
│   │    (parallel)       │    │    (parallel)       │                   │
│   ├─────────────────────┤    ├─────────────────────┤                   │
│   │ SDK: team6-agl-sdk  │    │ SDK: team6-r5-sdk   │                   │
│   │ • qt6-build.sh      │    │ • make              │                   │
│   │ • HelloQt6Qml       │    │ • can_to_kuksa_pub  │                   │
│   └─────────┬───────────┘    └──────────┬──────────┘                   │
│             │                           │                               │
│             └───────────┬───────────────┘                               │
│                         ▼                                               │
│              ┌──────────────────────┐                                   │
│              │      release         │                                   │
│              │ • Create tar.gz      │                                   │
│              │ • Generate SHA256    │                                   │
│              │ • Upload to Release  │                                   │
│              └──────────────────────┘                                   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### Device Polling (ota-check.sh)

```
┌─────────────────────────────────────────────────────────────────────────┐
│ ota-check.timer (every 15 minutes)                                      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│   1. Read /etc/ota-version         → "v1.8.0"                           │
│   2. Call GitHub API                                                    │
│      curl https://api.github.com/repos/.../releases/latest              │
│   3. Extract tag_name              → "v1.9.0"                           │
│   4. Compare versions                                                   │
│      "v1.8.0" != "v1.9.0" → NEW VERSION!                                │
│   5. Check /etc/ota-auto-update                                         │
│      └── If "enabled" → /opt/ota/ota-update.sh v1.9.0                   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### Update Script (ota-update.sh)

```
┌─────────────────────────────────────────────────────────────────────────┐
│ /opt/ota/ota-update.sh v1.9.0                                           │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  [1/10] Detect platform (rpi4/rpi5 via uname -m)                        │
│  [2/10] Download update-rpi4.tar.gz or update-rpi5.tar.gz               │
│  [3/10] Verify SHA256 hash                                              │
│  [4/10] Extract to /opt/ota/releases/v1.9.0/                            │
│  [5/10] Stop service                                                    │
│  [6/10] Atomic symlink: /opt/ota/current → v1.9.0                       │
│  [7/10] Verify binary architecture                                      │
│  [8/10] Copy binary to target path                                      │
│  [9/10] Start service                                                   │
│  [10/10] Health check (restart loop detection)                          │
│                                                                         │
│  ┌────────────────────┐     ┌────────────────────┐                      │
│  │ SUCCESS            │     │ FAILURE            │                      │
│  │ → /etc/ota-version │     │ → Rollback         │                      │
│  └────────────────────┘     └────────────────────┘                      │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## Docker Images Required

```bash
# For RPi4 (32-bit)
souzitaaaa/team6-agl-sdk:latest       # armv7vet2hf-neon-vfpv4-agl-linux-gnueabi

# For RPi5 (64-bit)
souzitaaaa/team6-r5-agl-sdk:latest    # aarch64-agl-linux
```

---

## OTA Script Features (v2)

| Feature | Description | Status |
|---------|-------------|--------|
| **Platform Auto-Detection** | Detects rpi4/rpi5 via `uname -m` | ✅ |
| **Architecture Verification** | Checks binary arch before install | ✅ |
| **Restart Loop Detection** | Detects if service keeps crashing | ✅ |
| **Automatic Rollback** | Reverts to previous version on failure | ✅ |
| **Hash Verification** | SHA256 integrity check | ✅ |
| **Atomic Symlink Switch** | Zero-downtime updates | ✅ |
| **Version History** | Keeps previous versions for rollback | ✅ |
| **Auto-Update Mode** | `/etc/ota-auto-update = enabled` | ✅ |
| **Timer Polling** | Checks every 15 minutes | ✅ |

---

## File Locations

### On AGL Devices

```
/opt/ota/
├── ota-update.sh           # Main update script
├── ota-check.sh            # Polling script
├── logs/
│   ├── ota.log             # Update logs
│   └── ota-check.log       # Polling logs
├── downloads/
│   └── update.tar.gz       # Downloaded package
├── releases/
│   ├── v1.7.0/
│   ├── v1.8.0/
│   └── v1.9.0/             # Extracted releases
├── current -> releases/v1.9.0   # Active symlink
└── backup/

/etc/
├── ota-version             # Current version (e.g., "v1.8.0")
├── ota-auto-update         # "enabled" or "disabled"
└── systemd/system/
    ├── ota-check.service
    └── ota-check.timer
```

### In Repository

```
src/ota/
├── scripts/
│   ├── ota-update.sh
│   ├── ota-check.sh
│   └── setup-ota-device.sh
└── systemd/
    ├── ota-check.service
    └── ota-check.timer

.github/workflows/
└── ota.yml                 # Multi-platform CI/CD
```

---

## Device Setup (One-Time)

```bash
# Copy and run setup script
scp src/ota/scripts/setup-ota-device.sh root@<IP>:/tmp/
ssh root@<IP> "chmod +x /tmp/setup-ota-device.sh && /tmp/setup-ota-device.sh"

# Enable auto-update
ssh root@<IP> "echo 'enabled' > /etc/ota-auto-update"

# Verify timer is active
ssh root@<IP> "systemctl list-timers | grep ota"
```

---

*Sprint 8 - 13 February 2026*
*SEA:ME Team 6*
