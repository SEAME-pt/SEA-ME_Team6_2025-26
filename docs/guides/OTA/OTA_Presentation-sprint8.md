# 📡 OTA Implementation — SEA:ME Team 6

## Sprint 8 Demo — February 2026

---

## 🎯 What is OTA?

**Over-the-Air (OTA)** = Update software remotely without physical access

| Type | Description | Our Status |
|------|-------------|------------|
| **SOTA** | Software updates (binaries, apps) | ✅ Implemented |
| **COTA** | Configuration updates (JSON, configs) | ✅ Implemented |
| **FOTA** | Firmware updates (STM32) | 📋 Planned |

---

## 🏗️ Multi-Platform Architecture

```
┌─────────────────────────┐         ┌─────────────────────────┐
│        RPi4             │  WiFi/  │       RPi5 (AGL)        │
│       (32-bit)          │ Network │        (64-bit)         │
│  ┌─────────────────┐    │◄───────►│    ┌─────────────────┐  │
│  │    Cluster      │    │         │    │     KUKSA       │  │
│  │   (Qt6 UI)      │    │         │    │   (CAN→VSS)     │  │
│  └─────────────────┘    │         │    └─────────────────┘  │
│  OTA: update-rpi4.tar.gz│         │  OTA: update-rpi5.tar.gz│
└─────────────────────────┘         └─────────────────────────┘
            │                                   │
            └───────────────┬───────────────────┘
                            ▼
                    GitHub Releases
                    ┌─────────────────┐
                    │ update-rpi4.tar │
                    │ update-rpi5.tar │
                    │ update.tar.gz   │
                    └─────────────────┘
```

### OTA Script Features (v2):
| Feature | Description |
|---------|-------------|
| **Arch Verification** | Validates binary matches system architecture |
| **Restart Loop Detection** | Detects 3+ restarts in 10s → triggers rollback |
| **SHA256 Hash** | Verifies package integrity |
| **Atomic Symlinks** | Zero-downtime version switching |

---

## 🔄 CI/CD Pipeline (Multi-Platform)

### Trigger: `git tag v1.8.0 && git push origin v1.8.0`

```
┌─────────────────────┐   ┌─────────────────────┐   ┌────────────────────┐
│  build-cluster-rpi4 │   │  build-kuksa-rpi5   │   │      release       │
│     (32-bit ARM)    │   │    (64-bit ARM)     │   │   (multi-package)  │
├─────────────────────┤   ├─────────────────────┤   ├────────────────────┤
│ SDK: team6-agl-sdk  │   │ SDK: team6-agl-sdk  │   │ update-rpi4.tar.gz │
│      :rpi4          │   │      :rpi5          │   │ update-rpi5.tar.gz │
│ Qt6 Cluster 13.5MB  │   │ KUKSA 856KB         │   │ update.tar.gz      │
└─────────────────────┘   └─────────────────────┘   └────────────────────┘
```

**Docker Images:**
- `souzitaaaa/team6-agl-sdk:latest` — ARM 32-bit (armv7) for RPi4
- `souzitaaaa/team6-r5-agl-sdk:latest` — ARM 64-bit (aarch64) for RPi5

---

## 📦 OTA Package Contents

### RPi4 Package (`update-rpi4.tar.gz`)
```
cluster/
└── HelloQt6Qml                  # Qt6 Dashboard UI (32-bit)
```

### RPi5 Package (`update-rpi5.tar.gz`)
```
kuksa/
├── bin/
│   └── can_to_kuksa_publisher   # CAN→KUKSA bridge (64-bit)
└── vss_min.json                 # Vehicle Signal Spec
```

**Verification:** SHA256 hash + Architecture verification

---

## 🚀 Demo: OTA Update with Health Check (v2)

### Enhanced Update Flow (10 Steps):
```
✅ [1/10] Download do package
✅ [2/10] Hash verified OK  
✅ [3/10] Extracting to /opt/ota/releases/v1.8.0
✅ [4/10] Stopping services
✅ [5/10] Previous version: v1.7.0
✅ [6/10] Symlink updated: /opt/ota/current -> /opt/ota/releases/v1.8.0
✅ [7/10] Verifying binary architecture (aarch64) ← NEW!
✅ [8/10] Installing binaries
✅ [9/10] Starting services
✅ [10/10] Health check (restart loop detection) ← NEW!
✅ === Update to v1.8.0 successful ===
```

