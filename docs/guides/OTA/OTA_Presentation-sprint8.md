# 📡 OTA Implementation — SEA:ME Team 6

## Sprint 8 Demo — 13 February 2026

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

---

## 🔄 Complete OTA Workflow (100% Automatic)

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

## 📋 System Components

| Componente | Ficheiro | Status | Descrição |
|------------|----------|--------|-----------|
| **CI/CD Build** | `.github/workflows/ota.yml` | ✅ Automático | Trigger em tags `v*` |
| **Update Script** | `/opt/ota/ota-update.sh` | ✅ Automático | Download, install, rollback |
| **Polling Timer** | `ota-check.timer` | ✅ Ativo | Verifica GitHub cada 15 min |
| **Polling Script** | `/opt/ota/ota-check.sh` | ✅ Ativo | Compara versões, trigger update |
| **Setup Script** | `setup-ota-device.sh` | ✅ Novo | Configura dispositivo automaticamente |

---

## 🔄 Workflow Detalhado (4 Fases)

### FASE 1: Developer faz alterações

```
┌─────────────────────────────────────────────────────────────────────────┐
│ 1. DEVELOPER                                                            │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│   $ git add .                                                           │
│   $ git commit -m "Fix bug in cluster UI"                               │
│   $ git push origin feature/OTA/implementation                          │
│                                                                         │
│   # Quando pronto para release:                                         │
│   $ git tag v1.9.0                                                      │
│   $ git push origin v1.9.0    ◄─── ISTO DISPARA O WORKFLOW!             │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### FASE 2: GitHub Actions (Automático)

```
┌─────────────────────────────────────────────────────────────────────────┐
│ 2. GITHUB ACTIONS (automático quando tag é pushed)                      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│   ┌─────────────────────┐    ┌─────────────────────┐                   │
│   │ build-cluster-rpi4  │    │  build-kuksa-rpi5   │                   │
│   │    (parallel)       │    │    (parallel)       │                   │
│   ├─────────────────────┤    ├─────────────────────┤                   │
│   │ • Pull SDK (32-bit) │    │ • Pull SDK (64-bit) │                   │
│   │ • qt6-build.sh      │    │ • make              │                   │
│   │ • HelloQt6Qml       │    │ • can_to_kuksa_pub  │                   │
│   │ • Upload artifact   │    │ • Upload artifact   │                   │
│   └─────────┬───────────┘    └──────────┬──────────┘                   │
│             │                           │                               │
│             └───────────┬───────────────┘                               │
│                         ▼                                               │
│              ┌──────────────────────┐                                   │
│              │      release         │                                   │
│              ├──────────────────────┤                                   │
│              │ • Download artifacts │                                   │
│              │ • Create tar.gz      │                                   │
│              │ • Generate SHA256    │                                   │
│              │ • Upload to Release  │                                   │
│              └──────────────────────┘                                   │
│                         │                                               │
│   GitHub Releases: v1.9.0                                               │
│   ├── update-rpi4.tar.gz (4.6 MB)                                       │
│   ├── update-rpi5.tar.gz (260 KB)                                       │
│   └── hash-*.txt                                                        │
│                                                                         │
│   Tempo estimado: ~3-5 minutos                                          │
└─────────────────────────────────────────────────────────────────────────┘
```

### FASE 3: Dispositivo AGL (Polling Automático)

```
┌─────────────────────────────────────────────────────────────────────────┐
│ 3. AGL DEVICE - POLLING (automático cada 15 minutos)                    │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│   systemd timer: ota-check.timer                                        │
│   ├── OnBootSec=2min          (2 min após boot)                         │
│   ├── OnUnitActiveSec=15min   (cada 15 min depois)                      │
│   └── RandomizedDelaySec=60   (evitar "thundering herd")                │
│                         │                                               │
│                         ▼                                               │
│   ┌─────────────────────────────────────────────────────────────────┐   │
│   │ /opt/ota/ota-check.sh                                           │   │
│   ├─────────────────────────────────────────────────────────────────┤   │
│   │ 1. Ler /etc/ota-version              → "v1.8.0"                 │   │
│   │ 2. Chamar GitHub API                                            │   │
│   │    curl https://api.github.com/repos/.../releases/latest        │   │
│   │ 3. Extrair tag_name                  → "v1.9.0"                 │   │
│   │ 4. Comparar versões                                             │   │
│   │    "v1.8.0" != "v1.9.0" → NOVA VERSÃO!                          │   │
│   │ 5. Verificar /etc/ota-auto-update                               │   │
│   │    └── Se "enabled" → /opt/ota/ota-update.sh v1.9.0             │   │
│   └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### FASE 4: Update Script (Automático)

