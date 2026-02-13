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

| Dispositivo | IP | Arquitetura | Plataforma | Timer | Auto-Update | Versão Atual |
|-------------|-----|-------------|------------|-------|-------------|---------------|
| **RPi5** | 10.21.220.191 | `aarch64` | rpi5 | ✅ Ativo | ✅ Enabled | **v1.9.0** ✅ |
| **RPi4** | 10.21.220.192 | `armv7l` | rpi4 | ✅ Ativo | ✅ Enabled | **v1.9.0** ✅ |

✅ **Ambos os dispositivos atualizados automaticamente para v1.9.0!**

**Binaries Installed:**
- RPi5: `/home/kuksa_RPi5/bin/can_to_kuksa_publisher` + `vss_min.json`
- RPi4: `/opt/cluster/HelloQt6Qml`

**Services:**
- RPi5: `can-to-kuksa.service`
- RPi4: `helloqt-app.service`

---

## 📋 Resultados dos Testes (v1.8.0 → v1.9.0) - 13 Feb 2026

### RPi5 (KUKSA Publisher) - Update Automático com Sucesso:

```
[2026-02-13 14:00:27] Current version: v1.8.0
[2026-02-13 14:00:27] Checking GitHub for latest release...
[2026-02-13 14:00:27] Latest version: v1.9.0
[2026-02-13 14:00:27] New version available: v1.9.0 (current: v1.8.0)
[2026-02-13 14:00:27] Auto-update is enabled, triggering update...
[2026-02-13 14:00:27] === OTA Update to v1.9.0 (Phase B - Atomic) ===
[2026-02-13 14:00:27] Detected platform: rpi5
[2026-02-13 14:00:27] Downloading update-rpi5.tar.gz for rpi5...
[2026-02-13 14:00:28] Hash verified OK
[2026-02-13 14:00:31] can_to_kuksa_publisher: architecture OK (64-bit ARM)
[2026-02-13 14:00:31] Installed: can_to_kuksa_publisher
[2026-02-13 14:00:31] Installed: vss_min.json
[2026-02-13 14:00:35] can-to-kuksa.service: active and stable (restarts: 0)
[2026-02-13 14:00:35] === Update to v1.9.0 successful ===
```

### RPi4 (Cluster) - Update Automático com Sucesso:

```
[2026-02-13 xx:xx:xx] === Update to v1.9.0 successful ===
```

**Demonstra:**
- ✅ Detecção automática de plataforma (rpi4 vs rpi5)
- ✅ Download do pacote correto (update-rpi4.tar.gz vs update-rpi5.tar.gz)
- ✅ Verificação de SHA256 hash
- ✅ Verificação de arquitetura (32-bit vs 64-bit)
- ✅ Health check do serviço
- ✅ Polling automático cada 15 minutos
- ✅ Atomic symlink switch
- ✅ Rollback disponivel (v1.8.0, v1.7.0, etc)

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
| AGL | `/etc/systemd/system/ota-check.service` | Service triggered by timer |
| AGL | `/etc/ota-version` | Current version |
| AGL | `/etc/ota-auto-update` | "enabled" for auto-update |
| Repo | `src/ota/scripts/setup-ota-device.sh` | One-time device setup |

---

## ⏱️ Timer e Polling (Localização nos Dispositivos AGL)

O timer de polling está instalado **nos dispositivos AGL** (não no GitHub). São ficheiros systemd:

| Ficheiro | Localização no Dispositivo | Função |
|----------|---------------------------|--------|
| `ota-check.timer` | `/etc/systemd/system/ota-check.timer` | Dispara o serviço cada 15 minutos |
| `ota-check.service` | `/etc/systemd/system/ota-check.service` | Executa o `/opt/ota/ota-check.sh` |

### Como visualizar os ficheiros:

```bash
# Ver conteúdo do timer
ssh root@10.21.220.191 "cat /etc/systemd/system/ota-check.timer"
ssh root@10.21.220.192 "cat /etc/systemd/system/ota-check.timer"

# Ver conteúdo do service
ssh root@10.21.220.191 "cat /etc/systemd/system/ota-check.service"
ssh root@10.21.220.192 "cat /etc/systemd/system/ota-check.service"
```

### O que cada ficheiro faz:

