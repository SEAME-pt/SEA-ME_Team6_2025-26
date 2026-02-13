# OTA Multi-Version Architecture - Sprint 8

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
                    │  │  v1.8.0                 │  │
                    │  │  ├── update-rpi4.tar.gz│  │
                    │  │  ├── hash-rpi4.txt     │  │
                    │  │  ├── update-rpi5.tar.gz│  │
                    │  │  ├── hash-rpi5.txt     │  │
                    │  │  ├── update.tar.gz     │  │
                    │  │  └── hash.txt          │  │
                    │  └─────────────────────────┘  │
                    │                               │
                    └───────────────────────────────┘
```

## Components

| Device | Architecture | Components | Service | OTA Package |
|--------|-------------|------------|---------|-------------|
| **RPi4** | ARM 32-bit (armv7l) | Qt6 Cluster UI | `helloqt-app.service` | `update-rpi4.tar.gz` |
| **RPi5** | ARM 64-bit (aarch64) | KUKSA CAN→VSS | `can-to-kuksa.service` | `update-rpi5.tar.gz` |

## Test Results (12 February 2026)

| Device | IP | `uname -m` | Package Size | Service Status |
|--------|-----|------------|--------------|----------------|
| **RPi5** | 10.21.220.191 | `aarch64` | 260 KB | ✅ Active, 0 restarts |
| **RPi4** | 10.21.220.192 | `armv7l` | 4.6 MB | ✅ Active, 0 restarts |

## Communication

- **RPi4 ↔ RPi5**: WiFi/Network (not CAN)
- **RPi4/RPi5 → GitHub**: HTTPS (OTA downloads)

## OTA Flow

```
GitHub Actions (CI/CD)
        │
        ├── build-cluster-rpi4 ──► SDK rpi4 (32-bit) ──► update-rpi4.tar.gz
        │
        └── build-kuksa-rpi5 ──► SDK rpi5 (64-bit) ──► update-rpi5.tar.gz
                                                              │
                                                              ▼
                                                      GitHub Releases
                                                              │
                    ┌─────────────────────────────────────────┴─────────────────────────────────────────┐
                    │                                                                                   │
                    ▼                                                                                   ▼
            RPi4 downloads                                                                      RPi5 downloads
            update-rpi4.tar.gz                                                                  update-rpi5.tar.gz
                    │                                                                                   │
                    ▼                                                                                   ▼
            ┌───────────────────┐                                                               ┌───────────────────┐
            │ OTA Health Check  │                                                               │ OTA Health Check  │
            │ • Verify arch     │                                                               │ • Verify arch     │
            │ • Detect loops    │                                                               │ • Detect loops    │
            │ • Auto-rollback   │                                                               │ • Auto-rollback   │
            └───────────────────┘                                                               └───────────────────┘
```

## Docker Images Required

```bash
# For RPi4 (32-bit)
souzitaaaa/team6-agl-sdk:latest       # armv7vet2hf-neon-vfpv4-agl-linux-gnueabi

# For RPi5 (64-bit)
souzitaaaa/team6-r5-agl-sdk:latest    # aarch64-agl-linux
```

## OTA Script Features (v2)

| Feature | Description | Status |
|---------|-------------|--------|
| **Architecture Verification** | Checks binary arch before install | ✅ |
| **Restart Loop Detection** | Detects if service keeps crashing | ✅ |
| **Automatic Rollback** | Reverts to previous version on failure | ✅ |
| **Hash Verification** | SHA256 integrity check | ✅ |
| **Atomic Symlink Switch** | Zero-downtime updates | ✅ |
| **Version History** | Keeps previous versions for rollback | ✅ |

## CI/CD Workflow Features

| Feature | Description |
|---------|-------------|
| **Platform Selection** | Build for `rpi4`, `rpi5`, or `both` |
| **Separate Artifacts** | Different packages per architecture |
| **Architecture Validation** | Verifies binary type in build |
| **Fallback Images** | Uses `:latest` if specific tag unavailable |

---

*Sprint 8 - February 2026*
*SEA:ME Team 6*