```
┌─────────────────────────────────────────────────────────────────────────┐
│ 4. OTA UPDATE (/opt/ota/ota-update.sh v1.9.0)                           │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  [1/10] Detect platform (rpi4 ou rpi5 via uname -m)                     │
│  [2/10] Download update-rpi4.tar.gz ou update-rpi5.tar.gz               │
│  [3/10] Verify SHA256 hash                                              │
│  [4/10] Extract to /opt/ota/releases/v1.9.0/                            │
│  [5/10] Stop service (helloqt-app.service ou can-to-kuksa.service)      │
│  [6/10] Atomic symlink: /opt/ota/current → /opt/ota/releases/v1.9.0     │
│  [7/10] Verify binary architecture                                      │
│  [8/10] Copy binary to target path                                      │
│  [9/10] Start service                                                   │
│  [10/10] Health check (restart loop detection)                          │
│                                                                         │
│  ┌────────────────────┐     ┌────────────────────┐                      │
│  │ SUCCESS            │     │ FAILURE            │                      │
│  ├────────────────────┤     ├────────────────────┤                      │
│  │ Write v1.9.0 to    │     │ Rollback symlink   │                      │
│  │ /etc/ota-version   │     │ to previous version│                      │
│  │ Log success        │     │ Restart old binary │                      │
│  └────────────────────┘     └────────────────────┘                      │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## ✅ Device Configuration (13 February 2026)

| Dispositivo | IP | Arquitetura | Plataforma | Timer | Auto-Update |
|-------------|-----|-------------|------------|-------|-------------|
| **RPi5** | 10.21.220.191 | `aarch64` | rpi5 | ✅ Ativo | ✅ Enabled |
| **RPi4** | 10.21.220.192 | `armv7l` | rpi4 | ✅ Ativo | ✅ Enabled |

**Binaries Installed:**
- RPi5: `/home/kuksa_RPi5/bin/can_to_kuksa_publisher` + `vss_min.json`
- RPi4: `/opt/cluster/HelloQt6Qml`

**Services:**
- RPi5: `can-to-kuksa.service`
- RPi4: `helloqt-app.service`

---

## 🛡️ Safety Features

| Feature | Description | Status |
|---------|-------------|--------|
| **Hash verification** | SHA256 before install | ✅ |
| **Architecture verification** | Check binary matches system | ✅ |
| **Restart loop detection** | 3+ restarts in 10s = failure | ✅ |
| **Atomic symlinks** | Zero-downtime switching | ✅ |
| **Health check** | Verify service starts | ✅ |
| **Auto-rollback** | Restore previous on failure | ✅ |
| **Logging** | All operations logged | ✅ |

---

## 📁 Key Files

| Location | File | Description |
|----------|------|-------------|
| GitHub | `.github/workflows/ota.yml` | CI/CD multi-platform workflow |
| AGL | `/opt/ota/ota-update.sh` | Main update script (10 steps) |
| AGL | `/opt/ota/ota-check.sh` | Polling script (GitHub API) |
| AGL | `/etc/systemd/system/ota-check.timer` | 15-min timer |
| AGL | `/etc/ota-version` | Current version |
| AGL | `/etc/ota-auto-update` | "enabled" for auto-update |
| Repo | `src/ota/scripts/setup-ota-device.sh` | One-time device setup |

---

## 📊 Metrics

| Metric | Value |
|--------|-------|
| CI/CD build time | ~3-5 minutes |
| Package size (RPi4) | 4.6 MB |
| Package size (RPi5) | 260 KB |
| Download time | ~1 second |
| Install time | ~5 seconds |
| Total OTA time | ~6 seconds |
| Polling interval | 15 minutes |

---

## 🗺️ Roadmap

| Phase | Description | Status |
|-------|-------------|--------|
| **A.1** | hello-ota PoC | ✅ Complete |
| **A.2** | Real binaries (kuksa + cluster) | ✅ Complete |
| **B** | Enhanced rollback, CI/CD | ✅ Complete |
| **C** | Atomic symlinks, auto-polling | ✅ Complete |
| **C.2** | Multi-platform (RPi4 + RPi5) | ✅ Complete |
| **C.3** | 100% Automatic (timer + auto-update) | ✅ **Complete** |
| **D** | RAUC (A/B rootfs) | 📋 Planned |

---

## 🚀 Demo Commands

### Trigger new release:
```bash
git tag v1.9.0
git push origin v1.9.0
```

### Check device versions:
```bash
ssh root@10.21.220.191 "cat /etc/ota-version"  # RPi5
ssh root@10.21.220.192 "cat /etc/ota-version"  # RPi4
```

### Manual trigger (without waiting for timer):
```bash
ssh root@10.21.220.191 "/opt/ota/ota-check.sh"
ssh root@10.21.220.192 "/opt/ota/ota-check.sh"
```

### View logs:
```bash
ssh root@10.21.220.191 "cat /opt/ota/logs/ota-check.log"
ssh root@10.21.220.191 "cat /opt/ota/logs/ota.log"
```

---

## 🎉 Summary

✅ **Complete 100% Automatic OTA Pipeline**

1. Developer pushes tag → GitHub builds 32-bit & 64-bit
2. Timer polls GitHub every 15 minutes
3. Auto-detects new version → triggers update
4. Architecture verification + health check + rollback

**No manual intervention required after initial setup!**

---

# Questions?

📡 **Team 6 — SEA:ME 2025-26**
