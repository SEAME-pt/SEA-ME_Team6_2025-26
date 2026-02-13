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

| Dispositivo | IP | Arquitetura | Plataforma | Timer | Auto-Update | Versão Atual | Rede |
|-------------|-----|-------------|------------|-------|-------------|---------------|------|
| **RPi5** | 10.21.220.191 | `aarch64` | rpi5 | ✅ Ativo | ✅ Enabled | v1.8.0 | ⚠️ Sem Internet |
| **RPi4** | 10.21.220.192 | `armv7l` | rpi4 | ✅ Ativo | ✅ Enabled | **v1.9.0** ✅ | ✅ OK |

> **Nota:** O RPi5 requer configuração de rede (DNS/gateway) para aceder ao GitHub API e receber updates OTA automáticos.

### Troubleshooting - Verificar conectividade:

```bash
# Testar se o dispositivo consegue aceder ao GitHub
ssh root@10.21.220.191 "curl -s --max-time 5 https://api.github.com/repos/SEAME-pt/SEA-ME_Team6_2025-26/releases/latest | grep tag_name"
ssh root@10.21.220.192 "curl -s --max-time 5 https://api.github.com/repos/SEAME-pt/SEA-ME_Team6_2025-26/releases/latest | grep tag_name"

# Se falhar, verificar DNS
ssh root@<IP> "ping -c 1 api.github.com"
ssh root@<IP> "cat /etc/resolv.conf"
```

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

## ⏱️ Timer e Polling (Localização nos Dispositivos AGL)

O timer de polling está instalado **nos dispositivos AGL** (não no GitHub). São ficheiros systemd:

| Ficheiro | Localização Completa | Função |
|----------|----------------------|--------|
| `ota-check.timer` | `/etc/systemd/system/ota-check.timer` | Define QUANDO executar (cada 15 min) |
| `ota-check.service` | `/etc/systemd/system/ota-check.service` | Define O QUE executar (/opt/ota/ota-check.sh) |

### Como visualizar os ficheiros:

```bash
# Ver conteúdo do timer
ssh root@10.21.220.191 "cat /etc/systemd/system/ota-check.timer"
ssh root@10.21.220.192 "cat /etc/systemd/system/ota-check.timer"

# Ver conteúdo do service
ssh root@10.21.220.191 "cat /etc/systemd/system/ota-check.service"
ssh root@10.21.220.192 "cat /etc/systemd/system/ota-check.service"
```

### Conteúdo esperado do `ota-check.timer`:

```ini
[Unit]
Description=OTA Update Check Timer

[Timer]
OnBootSec=2min
OnUnitActiveSec=15min
RandomizedDelaySec=60

[Install]
WantedBy=timers.target
```

**Explicação:**
- `OnBootSec=2min` → Primeira execução 2 minutos após boot
- `OnUnitActiveSec=15min` → Repetir cada 15 minutos após cada execução
- `RandomizedDelaySec=60` → Adiciona até 60 segundos de delay aleatório (evita "thundering herd")

### Conteúdo esperado do `ota-check.service`:

```ini
[Unit]
Description=OTA Update Check Service

[Service]
Type=oneshot
ExecStart=/opt/ota/ota-check.sh
```

**Explicação:**
- `Type=oneshot` → Executa uma vez e termina
- `ExecStart=/opt/ota/ota-check.sh` → Script que verifica nova versão no GitHub

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

## 🔍 Comandos de Verificação do Timer

### Ver quando o timer correu e quando vai correr novamente:

```bash
ssh root@10.21.220.191 "systemctl list-timers | grep ota"
ssh root@10.21.220.192 "systemctl list-timers | grep ota"
```

**Output esperado:**
```
NEXT                        LEFT          LAST                        PASSED       UNIT              ACTIVATES
Fri 2026-02-13 13:30:00 UTC 2min 30s left Fri 2026-02-13 13:15:00 UTC 12min ago    ota-check.timer   ota-check.service
```

**Significado das colunas:**
- `NEXT` = Próxima execução agendada
- `LEFT` = Tempo restante até próxima execução
- `LAST` = Última vez que executou
- `PASSED` = Há quanto tempo executou

### Ver os logs do que o timer fez:

```bash
# Logs do polling (ota-check.sh)
ssh root@10.21.220.191 "cat /opt/ota/logs/ota-check.log"
ssh root@10.21.220.192 "cat /opt/ota/logs/ota-check.log"

# Logs do update (ota-update.sh) - se houve update
ssh root@10.21.220.191 "cat /opt/ota/logs/ota.log"
ssh root@10.21.220.192 "cat /opt/ota/logs/ota.log"

# Ou ver via journalctl (últimos 30 minutos)
ssh root@10.21.220.191 "journalctl -u ota-check.service --since '30 min ago'"
ssh root@10.21.220.192 "journalctl -u ota-check.service --since '30 min ago'"
```

### Verificar a versão atual (confirmar se atualizou):

```bash
ssh root@10.21.220.191 "cat /etc/ota-version"   # RPi5 (KUKSA)
ssh root@10.21.220.192 "cat /etc/ota-version"   # RPi4 (Cluster)
```

---

## 📋 Resultados dos Testes (v1.7.0 → v1.8.0) - 12/13 Feb 2026

### Exemplo de log de polling (ota-check.log):

```
[2026-02-12 13:09:58] Current version: v1.7.0
[2026-02-12 13:09:59] Checking GitHub for latest release...
[2026-02-12 13:09:59] Latest version: v1.8.0
[2026-02-12 13:09:59] New version available: v1.8.0 (current: v1.7.0)
[2026-02-12 13:09:59] Auto-update is enabled, triggering update...
...
[2026-02-12 14:43:09] Current version: v1.8.0
[2026-02-12 14:43:09] Already up to date
```

### Exemplo de log de update bem sucedido (ota.log):

```
[2026-02-12 14:37:56] === OTA Update to v1.8.0 (Phase B - Atomic) ===
[2026-02-12 14:37:56] System architecture: aarch64
[2026-02-12 14:37:56] Device model: Raspberry Pi 5 Model B Rev 1.1
[2026-02-12 14:37:56] Detected platform: rpi5
[2026-02-12 14:37:56] [1/10] Downloading package from GitHub Release...
[2026-02-12 14:37:56] Downloading update-rpi5.tar.gz for rpi5...
[2026-02-12 14:37:57] [2/10] Verifying SHA256 hash...
[2026-02-12 14:37:57] Hash verified OK
[2026-02-12 14:37:57] [3/10] Extracting to /opt/ota/releases/v1.8.0...
[2026-02-12 14:37:57] [4/10] Stopping services...
[2026-02-12 14:37:57] [5/10] Previous version: v1.7.0
[2026-02-12 14:37:57] [6/10] Performing atomic symlink switch...
[2026-02-12 14:37:57] Symlink updated: /opt/ota/current -> /opt/ota/releases/v1.8.0
[2026-02-12 14:37:57] [7/10] Verifying binary architecture...
[2026-02-12 14:37:57] can_to_kuksa_publisher: architecture OK (64-bit ARM)
[2026-02-12 14:37:57] [8/10] Installing binaries for rpi5...
[2026-02-12 14:37:57] Installed: can_to_kuksa_publisher
[2026-02-12 14:37:57] Installed: vss_min.json
[2026-02-12 14:37:57] [9/10] Starting services for rpi5...
[2026-02-12 14:37:57] [10/10] Performing health check...
[2026-02-12 14:38:01] can-to-kuksa.service: active and stable (restarts: 0)
[2026-02-12 14:38:01] === Update to v1.8.0 successful ===
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