### Architecture Verification Example:
```bash
# Script verifies binary matches system architecture
$ uname -m
aarch64

$ file /home/kuksa_RPi5/bin/can_to_kuksa_publisher
ELF 64-bit LSB pie executable, ARM aarch64...  ✅

# If mismatch → abort update before installing!
```

### Timer-based Polling:
```bash
$ systemctl list-timers | grep ota
NEXT                          LEFT    LAST                          PASSED
Tue 2026-02-10 18:28:00 UTC   15min   Tue 2026-02-10 18:13:02 UTC   2s ago   ota-check.timer
```

---

## ✅ Verification

```bash
$ cat /etc/ota-version
v1.8.0

$ ls -la /opt/ota/current
lrwxrwxrwx 1 root root 26 /opt/ota/current -> /opt/ota/releases/v1.8.0

$ ls /opt/ota/releases/
v1.5.0  v1.6.0  v1.7.0  v1.8.0

$ systemctl is-active can-to-kuksa.service
active
```

---

## ✅ Test Results (12 February 2026)

| Device | IP | Architecture | Package | Service | Status |
|--------|-----|--------------|---------|---------|--------|
| **RPi5** | 10.21.220.191 | `aarch64` (64-bit) | `update-rpi5.tar.gz` | `can-to-kuksa.service` | ✅ Active |
| **RPi4** | 10.21.220.192 | `armv7l` (32-bit) | `update-rpi4.tar.gz` | `helloqt-app.service` | ✅ Active |

**Binaries Installed:**
- RPi5: `/home/kuksa_RPi5/bin/can_to_kuksa_publisher` + `vss_min.json`
- RPi4: `/opt/cluster/HelloQt6Qml`

---

## 🛡️ Safety Features

| Feature | Description | Status |
|---------|-------------|--------|
| **Hash verification** | SHA256 before install | ✅ |
| **Architecture verification** | Check binary matches system | ✅ NEW |
| **Restart loop detection** | 3+ restarts in 10s = failure | ✅ NEW |
| **Backup** | Previous version saved | ✅ |
| **Health check** | Verify service starts | ✅ |
| **Rollback** | Auto-restore if failed | ✅ |
| **Logging** | All operations logged | ✅ |

---

## 📊 Metrics

| Metric | Value |
|--------|-------|
| Package size | 4.8 MB |
| Download time | ~1 second |
| Install time | ~5 seconds |
| Total OTA time | ~6 seconds |

---

## 🗺️ Roadmap

| Phase | Description | Status |
|-------|-------------|--------|
| **A.1** | hello-ota PoC | ✅ Complete |
| **A.2** | Real binaries (kuksa + cluster) | ✅ Complete |
| **B** | Enhanced rollback, CI/CD | ✅ Complete |
| **C** | Atomic symlinks, auto-polling | ✅ Complete |
| **C.2** | Multi-platform (RPi4 + RPi5) | ✅ **Complete** |
| **D** | RAUC (A/B rootfs) | 📋 Planned |

### Phase C.2 Features (NEW):

| Feature | Status |
|---------|--------|
| Multi-platform workflow | ✅ |
| ARM 32-bit (RPi4) support | ✅ |
| ARM 64-bit (RPi5) support | ✅ |
| Architecture verification | ✅ |
| Restart loop detection | ✅ |
| Separate packages per platform | ✅ |
| Docker SDK images (rpi4/rpi5) | ✅ |

---

## 📁 Key Files

| Location | File |
|----------|------|
| GitHub | `.github/workflows/ota.yml` |
| AGL | `/opt/ota/ota-update.sh` |
| AGL | `/etc/ota-version` |
| Docs | `docs/guides/OTA_Implementation_Guide.md` |

---

## 🎉 Summary

✅ **Complete Multi-Platform OTA pipeline**
- Developer pushes tag → GitHub builds 32-bit & 64-bit → AGL auto-installs
- Timer polls GitHub every 15 minutes
- Atomic symlink switching for zero-downtime
- Architecture verification + restart loop detection + rollback

**Delivered:** Phases A, B, C, C.2 complete

**Next:** Phase D - RAUC (A/B rootfs)

---

# Questions?

📡 **Team 6 — SEA:ME 2025-26**
