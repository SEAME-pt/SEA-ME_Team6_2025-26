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

## 🏗️ Architecture

```
┌──────────────┐     ┌─────────────────┐     ┌─────────────────┐
│  Developer   │────▶│  GitHub Actions │────▶│ GitHub Releases │
│  git push    │     │  ARM64 Build    │     │  update.tar.gz  │
└──────────────┘     └─────────────────┘     └────────┬────────┘
                                                      │ HTTPS
                                                      ▼
                                             ┌─────────────────┐
                                             │  AGL (RPi 5)    │
                                             │  ota-update.sh  │
                                             │  ─────────────  │
                                             │  • Download     │
                                             │  • Verify hash  │
                                             │  • Backup       │
                                             │  • Install      │
                                             │  • Health check │
                                             │  • Rollback     │
                                             └─────────────────┘
```

---

## 🔄 CI/CD Pipeline

### Trigger: `git tag v1.5.0 && git push origin v1.5.0`

```
┌─────────────────┐   ┌──────────────────┐   ┌───────────────┐
│   build-kuksa   │   │  build-cluster   │   │    release    │
│    (parallel)   │   │    (parallel)    │   │  (sequential) │
├─────────────────┤   ├──────────────────┤   ├───────────────┤
│ Cross-compile   │   │ Cross-compile    │   │ Package OTA   │
│ ARM binary      │   │ Qt6 ARM binary   │   │ Create release│
│ 856KB           │   │ 13.5MB           │   │ Upload assets │
└─────────────────┘   └──────────────────┘   └───────────────┘
```

**Docker Image:** `souzitaaaa/team6-agl-sdk:latest`
- Qt6 6.7.3 + gRPC + Protobuf
- ARM cross-compiler (armv7vet2hf-neon-vfpv4)

---

## 📦 OTA Package Contents

```
update.tar.gz (4.8 MB)
├── kuksa/
│   ├── bin/
│   │   └── can_to_kuksa_publisher   # CAN→KUKSA bridge
│   └── vss_min.json                 # Vehicle Signal Spec
└── cluster/
    └── HelloQt6Qml                  # Qt6 Dashboard UI
```

**Verification:** SHA256 hash checked before installation

---

## 🚀 Demo: Automatic OTA Update (Phase C)

### Automatic Update Flow:
```
✅ [1/8] Download do package
✅ [2/8] Hash verified OK  
✅ [3/8] Extracting to /opt/ota/releases/v1.6.0
✅ [4/8] Stopping services
✅ [5/8] Previous version: v1.5.0
✅ [6/8] Symlink updated: /opt/ota/current -> /opt/ota/releases/v1.6.0
✅ [7/8] Installing binaries
✅ [8/8] Starting services
✅ === Update to v1.6.0 successful ===
```

### Timer-based Polling:
```bash
$ systemctl list-timers | grep ota
NEXT                          LEFT    LAST                          PASSED
Tue 2026-02-10 18:28:00 UTC   15min   Tue 2026-02-10 18:13:02 UTC   2s ago   ota-check.timer
```
```

---

## ✅ Verification

```bash
$ cat /etc/ota-version
v1.6.0

$ ls -la /opt/ota/current
lrwxrwxrwx 1 root root 26 /opt/ota/current -> /opt/ota/releases/v1.6.0

$ ls /opt/ota/releases/
v1.0.1  v1.5.0  v1.6.0

$ systemctl is-active can-to-kuksa.service
active
```

---

## 🛡️ Safety Features

| Feature | Description | Status |
|---------|-------------|--------|
| **Hash verification** | SHA256 before install | ✅ |
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
| **C** | Atomic symlinks, auto-polling | ✅ **Complete** |
| **D** | RAUC (A/B rootfs) | 📋 Planned |

### Phase C Features:

| Feature | Status |
|---------|--------|
| Timer automático (15 min) | ✅ |
| GitHub API polling | ✅ |
| Auto-download | ✅ |
| Hash verification | ✅ |
| Atomic symlink switch | ✅ |
| Service restart | ✅ |
| Version history | ✅ |
| Rollback capability | ✅ |

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

✅ **Complete OTA pipeline with automatic updates**
- Developer pushes tag → GitHub builds → AGL auto-installs
- Timer polls GitHub every 15 minutes
- Atomic symlink switching for zero-downtime
- Hash verification + version history + rollback

**Delivered:** Phases A, B, C complete

**Next:** Phase D - RAUC (A/B rootfs)

---

# Questions?

📡 **Team 6 — SEA:ME 2025-26**