**`ota-check.timer`** - Define QUANDO o polling acontece:
- `OnBootSec=2min` → Primeira execução 2 min após boot
- `OnUnitActiveSec=15min` → Repetir cada 15 minutos
- `RandomizedDelaySec=60` → Adiciona até 60s de delay aleatório (evita "thundering herd")

**`ota-check.service`** - Define O QUE executa:
- Executa `/opt/ota/ota-check.sh`
- Verifica versão atual vs GitHub API
- Se nova versão disponível → chama `ota-update.sh`

---

## 🔍 Verificar se o Timer está a funcionar

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
- `LEFT` = Tempo até próxima execução
- `LAST` = Última vez que correu
- `PASSED` = Há quanto tempo correu

### Ver os logs do polling:

```bash
# Logs do ota-check.sh (polling)
ssh root@10.21.220.191 "cat /opt/ota/logs/ota-check.log"
ssh root@10.21.220.192 "cat /opt/ota/logs/ota-check.log"

# Logs do ota-update.sh (update real)
ssh root@10.21.220.191 "cat /opt/ota/logs/ota.log"
ssh root@10.21.220.192 "cat /opt/ota/logs/ota.log"

# Ou via journalctl (últimos 30 min)
ssh root@10.21.220.191 "journalctl -u ota-check.service --since '30 min ago'"
ssh root@10.21.220.192 "journalctl -u ota-check.service --since '30 min ago'"
```

### Verificar a versão atual (confirmar se atualizou):

```bash
ssh root@10.21.220.191 "cat /etc/ota-version"   # RPi5
ssh root@10.21.220.192 "cat /etc/ota-version"   # RPi4
```

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
git tag v1.10.0
git push origin v1.10.0
```

### Check device versions:
```bash
ssh root@10.21.220.191 "cat /etc/ota-version"  # RPi5 (KUKSA)
ssh root@10.21.220.192 "cat /etc/ota-version"  # RPi4 (Cluster)
```

### Verificar estado do timer:
```bash
# Ver quando vai correr novamente
ssh root@10.21.220.191 "systemctl list-timers | grep ota"
ssh root@10.21.220.192 "systemctl list-timers | grep ota"
```

### Manual trigger (sem esperar pelo timer):
```bash
ssh root@10.21.220.191 "/opt/ota/ota-check.sh"
ssh root@10.21.220.192 "/opt/ota/ota-check.sh"
```

### View logs:
```bash
# Logs de polling
ssh root@10.21.220.191 "cat /opt/ota/logs/ota-check.log"
ssh root@10.21.220.192 "cat /opt/ota/logs/ota-check.log"

# Logs de update
ssh root@10.21.220.191 "cat /opt/ota/logs/ota.log"
ssh root@10.21.220.192 "cat /opt/ota/logs/ota.log"

# Via journalctl (últimos 30 min)
ssh root@10.21.220.191 "journalctl -u ota-check.service --since '30 min ago'"
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

## 🔧 Troubleshooting

### Problema: Timer corre mas update falha

**1. Verificar conectividade de rede:**
```bash
ssh root@<IP> "curl -s --max-time 5 https://api.github.com | head -1"
```

**2. Se falhar, verificar DNS:**
```bash
ssh root@<IP> "cat /etc/resolv.conf"
# Se vazio ou não existe, adicionar:
ssh root@<IP> "echo 'nameserver 8.8.8.8' > /etc/resolv.conf"
```

**3. Se DNS OK mas HTTPS falha ("certificate not yet valid"):**
```bash
# Problema: relógio do sistema está errado
ssh root@<IP> "date"
# Corrigir:
ssh root@<IP> "date -s '2026-02-13 14:00:00'"
```

**4. Verificar logs para detalhes:**
```bash
ssh root@<IP> "journalctl -u ota-check.service --since '1 hour ago'"
ssh root@<IP> "cat /opt/ota/logs/ota-check.log | tail -20"
```

### Problema: Versão não atualiza

```bash
# Verificar se auto-update está enabled
ssh root@<IP> "cat /etc/ota-auto-update"

# Se não estiver "enabled":
ssh root@<IP> "echo 'enabled' > /etc/ota-auto-update"

# Trigger manual para testar:
ssh root@<IP> "/opt/ota/ota-check.sh"
```

---

# Questions?

📡 **Team 6 — SEA:ME 2025-26**
