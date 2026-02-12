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

## 🚀 Demo: OTA Update on AGL

### Command:
```bash
/opt/ota/ota-update.sh v1.5.0
```

### Output:
```
[14:11:41] === OTA Update to v1.5.0 ===
[14:11:41] [1/7] Downloading package from GitHub Release...
[14:11:42] [2/7] Verifying SHA256 hash... OK
[14:11:42] [3/7] Backing up current version...
[14:11:43] [4/7] Extracting update package...
[14:11:43] [5/7] Stopping services...
[14:11:43] [6/7] Installing new binaries...
           Installed: can_to_kuksa_publisher
           Installed: vss_min.json
           Installed: HelloQt6Qml
[14:11:43] [7/7] Starting services...
[14:11:46] === Update to v1.5.0 successful ===
```

---

## ✅ Verification

```bash
$ cat /etc/ota-version
v1.5.0

$ file /home/root/kuksa_RPi5/bin/can_to_kuksa_publisher
ELF 32-bit LSB pie executable, ARM

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
| **B** | Atomic symlinks, auto-polling | 🔜 Next |
| **C** | RAUC (A/B rootfs) | 📋 Planned |

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

✅ **Complete OTA pipeline working**
- Developer pushes tag → GitHub builds → AGL installs
- Hash verification + backup + rollback
- 13 sprint points delivered

**Next:** Automatic polling + cluster.service

---

# Questions?

📡 **Team 6 — SEA:ME 2025-26**
