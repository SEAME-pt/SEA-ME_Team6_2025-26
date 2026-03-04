# 📡 OTA Implementation Guide — SEA:ME Team 6

**Last Updated:** 4 March 2026  
**Branch:** `feature/OTA/implementation`  
**Status:** ✅ Multi-Platform Tested (RPi4 + RPi5), RAUC Configured, Dual Implementation (tar.gz + RAUC)

---

## Table of Contents

1. [What is OTA?](#1-what-is-ota)
2. [OTA Types](#2-ota-types)
3. [Architecture Overview](#3-architecture-overview)
4. [What We Are Implementing](#4-what-we-are-implementing)
5. [Current Implementation Status](#5-current-implementation-status)
6. [Complete File Inventory](#6-complete-file-inventory)
7. [Implementation Details](#7-implementation-details)
8. [CI/CD Pipeline](#8-cicd-pipeline)
9. [How to Use](#9-how-to-use)
10. [Security Considerations](#10-security-considerations)
11. [Future Roadmap](#11-future-roadmap)
12. [Troubleshooting](#12-troubleshooting)
13. [Testing Strategy](#13-testing-strategy)
14. [FAQ - Frequently Asked Questions](#14-faq---frequently-asked-questions)
15. [References](#15-references)

---

## 1. What is OTA?

### 1.1 Definition

**Over-the-Air (OTA)** means transmitting data, commands, or updates without physical connection, using wireless communication.

In simple terms:
> Alter, update, or control a device remotely, via network (Wi-Fi, cellular, Bluetooth, LoRa, satellite, etc.).

### 1.2 Why OTA Matters

| Benefit | Description |
|---------|-------------|
| 🔧 **Updates** | Firmware, software, security patches, bug fixes |
| 📡 **Remote Config** | Change parameters, enable/disable features |
| 🔍 **Monitoring** | Collect logs, remote diagnostics, telemetry |
| 🤖 **Scale** | Manage thousands of devices without physical access |

### 1.3 Real-World Examples

- **Tesla** → Driving and safety updates
- **Apple/Google** → Mobile OS updates
- **AWS IoT / Azure IoT Hub** → OTA for millions of devices
- **Routers / Firewalls** → Critical security patches

### 1.4 Advantages

**Technical:**
- No physical access needed
- Fast updates
- Reduced downtime
- Immediate security fixes

**Economic:**
- Lower maintenance costs
- Fewer field technicians
- Global scale

**Strategic:**
- "Living" product, always evolving
- Rapid response to critical failures
- Continuous improvement

### 1.5 Risks & Challenges

| Risk | Description |
|------|-------------|
| ⚠️ **Security** | Man-in-the-Middle attacks, malicious firmware |
| ⚠️ **Reliability** | Interrupted update → bricked device |
| ⚠️ **Control** | Poorly tested updates affect millions |
| ⚠️ **Energy** | Battery consumption (critical in IoT) |

---

## 2. OTA Types

### 2.1 The Three OTA Categories

| Type | Full Name | Description | Our Implementation |
|------|-----------|-------------|-------------------|
| **FOTA** | Firmware OTA | Update firmware on MCUs | Planned (STM32) |
| **SOTA** | Software OTA | Update applications/services | ✅ Implemented |
| **COTA** | Configuration OTA | Update configs without code change | ✅ Implemented |

### 2.2 SOTA (Software OTA) — ✅ Implemented

Updates user-space software:
- Binaries (e.g., `kuksa` publisher)
- systemd services
- Qt applications
- Python scripts

**Currently implemented:**
- `can_to_kuksa_publisher` binary → installed in `/home/kuksa_RPi5/bin/`
- `can-to-kuksa.service` → systemd service updated via OTA
- `HelloQt6Qml` → Qt6 Cluster UI on RPi4

### 2.3 COTA (Configuration OTA) — ✅ Implemented

Updates configuration without reboot:
- VSS tree (`vss_min.json`)
- JSON/YAML configs
- Feature flags
- CAN parameters

**Currently implemented:**
- `vss_min.json` is included in `update-rpi5.tar.gz` and installed alongside the binary
- Allows remote update of VSS configuration without code changes

### 2.4 FOTA (Firmware OTA) — 📋 Planned

Updates firmware on STM32:
- Application only (not bootloader)
- Via AGL gateway over CAN/UART
- UDS-inspired protocol

**Not yet implemented.** This would be the next step:
- Send firmware `.bin` via CAN or UART to the STM32
- STM32 bootloader receives and flashes
- More complex as it requires custom bootloader on STM32

---

## 3. Architecture Overview

### 3.1 Multi-Platform Architecture

> **Update (Sprint 8):** The system now supports multi-platform OTA with separate packages for RPi4 (32-bit) and RPi5 (64-bit).

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
                            ▲
                            │
                    GitHub Actions
                    (Multi-Platform Build)
                    ┌─────────────────────────────┐
                    │ build-cluster-rpi4 (32-bit) │
                    │ build-kuksa-rpi5 (64-bit)   │
                    │ release (package & upload)  │
                    └─────────────────────────────┘
```

### 3.2 Platform Details

| Platform | Architecture | Component | Service | Docker SDK |
|----------|--------------|-----------|---------|------------|
| **RPi4** | ARM 32-bit (armv7l) | Qt6 Cluster UI | `helloqt-app.service` | `souzitaaaa/team6-agl-sdk:latest` |
| **RPi5** | ARM 64-bit (aarch64) | KUKSA CAN→VSS | `can-to-kuksa.service` | `souzitaaaa/team6-r5-agl-sdk:latest` |

### 3.3 Test Results (12 February 2026)

| Device | IP | `uname -m` | Package | Service | Status |
|--------|-----|------------|---------|---------|--------|
| RPi5 | 10.21.220.191 | `aarch64` | `update-rpi5.tar.gz` (260KB) | `can-to-kuksa.service` | ✅ Active, 0 restarts |
| RPi4 | 10.21.220.192 | `armv7l` | `update-rpi4.tar.gz` (4.6MB) | `helloqt-app.service` | ✅ Active, 0 restarts |

### 3.3 High-Level Architecture (Legacy Single-Platform)

```
┌──────────────────┐
│    Developer     │
└────────┬─────────┘
         │ git push / tag
         ▼
┌──────────────────────────┐
│    GitHub Actions        │
│  ────────────────────    │
│  • Build                 │
│  • Test                  │
│  • Package (tar.gz)      │
│  • Generate hash         │
│  • Create Release        │
└────────┬─────────────────┘
         │ HTTPS
         ▼
┌──────────────────────────┐
│   GitHub Releases        │
│   (OTA Server)           │
│  ────────────────────    │
│  • update.tar.gz         │
│  • hash.txt              │
└────────┬─────────────────┘
         │ HTTPS download
         ▼
┌──────────────────────────────────────────────┐
│           AGL Device (Raspberry Pi 5)        │
│  ──────────────────────────────────────────  │
│  OTA Agent (systemd service)                 │
│   • Poll / webhook                           │
│   • Download                                 │
│   • Verify (hash/signature)                  │
│   • Install                                  │
│   • Health check                             │
│   • Rollback (user-space)                    │
│                                              │
│  Services: kuksa, CAN apps, etc.             │
│                                              │
│  (Future) RAUC: rootfs, kernel, boot         │
└──────────────────────────────────────────────┘
         │ CAN / UART
         ▼
┌──────────────────────────┐
│   STM32U585 (ThreadX)    │
│   (Future FOTA target)   │
└──────────────────────────┘
```

### 3.2 Detailed OTA Flow (Target Architecture)

```
┌─────────────────────────────────────────────────────────────┐
│                      GITHUB                                 │
├─────────────────────────────────────────────────────────────┤
│  Developer pushes code                                      │
│       │                                                     │
│       ▼                                                     │
│  GitHub Actions Workflow                                    │
│   • Detect changes in src/kuksa/kuksa_RPi5/                │
│   • Cross-compile for ARM64 (aarch64)                      │
│   • Package: can_to_kuksa_publisher + vss_min.json         │
│   • Generate hash.txt                                       │
│   • Create GitHub Release (v1.0.x)                         │
└─────────────────────────────────────────────────────────────┘
                          │
                          │ HTTPS (GitHub Releases)
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                    AGL (Raspberry Pi 5)                     │
├─────────────────────────────────────────────────────────────┤
│  ota-polling.service (systemd timer)                        │
│   • Runs every X minutes                                    │
│   • Checks GitHub API for new release                      │
│   • Compares with /etc/ota-version                         │
│   • If new version:                                        │
│     → Download update.tar.gz                               │
│     → Verify hash                                          │
│     → Stop can-to-kuksa.service                            │
│     → Backup current binary                                │
│     → Install new binary                                   │
│     → Start can-to-kuksa.service                           │
│     → Health check                                         │
│     → Rollback if failed                                   │
└─────────────────────────────────────────────────────────────┘
```

### 3.3 Key Principle

> **AGL acts as OTA Gateway / Master ECU**

The STM32 MCU does NOT communicate directly with the cloud.
All updates flow through AGL, which:
- Centralizes security
- Manages downloads
- Controls update decisions

This aligns with automotive OEM architectures.

### 3.4 Separation of Responsibilities

| Layer | Responsible | Function |
|-------|-------------|----------|
| **CI/CD** | GitHub Actions | Build, test, sign |
| **Distribution** | OTA Server (GitHub Releases) | Store artifacts |
| **Execution** | OTA Agent (AGL) | Apply update |
| **Security** | Hash / signature | Integrity |
| **Robustness** | RAUC (future) | Atomicity |

---

## 4. What We Are Implementing

### 4.1 Implementation Strategy

We chose a **phased approach**:

| Phase | Description | Status |
|-------|-------------|--------|
| **Phase A** | OTA Manual with systemd | ✅ Complete |
| **Phase B** | SWUpdate / Enhanced rollback | ✅ Complete |
| **Phase C** | Atomic symlinks + auto-polling | ✅ Complete |
| **Phase D** | RAUC (A/B rootfs) | ✅ Configured |

#### Phase D Clarification: When to use RAUC vs OTA Scripts?

**Phase D (RAUC A/B rootfs)** was designed to support **two scenarios**:

| Scenario | Description | Recommended Method |
|----------|-------------|--------------------|
| **Production** | Cars with limited 4G, need efficient updates | OTA scripts for app patches, RAUC for major releases |
| **Development** | Lab environment with fast network | Can use only RAUC if simplicity is preferred |

**Our approach (Academic Exercise):** We implement **BOTH** methods:
- Maintain tar.gz (OTA scripts) → fast, lightweight app updates
- Add RAUC bundles in parallel → full system updates with A/B safety
- **Document comparative tests** (time, size, rollback, etc.)

### 4.2 Why This Order?

1. **Phase A** (Manual) — Proves the concept works with hello-ota test
2. **Phase B** (Enhanced) — Real binaries (KUKSA + Qt Cluster) with rollback
3. **Phase C** (Production) — Atomic symlinks + automatic polling
4. **Phase D** (RAUC) — A/B rootfs for full system updates

This approach:
- ✅ Minimizes risk
- ✅ Each phase is demonstrable
- ✅ Academically defensible
- ✅ Aligns with automotive best practices

### 4.3 What We Update

| Component | Type | Method | Status | Details |
|-----------|------|--------|--------|----------|
| `kuksa` publisher | SOTA | systemd + script | ✅ Working | `can_to_kuksa_publisher` on RPi5 |
| Qt Cluster app | SOTA | systemd + script | ✅ Working | `HelloQt6Qml` on RPi4 |
| VSS tree (`vss_min.json`) | COTA | File copy | ✅ Working | Included in `update-rpi5.tar.gz` |
| CAN services | SOTA | systemd | ✅ Working | `can-to-kuksa.service` |
| STM32 firmware | FOTA | CAN/UART | 📋 Planned | Requires STM32 bootloader |

---

## 5. Current Implementation Status

### 5.1 What's Working Now

#### On AGL (Raspberry Pi 5):

| Component | Path | Status |
|-----------|------|--------|
| OTA script | `/opt/ota/ota-update.sh` | ✅ |
| OTA directories | `/opt/ota/{downloads,releases,logs,backup,current}` | ✅ |
| Version file | `/etc/ota-version` | ✅ (v1.0.1) |
| systemd services | `/etc/systemd/system/ota-*.service` | ✅ |
| Hello OTA test | `/usr/bin/hello-ota.sh` | ✅ |

#### On GitHub:

| Component | Path | Status |
|-----------|------|--------|
| OTA workflow | `.github/workflows/ota.yml` | ✅ |
| Releases | GitHub Releases page | ✅ |

### 5.2 Directory Structure on AGL

```
/opt/ota/
├── backup/              # Previous version backup
│   └── hello-ota.sh
├── current/             # Current installed version
│   └── hello-ota.sh
├── downloads/           # Downloaded packages
│   └── update.tar.gz
├── logs/                # OTA operation logs
│   └── ota.log
├── releases/            # Extracted releases
│   ├── test.txt
│   └── v1.0.1/
│       └── hello-ota.sh
└── ota-update.sh        # Main OTA script
```

### 5.3 systemd Services

```bash
# Hello OTA test service
cat /etc/systemd/system/hello-ota.service
[Unit]
Description=Hello OTA Test Service
After=network.target

[Service]
Type=simple
ExecStart=/usr/bin/hello-ota.sh
Restart=always
RestartSec=2

[Install]
WantedBy=multi-user.target
```

```bash
# OTA Agent service
cat /etc/systemd/system/ota-agent.service
[Unit]
Description=OTA Update Agent
After=network-online.target
Wants=network-online.target

[Service]
Type=oneshot
ExecStart=/opt/ota/ota-update.sh
RemainAfterExit=true

[Install]
WantedBy=multi-user.target
```

### 5.4 Phase B: Enhanced Rollback (✅ Complete)

Phase B replaced the hello-ota proof-of-concept with real binaries:

| Task | Description | Status |
|------|-------------|--------|
| Install `kuksa` binary | Replace hello-ota with actual kuksa publisher | ✅ Done |
| Install Qt Cluster | HelloQt6Qml dashboard application | ✅ Done |
| CI/CD ARM64 cross-compile | GitHub Actions with Diogo's SDK | ✅ Done |
| Service-level rollback | Automatic rollback on service failure | ✅ Done |
| Version file tracking | `/etc/ota-version` with version history | ✅ Done |

**Phase B Deliverables:**
- [x] KUKSA `can_to_kuksa_publisher` (856KB ARM binary)
- [x] Qt Cluster `HelloQt6Qml` (13.5MB ARM binary)
- [x] GitHub Actions workflow with 3 parallel jobs
- [x] `ota-update.sh` script with backup/restore
- [x] v1.5.0 deployed on AGL

### 5.5 Phase A.2: Current Progress (Real-time Tracking)

> **Last Updated:** 2026-02-10 ✅ **COMPLETE**

#### A.2 Objective

Replace `hello-ota` proof-of-concept with actual binaries:
1. **`can_to_kuksa_publisher`** — KUKSA CAN-to-databroker publisher
2. **`HelloQt6Qml`** — Qt6 Cluster UI application

#### A.2 Implementation Steps

| Step | Task | Status | Notes |
|------|------|--------|-------|
| A.2.1 | Choose cross-compilation strategy | ✅ Done | Diogo's AGL SDK with ARM cross-compiler |
| A.2.2 | Update `.github/workflows/ota.yml` | ✅ Done | 3-job workflow (kuksa, cluster, release) |
| A.2.3 | Create workflow tags | ✅ Done | v1.1.0 → v1.5.0 (final working version) |
| A.2.4a | **Qt Cluster build** | ✅ Done | `HelloQt6Qml` (13.5MB ARM ELF) |
| A.2.4b | **KUKSA build** | ✅ Done | `can_to_kuksa_publisher` (856KB ARM ELF) |
| A.2.5 | Confirm artifacts in Release | ✅ Done | `update.tar.gz` + `hash.txt` |
| A.2.6 | Update `ota-update.sh` on AGL | ✅ Done | New script handles kuksa+cluster |
| A.2.7 | Test OTA download on AGL | ✅ Done | v1.5.0 installed successfully |
| A.2.8 | Verify service restart | ✅ Done | `can-to-kuksa.service` active |
| A.2.9 | Test rollback mechanism | ⬜ Pending | Force failure, verify recovery |

#### All Blockers Resolved ✅

- [x] ~~Workflow strategy~~ — Resolved: use Diogo's SDK (`souzitaaaa/team6-agl-sdk:latest`)
- [x] ~~Code sync~~ — Resolved: all branches synchronized (main, development, feature/OTA)
- [x] ~~Qt Cluster build~~ — Resolved: compiles successfully in CI
- [x] ~~KUKSA build~~ — Resolved: Diogo added gRPC to SDK image
- [x] ~~GitHub Release permissions~~ — Resolved: added `permissions: contents: write`

#### Final Workflow (v1.5.0) ✅

```
┌─────────────────┐   ┌──────────────────┐   ┌───────────────┐
│   build-kuksa   │   │  build-cluster   │   │    release    │
│    (✅ SUCCESS)  │   │    (✅ SUCCESS)   │   │  (✅ SUCCESS)  │
├─────────────────┤   ├──────────────────┤   ├───────────────┤
│ 856KB ARM ELF   │   │ 13.5MB ARM ELF   │   │ update.tar.gz │
│ + vss_min.json  │   │ HelloQt6Qml      │   │ + hash.txt    │
└─────────────────┘   └──────────────────┘   └───────────────┘
```

#### Installed on AGL (2026-02-10)

```bash
$ cat /etc/ota-version
v1.5.0

$ file /home/root/kuksa_RPi5/bin/can_to_kuksa_publisher
ELF 32-bit LSB pie executable, ARM, EABI5 version 1 (GNU/Linux)

$ file /opt/cluster/HelloQt6Qml  
ELF 32-bit LSB pie executable, ARM, EABI5 version 1 (GNU/Linux)

$ systemctl is-active can-to-kuksa.service
active
```

#### Key Files for A.2

| Location | File | Purpose |
|----------|------|---------|
| GitHub | `.github/workflows/ota.yml` | Multi-job ARM cross-compile workflow |
| Docker | `souzitaaaa/team6-agl-sdk:latest` | Diogo's SDK with Qt6 + gRPC + ARM toolchain |
| AGL | `/opt/ota/ota-update.sh` | Main OTA script (updated) |
| AGL | `/home/root/kuksa_RPi5/bin/can_to_kuksa_publisher` | KUKSA binary ✅ |
| AGL | `/home/root/kuksa_RPi5/vss_min.json` | VSS config ✅ |
| AGL | `/opt/cluster/HelloQt6Qml` | Qt Cluster binary ✅ |
| AGL | `/etc/systemd/system/can-to-kuksa.service` | KUKSA service ✅ |

#### OTA Package Contents (v1.5.0)

```
update.tar.gz
├── kuksa/
│   ├── bin/
│   │   └── can_to_kuksa_publisher   # 856KB ARM binary
│   └── vss_min.json                 # VSS tree config
└── cluster/
    └── HelloQt6Qml                  # 13.5MB ARM binary
```

---

## 6. Complete File Inventory

This section documents **ALL files and directories** involved in the OTA process, including who created them and their purpose.

### 6.0 Directory Structure: `/bin` vs `/usr/bin`

Understanding where to install binaries is important for OTA:

| Directory | Purpose | When to Use | Examples |
|-----------|---------|-------------|----------|
| `/bin` | **Essential system binaries** — required for boot and single-user mode | Core OS utilities | `ls`, `cp`, `cat`, `sh` |
| `/usr/bin` | **User/application binaries** — installed by user or packages | OTA scripts, applications, tools | `hello-ota.sh`, `kuksa` |

**For OTA:** We use `/usr/bin/` because our scripts are applications, not essential system utilities.

> ⚠️ **Note:** Never install OTA-managed binaries in `/bin` — that directory is for boot-essential commands only.

### 6.1 Files on AGL Device (Raspberry Pi 5)

#### 📁 `/opt/ota/` — OTA Working Directory

| File/Directory | Purpose | Created By | Notes |
|----------------|---------|------------|-------|
| `ota-update.sh` | Main OTA installation script | User (João) | Core script that handles download, verify, backup, install, rollback |
| `ota-update.sh~` | Backup of script (nano editor) | System | Auto-generated by nano |
| `backup/` | Stores previous version for rollback | Script | Created by `ota-update.sh` |
| `backup/hello-ota.sh` | Previous version of hello-ota | Script | Copied before update |
| `current/` | Currently installed version | Script | Active version symlink target |
| `current/hello-ota.sh` | Current hello-ota binary | Script | What's actually running |
| `downloads/` | Temporary download location | Script | Cleaned after install |
| `downloads/update.tar.gz` | Downloaded OTA package | Script | From GitHub Release |
| `logs/` | OTA operation logs | Script | Persistent logs |
| `logs/ota.log` | Main OTA log file | Script | All operations logged here |
| `releases/` | Extracted release versions | Script | Historical versions |
| `releases/v1.0.1/` | Specific version directory | Script | Contains extracted files |
| `releases/test.txt` | Test file | User (João) | Initial testing |

#### 📁 `/etc/` — System Configuration

| File | Purpose | Created By | Notes |
|------|---------|------------|-------|
| `/etc/ota-version` | Current OTA version marker | Script | Contains e.g., `v1.0.1` |
| `/etc/version` | AGL system version | AGL Build | System image version `20251103101437` |
| `/etc/systemd/system/hello-ota.service` | Hello OTA systemd unit | User (João) | Test service for OTA validation |
| `/etc/systemd/system/ota-agent.service` | OTA Agent systemd unit | User (João) | Triggers OTA updates |
| `/etc/systemd/system/ota-apply.service` | OTA Apply systemd unit | User (João) | Alternative trigger method |
| `/etc/systemd/system/can-to-kuksa.service` | CAN to KUKSA service | User (João) | Main application service |
| `/etc/systemd/system/can-heartbeat.service` | CAN heartbeat service | User (João) | CAN monitoring |
| `/etc/kuksa/` | KUKSA configuration directory | User (João) | KUKSA databroker configs |

#### 📁 `/usr/bin/` — System Binaries

| File | Purpose | Created By | Notes |
|------|---------|------------|-------|
| `/usr/bin/hello-ota.sh` | Hello OTA test executable | Script (OTA) | Copied from `/opt/ota/current/` |
| `/usr/bin/ota-update.sh` | OTA script (if installed here) | Optional | Alternative location |

#### 📁 `/bin/` — Core Binaries (AGL)

| File | Purpose | Created By | Notes |
|------|---------|------------|-------|
| `/bin/hello-ota.sh` | Hello OTA (symlink or copy) | Script | May exist here too |
| `/bin/ota-update.sh` | OTA update script copy | Script | Convenience symlink |
| `/bin/databroker` | KUKSA databroker binary | AGL Build | Core KUKSA component |
| `/bin/hash.txt` | Hash file (test artifact) | User (test) | From OTA testing |
| `/bin/update.tar.gz` | Update package (test) | User (test) | From OTA testing |

#### 📁 `/var/log/` — Runtime Logs

| File | Purpose | Created By | Notes |
|------|---------|------------|-------|
| `/var/log/hello-ota.log` | Hello OTA service output | hello-ota.sh | Appended on each run |
| `/var/log/mosquitto/` | MQTT broker logs | Mosquitto | If MQTT is used |

#### 📁 `/root/` — Root Home Directory

| File | Purpose | Created By | Notes |
|------|---------|------------|-------|
| `/root/SEAME_vss_tree.json` | VSS tree definition | User (João) | Vehicle Signal Specification |
| `/root/joystick_control.py` | Joystick control script | User | Manual control |
| `/root/test_can_actuation.sh` | CAN test script | User | Testing |
| `/root/test_can_motors.sh` | Motor test script | User | Testing |
| `/root/teste_can.py` | CAN test Python | User | Testing |

#### 📁 `/opt/scripts/` — Custom Scripts

| File | Purpose | Created By | Notes |
|------|---------|------------|-------|
| `/opt/scripts/` | Additional scripts | User | Custom automation |

### 6.2 Files on GitHub Repository

#### 📁 `.github/workflows/` — CI/CD Pipelines

| File | Purpose | Created By | Notes |
|------|---------|------------|-------|
| `ota.yml` | OTA build & release workflow | User (João) + AI Support | Triggers on `v*` tags |
| `tsf-validate.yml` | TSF validation workflow | User (João) + AI Support | TSF CI/CD |
| `daily-meeting.yml` | Daily meeting automation | User | Team workflow |
| `taskly.yml` | Task automation | User | Team workflow |

#### 📁 `src/kuksa/kuksa_RPi5/` — KUKSA Publisher Source

| File | Purpose | Created By | Notes |
|------|---------|------------|-------|
| `Makefile` | Build configuration | User (João) | Compiles `kuksa` binary |
| `kuksa` | Compiled binary (output) | Make | OTA artifact |
| `vss_min.json` | Minimal VSS tree | User (João) | COTA artifact |
| `src/` | Source code | User (João) | C/C++ sources |
| `generated/` | Generated headers | User (João) | From VSS |

#### 📁 `docs/guides/` — Documentation

| File | Purpose | Created By | Notes |
|------|---------|------------|-------|
| `OTA_Implementation_Guide.md` | This document | User (João) + AI Support | Complete OTA documentation |

### 6.3 OTA Artifacts (Generated)

| Artifact | Location | Purpose | Created By |
|----------|----------|---------|------------|
| `update.tar.gz` | GitHub Release | OTA package containing binaries/configs | GitHub Actions |
| `hash.txt` | GitHub Release | SHA-256 checksum of package | GitHub Actions |

### 6.4 Complete Path Reference Table

| Category | Path | Description |
|----------|------|-------------|
| **OTA Working Dir** | `/opt/ota/` | All OTA operations happen here |
| **OTA Script** | `/opt/ota/ota-update.sh` | Main update script |
| **OTA Logs** | `/opt/ota/logs/ota.log` | Operation logs |
| **OTA Downloads** | `/opt/ota/downloads/` | Temp download location |
| **OTA Releases** | `/opt/ota/releases/` | Version history |
| **OTA Backup** | `/opt/ota/backup/` | Rollback source |
| **OTA Current** | `/opt/ota/current/` | Active version |
| **Version Marker** | `/etc/ota-version` | Current version string |
| **systemd Units** | `/etc/systemd/system/` | Service definitions |
| **Installed Binary** | `/usr/bin/hello-ota.sh` | Active executable |
| **Service Logs** | `/var/log/hello-ota.log` | Service output |
| **GitHub Workflow** | `.github/workflows/ota.yml` | CI/CD pipeline |
| **Source Code** | `src/kuksa/kuksa_RPi5/` | Build source |

### 6.5 File Flow During OTA Update

```
GitHub Repository
       │
       ├─ src/kuksa/kuksa_RPi5/
       │   ├─ Makefile
       │   ├─ src/*.c
       │   └─ vss_min.json
       │
       ▼ (git tag v1.0.2 → triggers workflow)
       
GitHub Actions (.github/workflows/ota.yml)
       │
       ├─ make → kuksa binary
       ├─ tar -czf → update.tar.gz
       └─ sha256sum → hash.txt
       │
       ▼ (uploaded to GitHub Release)
       
GitHub Release (https://github.com/.../releases/download/v1.0.2/)
       │
       ├─ update.tar.gz
       └─ hash.txt
       │
       ▼ (curl download by OTA script)
       
AGL Device (/opt/ota/)
       │
       ├─ downloads/update.tar.gz    ← Downloaded here
       ├─ releases/v1.0.2/           ← Extracted here
       ├─ backup/                    ← Previous version backed up
       ├─ current/                   ← New version installed
       └─ logs/ota.log               ← Operation logged
       │
       ▼ (cp to system location)
       
System Locations
       │
       ├─ /usr/bin/hello-ota.sh      ← Binary installed
       ├─ /etc/ota-version           ← Version updated to "v1.0.2"
       └─ systemctl restart          ← Service restarted
```

### 6.6 Ownership Summary

| Created By | Files |
|------------|-------|
| **User (João)** | `ota-update.sh`, systemd services, test scripts, source code, Makefile |
| **User (João) + AI Support** | `OTA_Implementation_Guide.md`, workflow improvements, documentation |
| **GitHub Actions** | `update.tar.gz`, `hash.txt` (generated artifacts) |
| **OTA Script** | `backup/`, `current/`, `releases/`, `logs/ota.log`, `/etc/ota-version` |
| **AGL Build** | System binaries, `/etc/version`, base OS files |

---

## 7. Implementation Details

### 7.1 OTA Update Script

**Location:** `/opt/ota/ota-update.sh`

```bash
#!/bin/bash
set -e

# -------- ARGUMENTS --------
if [ $# -ne 3 ]; then
  echo "Usage: ota-update.sh <VERSION> <URL> <HASH>"
  exit 1
fi

VERSION="$1"
URL="$2"
HASH="$3"

# -------- PATHS --------
WORKDIR="/opt/ota"
LOG="$WORKDIR/logs/ota.log"
DL="$WORKDIR/downloads/update.tar.gz"
RELEASE="$WORKDIR/releases/$VERSION"
CURRENT="$WORKDIR/current"
BACKUP="$WORKDIR/backup"

SERVICE_NAME="hello-ota"
BIN_DST="/usr/bin/hello-ota.sh"

mkdir -p "$WORKDIR/logs" "$WORKDIR/downloads" "$WORKDIR/releases"

echo "=== OTA update $VERSION ===" >> "$LOG"

# -------- DOWNLOAD --------
echo "[1] Downloading package" >> "$LOG"
curl -fL "$URL" -o "$DL"

# -------- VERIFY HASH --------
echo "[2] Verifying hash" >> "$LOG"
echo "$HASH  $DL" | sha256sum -c -

# -------- BACKUP --------
echo "[3] Backup current version" >> "$LOG"
rm -rf "$BACKUP"
if [ -d "$CURRENT" ]; then
  cp -r "$CURRENT" "$BACKUP"
fi

# -------- EXTRACT --------
echo "[4] Extracting update" >> "$LOG"
rm -rf "$RELEASE"
mkdir -p "$RELEASE"
tar -xzf "$DL" -C "$RELEASE"

# -------- APPLY --------
echo "[5] Applying update" >> "$LOG"
systemctl stop "$SERVICE_NAME"

rm -rf "$CURRENT"
cp -r "$RELEASE" "$CURRENT"

cp "$CURRENT/hello-ota.sh" "$BIN_DST"
chmod +x "$BIN_DST"

systemctl start "$SERVICE_NAME"

# -------- HEALTH CHECK --------
sleep 2
if ! systemctl is-active --quiet "$SERVICE_NAME"; then
  echo "[ERROR] Service failed, rolling back" >> "$LOG"

  systemctl stop "$SERVICE_NAME"
  rm -rf "$CURRENT"
  cp -r "$BACKUP" "$CURRENT"
  cp "$CURRENT/hello-ota.sh" "$BIN_DST"
  chmod +x "$BIN_DST"
  systemctl start "$SERVICE_NAME"

  echo "rollback" > /etc/ota-version
  exit 1
fi

# -------- SUCCESS --------
echo "$VERSION" > /etc/ota-version
echo "Update $VERSION successful" >> "$LOG"
```

### 7.2 OTA Flow

```
1. Download package from GitHub Release
         ↓
2. Verify SHA-256 hash
         ↓
3. Backup current version
         ↓
4. Extract new version
         ↓
5. Stop service
         ↓
6. Copy new files
         ↓
7. Start service
         ↓
8. Health check
         ↓
   ┌─────┴─────┐
   │           │
  OK?        FAIL?
   │           │
   ▼           ▼
Update     Rollback
version    to backup
```

### 7.3 Enhanced Health Check Features (Sprint 8)

> **New in Sprint 8:** The OTA script now includes architecture verification and restart loop detection.

#### 7.3.1 Architecture Verification

The script verifies that binaries match the system architecture before installation:

```bash
verify_binary_arch() {
    local binary="$1"
    local expected_arch=$(uname -m)  # e.g., "aarch64" or "armv7l"
    
    # Use file command to check binary architecture
    local file_output=$(file "$binary")
    
    case "$expected_arch" in
        aarch64)
            if [[ ! "$file_output" =~ "ARM aarch64" ]]; then
                log "ERROR" "Binary is not ARM 64-bit!"
                return 1
            fi
            ;;
        armv7l|armv7*)
            if [[ ! "$file_output" =~ "ARM" ]] || [[ "$file_output" =~ "aarch64" ]]; then
                log "ERROR" "Binary is not ARM 32-bit!"
                return 1
            fi
            ;;
    esac
    return 0
}
```

**Why this matters:**
- Prevents installing 32-bit binaries on 64-bit systems (and vice versa)
- Catches CI/CD misconfigurations early
- Avoids `status=203/EXEC` service failures

#### 7.3.2 Restart Loop Detection

The script detects services stuck in restart loops:

```bash
check_service_health() {
    local service="$1"
    local max_wait=30     # Max seconds to wait
    local restart_window=10  # Window to check for restart loops
    local max_restarts=3     # Max restarts before failure
    
    sleep 3  # Initial wait for service to start
    
    # Check if active
    if ! systemctl is-active --quiet "$service"; then
        return 1
    fi
    
    # Check for restart loop
    local restarts=$(systemctl show "$service" --property=NRestarts --value 2>/dev/null || echo "0")
    sleep "$restart_window"
    local restarts_after=$(systemctl show "$service" --property=NRestarts --value 2>/dev/null || echo "0")
    
    local restart_diff=$((restarts_after - restarts))
    if [ "$restart_diff" -ge "$max_restarts" ]; then
        log "ERROR" "Service in restart loop ($restart_diff restarts in ${restart_window}s)"
        return 1
    fi
    
    return 0
}
```

**Why this matters:**
- Services with `Restart=always` can appear "active" even while crashing repeatedly
- Simple `systemctl is-active` misses restart loops
- Early detection prevents running broken software

#### 7.3.3 OTA Update Flow (v2)

The updated flow now includes 10 steps:

```
1. Download package from GitHub Release
         ↓
2. Verify SHA-256 hash
         ↓
3. Extract to /opt/ota/releases/vX.X.X
         ↓
4. Stop services
         ↓
5. Record previous version
         ↓
6. Update symlink atomically
         ↓
7. Verify binary architecture ← NEW
         ↓
8. Install binaries
         ↓
9. Start services
         ↓
10. Health check (restart loop detection) ← NEW
         ↓
   ┌─────┴─────┐
   │           │
  OK?        FAIL?
   │           │
   ▼           ▼
Update     Rollback
version    to backup
```

---

## 8. Complete OTA Workflow

### 8.0 Workflow Overview

O sistema OTA é **semi-automático**:
- **CI/CD (GitHub Actions):** ✅ Automático quando fazes `git push --tags`
- **Dispositivo (AGL):** Manual ou Automático (com timer polling)

### 8.1 Workflow Visual

```
┌─────────────────────────────────────────────────────────────────────────┐
│ PARTE 1: CI/CD (GitHub Actions) - AUTOMÁTICO                            │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  Developer                    GitHub Actions              GitHub Releases│
│      │                              │                           │       │
│      │  git tag v1.9.0              │                           │       │
│      │  git push origin v1.9.0      │                           │       │
│      │ ────────────────────────────►│                           │       │
│      │                              │                           │       │
│      │                        ┌─────┴─────┐                     │       │
│      │                        │ TRIGGER:  │                     │       │
│      │                        │ on: push  │                     │       │
│      │                        │   tags:   │                     │       │
│      │                        │   - "v*"  │                     │       │
│      │                        └─────┬─────┘                     │       │
│      │                              │                           │       │
│      │                        build-rpi4 (32-bit)               │       │
│      │                        build-rpi5 (64-bit)               │       │
│      │                              │                           │       │
│      │                              │ upload artifacts          │       │
│      │                              │──────────────────────────►│       │
│      │                              │                           │       │
│      │                              │       update-rpi4.tar.gz  │       │
│      │                              │       update-rpi5.tar.gz  │       │
│      │                              │       hash-*.txt          │       │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────┐
│ PARTE 2: Dispositivo (AGL) - MANUAL ou AUTOMÁTICO                       │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│                    GitHub Releases                    AGL Device        │
│                          │                                │             │
│  ┌───────────────────────┼────────────────────────────────┼───────────┐ │
│  │ OPÇÃO A: MANUAL       │                                │           │ │
│  │                       │      ssh + run script          │           │ │
│  │                       │◄───────────────────────────────│           │ │
│  │                       │  /opt/ota/ota-update.sh v1.9.0 │           │ │
│  └───────────────────────┼────────────────────────────────┼───────────┘ │
│                          │                                │             │
│  ┌───────────────────────┼────────────────────────────────┼───────────┐ │
│  │ OPÇÃO B: AUTOMÁTICO   │                                │           │ │
│  │ (timer polling)       │      ota-check.timer           │           │ │
│  │                       │◄───────────────────────────────│(15 min)   │ │
│  │                       │      checks GitHub API         │           │ │
│  │                       │      for new releases          │           │ │
│  └───────────────────────┼────────────────────────────────┼───────────┘ │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 8.2 Component Status

| Componente | Ficheiro | Status | Descrição |
|------------|----------|--------|-----------|
| **CI/CD Build** | `.github/workflows/ota.yml` | ✅ Automático | Trigger em tags `v*` |
| **Update Script** | `/opt/ota/ota-update.sh` | ✅ Funciona | Download, install, rollback |
| **Polling Timer** | `ota-check.timer` | ⚠️ Precisa ativar | Verifica GitHub a cada 15 min |
| **Polling Script** | `ota-check.sh` | ⚠️ Precisa instalar | Script que chama o update |

### 8.3 Detailed Flow (4 Phases)

#### FASE 1: Developer faz alterações

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

#### FASE 2: GitHub Actions (Automático)

```yaml
# .github/workflows/ota.yml
on:
  push:
    tags:
      - "v*"   # ◄─── Dispara quando tag v* é pushed
```

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
│                         ▼                                               │
│   GitHub Releases: v1.9.0                                               │
│   ├── update-rpi4.tar.gz (4.6 MB)                                       │
│   ├── hash-rpi4.txt                                                     │
│   ├── update-rpi5.tar.gz (260 KB)                                       │
│   ├── hash-rpi5.txt                                                     │
│   ├── update.tar.gz (combined)                                          │
│   └── hash.txt                                                          │
│                                                                         │
│   Tempo estimado: ~3-5 minutos                                          │
└─────────────────────────────────────────────────────────────────────────┘
```

#### FASE 3: Dispositivo AGL (Polling Automático)

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

#### FASE 4: Update Script (Automático se auto-update enabled)

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

## 9. Multi-Platform CI/CD Pipeline

### 9.0 Multi-Platform Workflow (Sprint 8)

> **Updated:** The workflow now supports building for both RPi4 (32-bit) and RPi5 (64-bit) platforms.

**Location:** `.github/workflows/ota.yml`

```yaml
name: OTA Build & Release (Multi-Platform)

on:
  push:
    tags:
      - "v*"
  workflow_dispatch:
    inputs:
      platform:
        description: 'Target platform'
        type: choice
        options:
          - both
          - rpi4
          - rpi5
        default: 'both'

env:
  SDK_IMAGE_RPI4: souzitaaaa/team6-agl-sdk:rpi4
  SDK_IMAGE_RPI5: souzitaaaa/team6-agl-sdk:rpi5

jobs:
  build-kuksa-rpi5:    # ARM 64-bit (aarch64)
  build-cluster-rpi4:  # ARM 32-bit (armv7)
  release:             # Package and upload
```

### 8.0.1 Multi-Platform Build Jobs

| Job | Platform | Architecture | SDK Image | Output |
|-----|----------|--------------|-----------|--------|
| `build-kuksa-rpi5` | RPi5 | aarch64 (64-bit) | `team6-agl-sdk:rpi5` | `can_to_kuksa_publisher` |
| `build-cluster-rpi4` | RPi4 | armv7 (32-bit) | `team6-agl-sdk:rpi4` | `HelloQt6Qml` |

### 8.0.2 Release Packages

| Package | Contents | Platform |
|---------|----------|----------|
| `update-rpi4.tar.gz` | Qt6 Cluster binary | RPi4 (32-bit) |
| `update-rpi5.tar.gz` | KUKSA binary + VSS | RPi5 (64-bit) |
| `update.tar.gz` | Both platforms | Combined |

### 8.1 GitHub Actions Workflow (Legacy)

**Location:** `.github/workflows/ota.yml`

```yaml
name: OTA Build & Deploy/Release

on:
  push:
    tags:
      - "v*"

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
      - uses: actions/checkout@v4

      - name: Build kuksa_RPi5
        run: |
          cd src/kuksa/kuksa_RPi5
          make
          
      - name: Package OTA
        run: |
          mkdir -p ota
          cp src/kuksa/kuksa_RPi5/kuksa ota/
          cp src/kuksa/kuksa_RPi5/vss_min.json ota/
          tar -czf update.tar.gz ota/
          
      - name: Hash
        run: sha256sum update.tar.gz > hash.txt

      - name: Upload Release
        uses: softprops/action-gh-release@v1
        with:
          files: |
            update.tar.gz
            hash.txt
```

### 8.2 Workflow Trigger

The workflow triggers on tags matching `v*`:

```bash
# Create and push tag
git tag v1.0.2
git push origin v1.0.2
```

### 8.3 Release Artifacts

After workflow completes, GitHub Release contains:
- `update.tar.gz` — The OTA package
- `hash.txt` — SHA-256 checksum

### 8.4 Cross-Compilation Strategy

#### The Challenge

The KUKSA publisher (`can_to_kuksa_publisher`) must run on **ARM64** (Raspberry Pi 5), but GitHub Actions runners use **x86_64**. This requires either cross-compilation or emulation.

#### What is QEMU?

**QEMU** (Quick EMUlator) is an open-source emulator that can run code compiled for one architecture on a different architecture. In CI/CD, QEMU allows running ARM64 binaries on x86_64 runners by emulating the ARM processor.

#### Build Options Considered

| Option | Description | Build Time | Complexity | Status |
|--------|-------------|------------|------------|--------|
| **A) QEMU + Docker** | Run ARM64 Docker container via QEMU | ~5-10 min | Low | ✅ **CHOSEN** |
| **B) Native Cross-Compile** | Use ARM64 toolchain directly | ~1-2 min | High | ❌ Rejected |
| **C) Self-Hosted Runner** | CI on actual ARM64 hardware | ~1-2 min | Very High | ❌ Rejected |

#### Decision: Option A (QEMU + Docker ARM64)

We chose **QEMU emulation with Docker ARM64** for the following reasons:

**Pros:**
| Advantage | Description |
|-----------|-------------|
| ✅ **Simplicity** | No complex toolchain setup required |
| ✅ **Identical Environment** | Compiles in same environment as target (Debian ARM64) |
| ✅ **Package Availability** | Uses native ARM64 packages (gRPC, protobuf) |
| ✅ **Maintainability** | Standard Makefile works without modification |
| ✅ **Reproducibility** | Same container = same results every time |

**Cons:**
| Disadvantage | Mitigation |
|--------------|------------|
| ⚠️ Slower (~5-10 min) | Acceptable for our release frequency |
| ⚠️ QEMU emulation overhead | Build happens in CI, not blocking development |

#### Why We Rejected Option B (Native Cross-Compile)

| Reason | Explanation |
|--------|-------------|
| ❌ **gRPC Complexity** | Would need to cross-compile gRPC (~30+ min first build) |
| ❌ **Library Compatibility** | Risk of ABI mismatches between host and target |
| ❌ **Toolchain Setup** | Complex CMake toolchain files required |
| ❌ **Debugging Difficulty** | Cross-compile errors harder to diagnose |

#### Why We Rejected Option C (Self-Hosted Runner)

| Reason | Explanation |
|--------|-------------|
| ❌ **Hardware Dependency** | RPi must be online and connected |
| ❌ **Security Risk** | Self-hosted runners have security implications |
| ❌ **Maintenance Overhead** | Need to maintain runner software |
| ❌ **Not Portable** | Tied to specific hardware |

---

### 8.5 Workflow Templates (Archive)

This section archives the workflow configurations for each option, for future reference.

#### Template A: QEMU + Docker (CURRENT - In Use)

**File:** `.github/workflows/ota.yml`

```yaml
name: OTA Build & Release (ARM64)

on:
  push:
    tags:
      - "v*"
  workflow_dispatch:
    inputs:
      version:
        description: 'Version tag (e.g., v1.0.3)'
        required: true
        default: 'v1.0.3'

jobs:
  build-arm64:
    runs-on: ubuntu-22.04
    
    steps:
      - name: Checkout repository
        uses: actions/checkout@v4

      - name: Set up QEMU for ARM64
        uses: docker/setup-qemu-action@v3
        with:
          platforms: arm64

      - name: Set up Docker Buildx
        uses: docker/setup-buildx-action@v3

      - name: Build in ARM64 container
        run: |
          docker run --rm --platform linux/arm64 \
            -v ${{ github.workspace }}:/workspace \
            -w /workspace/src/kuksa/kuksa_RPi5 \
            arm64v8/debian:bookworm-slim \
            bash -c "
              apt-get update && \
              apt-get install -y g++ make pkg-config libgrpc++-dev libprotobuf-dev protobuf-compiler-grpc && \
              make clean || true && \
              make && \
              file bin/can_to_kuksa_publisher
            "

      - name: Package OTA
        run: |
          mkdir -p ota
          cp src/kuksa/kuksa_RPi5/bin/can_to_kuksa_publisher ota/
          cp src/kuksa/kuksa_RPi5/vss_min.json ota/
          tar -czf update.tar.gz -C ota .
          
      - name: Generate hash
        run: sha256sum update.tar.gz > hash.txt

      - name: Upload Release
        uses: softprops/action-gh-release@v1
        with:
          files: |
            update.tar.gz
            hash.txt
```

#### Template B: Native Cross-Compile (NOT USED - Archive)

This template was designed but **not implemented** due to complexity:

```yaml
name: OTA Build & Release (ARM64 Cross-Compile)

on:
  push:
    tags:
      - "v*"

env:
  GRPC_VERSION: "1.60.0"
  TARGET_ARCH: aarch64-linux-gnu

jobs:
  build-arm64:
    runs-on: ubuntu-22.04
    
    steps:
      - name: Checkout repository
        uses: actions/checkout@v4

      - name: Install cross-compilation toolchain
        run: |
          sudo apt-get update
          sudo apt-get install -y \
            gcc-aarch64-linux-gnu \
            g++-aarch64-linux-gnu \
            binutils-aarch64-linux-gnu \
            pkg-config cmake ninja-build git

      - name: Cache gRPC ARM64 build
        id: cache-grpc
        uses: actions/cache@v4
        with:
          path: ~/grpc-arm64
          key: grpc-arm64-${{ env.GRPC_VERSION }}-v2

      - name: Build gRPC for ARM64 (if not cached)
        if: steps.cache-grpc.outputs.cache-hit != 'true'
        run: |
          # Clone gRPC with submodules
          git clone --recurse-submodules -b v${{ env.GRPC_VERSION }} --depth 1 \
            https://github.com/grpc/grpc.git ~/grpc-src
          
          # Build protoc for host first
          mkdir -p ~/grpc-host-build && cd ~/grpc-host-build
          cmake -G Ninja -DCMAKE_BUILD_TYPE=Release \
            -DgRPC_BUILD_TESTS=OFF ~/grpc-src
          ninja -j$(nproc) grpc_cpp_plugin protoc
          
          # Create ARM64 toolchain file
          cat > ~/arm64-toolchain.cmake << 'EOF'
          set(CMAKE_SYSTEM_NAME Linux)
          set(CMAKE_SYSTEM_PROCESSOR aarch64)
          set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
          set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
          set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
          set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
          set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
          EOF
          
          # Build gRPC for ARM64
          mkdir -p ~/grpc-arm64-build && cd ~/grpc-arm64-build
          cmake -G Ninja \
            -DCMAKE_TOOLCHAIN_FILE=~/arm64-toolchain.cmake \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=~/grpc-arm64 \
            -DgRPC_BUILD_TESTS=OFF \
            -D_gRPC_CPP_PLUGIN=~/grpc-host-build/grpc_cpp_plugin \
            ~/grpc-src
          ninja -j$(nproc) && ninja install

      - name: Build KUKSA Publisher for ARM64
        run: |
          cd src/kuksa/kuksa_RPi5
          
          # Use cross-compiler with ARM64 gRPC
          aarch64-linux-gnu-g++ -std=c++17 -O2 \
            -I$HOME/grpc-arm64/include -Iinc -Igenerated \
            src/*.cpp src/handlers/*.cpp generated/kuksa/val/v2/*.cc \
            -L$HOME/grpc-arm64/lib \
            -lgrpc++ -lgrpc -lgpr -lprotobuf -lpthread \
            -static-libstdc++ -static-libgcc \
            -o bin/can_to_kuksa_publisher
          
          file bin/can_to_kuksa_publisher

      - name: Package and Release
        run: |
          mkdir -p ota
          cp src/kuksa/kuksa_RPi5/bin/can_to_kuksa_publisher ota/
          tar -czf update.tar.gz -C ota .
          sha256sum update.tar.gz > hash.txt

      - uses: softprops/action-gh-release@v1
        with:
          files: |
            update.tar.gz
            hash.txt
```

**Why this was rejected:**
- First build of gRPC for ARM64 takes ~30+ minutes
- Complex CMake toolchain configuration
- Risk of ABI mismatches
- Cache invalidation issues

#### Template C: Hello-OTA Test (Phase A.1 - Historical)

This was the original proof-of-concept workflow used with `hello-ota.sh`:

```yaml
name: OTA Build & Deploy/Release (Hello-OTA Test)

on:
  push:
    tags:
      - "v*"

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
      - uses: actions/checkout@v4

      - name: Create hello-ota script
        run: |
          mkdir -p ota
          cat > ota/hello-ota.sh << 'EOF'
          #!/bin/bash
          while true; do
            echo "[$(date)] Hello from OTA version $OTA_VERSION" >> /var/log/hello-ota.log
            sleep 10
          done
          EOF
          chmod +x ota/hello-ota.sh
          
      - name: Package OTA
        run: tar -czf update.tar.gz -C ota .
          
      - name: Hash
        run: sha256sum update.tar.gz > hash.txt

      - name: Upload Release
        uses: softprops/action-gh-release@v1
        with:
          files: |
            update.tar.gz
            hash.txt
```

**Purpose:** Validated the entire OTA pipeline (GitHub Actions → Release → Download → Install → Rollback) before implementing with real binaries.

---

### 8.6 Dependencies for Cross-Compile

For reference, the KUKSA publisher requires these libraries:

| Library | Purpose | ARM64 Package (Debian) |
|---------|---------|------------------------|
| gRPC++ | gRPC C++ framework | `libgrpc++-dev` |
| protobuf | Protocol Buffers | `libprotobuf-dev` |
| protoc-grpc | gRPC code generator | `protobuf-compiler-grpc` |
| pthread | POSIX threads | Built-in |

---

## 9. How to Use

### 9.1 Creating a New Release

**Step 1:** Make your changes and commit
```bash
git add .
git commit -m "feat: Add new feature"
git push
```

**Step 2:** Create a tag
```bash
git tag v1.0.2
git push origin v1.0.2
```

**Step 3:** Wait for GitHub Actions to complete

**Step 4:** Verify release at:
```
https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/releases
```

### 9.2 Deploying to AGL

**Option A: Manual Deploy (SSH)**

```bash
# SSH to AGL device
ssh root@<AGL_IP>

# Download release
VERSION="v1.0.2"
REPO="SEAME-pt/SEA-ME_Team6_2025-26"

curl -L -o /tmp/update.tar.gz \
  "https://github.com/$REPO/releases/download/$VERSION/update.tar.gz"

curl -L -o /tmp/hash.txt \
  "https://github.com/$REPO/releases/download/$VERSION/hash.txt"

# Get hash
HASH=$(cut -d' ' -f1 /tmp/hash.txt)

# Run OTA update
/opt/ota/ota-update.sh "$VERSION" \
  "https://github.com/$REPO/releases/download/$VERSION/update.tar.gz" \
  "$HASH"
```

**Option B: One-liner from Mac**

```bash
ssh root@10.21.220.191 \
  "/opt/ota/ota-update.sh v1.0.2 \
   https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/releases/download/v1.0.2/update.tar.gz \
   \$(curl -sL https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/releases/download/v1.0.2/hash.txt | cut -d' ' -f1)"
```

### 9.3 Verifying Update

```bash
# Check version
cat /etc/ota-version

# Check logs
cat /opt/ota/logs/ota.log

# Check service status
systemctl status hello-ota

# Check service logs
cat /var/log/hello-ota.log
```

### 9.4 Device Setup (One-Time)

Para configurar o OTA automático num dispositivo AGL, usa o script de setup:

**Opção A: Script de Setup Automático (Recomendado)**

```bash
# Do teu Mac, copia e executa o script de setup:
scp /path/to/project/src/ota/scripts/setup-ota-device.sh root@<DEVICE_IP>:/tmp/
ssh root@<DEVICE_IP> "chmod +x /tmp/setup-ota-device.sh && /tmp/setup-ota-device.sh"
```

**Opção B: Setup Manual**

```bash
# SSH para o dispositivo
ssh root@<DEVICE_IP>

# Criar directórios
mkdir -p /opt/ota/{logs,downloads,releases,backup}

# Copiar scripts (do Mac)
scp src/ota/scripts/ota-update.sh root@<IP>:/opt/ota/
scp src/ota/scripts/ota-check.sh root@<IP>:/opt/ota/
scp src/ota/systemd/ota-check.service root@<IP>:/etc/systemd/system/
scp src/ota/systemd/ota-check.timer root@<IP>:/etc/systemd/system/

# No dispositivo, ativar o timer
chmod +x /opt/ota/*.sh
systemctl daemon-reload
systemctl enable ota-check.timer
systemctl start ota-check.timer

# Ativar auto-update automático
echo "enabled" > /etc/ota-auto-update
```

**Verificar setup:**

```bash
# Ver próxima execução do timer
systemctl list-timers | grep ota

# Ver logs do polling
journalctl -u ota-check.service -f

# Testar manualmente
/opt/ota/ota-check.sh
```

---

## 10. Security Considerations

### 10.1 Current Security Measures

| Measure | Description | Status |
|---------|-------------|--------|
| **HTTPS** | Encrypted download from GitHub | ✅ |
| **SHA-256** | Hash verification | ✅ |
| **Versioning** | Explicit version tracking | ✅ |
| **Logs** | Persistent operation logs | ✅ |
| **Rollback** | Automatic on service failure | ✅ |

### 9.2 Known Limitations

| Limitation | Description | Mitigation |
|------------|-------------|------------|
| No code signing | Artifacts not cryptographically signed | Use private repo + HTTPS |
| User-space only | No kernel/boot protection | Acceptable for academic project |
| Manual trigger | No automatic polling | Future: webhook/cron |

### 10.3 Future Security Enhancements

- [ ] RSA/ECC signature verification
- [ ] TLS client certificates
- [ ] Secure boot chain
- [ ] Encrypted artifacts

---

## 11. Future Roadmap

### 11.1 Phase B: Enhanced OTA ✅ Complete

Real binaries with CI/CD cross-compilation:
- [x] KUKSA `can_to_kuksa_publisher` binary
- [x] Qt Cluster `HelloQt6Qml` application
- [x] GitHub Actions ARM64 workflow
- [x] Service-level rollback
- [x] Version tracking in `/etc/ota-version`

### 11.2 Phase C: Atomic Symlinks + Auto-polling ✅ Complete

Production-ready OTA with zero-downtime updates:
- [x] Atomic symlink switching
- [x] systemd timer for automatic polling
- [x] GitHub API integration for version checks
- [x] Version history with improved rollback
- [x] Qt Cluster service (Wayland)

### 11.3 Phase D: RAUC Integration ✅ Configured (13 Feb 2026)

RAUC A/B rootfs update system is now configured on both devices:

**What is RAUC?**
- Robust Auto-Update Controller for atomic system updates
- Uses A/B partition scheme for safe updates
- Automatic rollback on boot failure

**Current Configuration:**
- [x] RAUC installed (v1.15.1) on both devices
- [x] Partitions prepared (rootfs-A at p2, rootfs-B at p3)
- [x] Custom bootloader backend for native RPi bootloader
- [x] Configuration files deployed
- [x] SSL certificates generated for bundle signing
- [x] Installation scripts created (`install-bundle.sh`)
- [x] Post-reboot verification scripts created (`post-reboot-verify.sh`)
- [ ] Bundle creation workflow (Ruben - in progress)
- [ ] Full A/B switch test with reboot

**Device Status:**

| Device | Compatible | Active Slot | Boot Status |
|--------|------------|-------------|-------------|
| RPi5 | seame-team6-rpi5 | rootfs.0 (A) | ✅ good |
| RPi4 | seame-team6-rpi4 | rootfs.0 (A) | ✅ good |

**Partition Layout:**

```
┌─────────────────────────────────────────────────────────────────┐
│                         SD Card                                  │
├─────────┬─────────────────┬─────────────────┬──────────────────┤
│  boot   │   rootfs-A (p2) │   rootfs-B (p3) │     data (p4)    │
│  (p1)   │   [ACTIVE]      │   [INACTIVE]    │   [PERSISTENT]   │
│         │                 │                 │                  │
│ config  │  Sistema atual  │  Para updates   │  - configs       │
│ kernel  │  (AGL running)  │  RAUC escreve   │  - logs          │
│         │                 │  aqui           │  - certs         │
│         │                 │                 │  - user data     │
└─────────┴─────────────────┴─────────────────┴──────────────────┘
         RAUC gere ◄────────────────────────► RAUC ignora
```

**What RAUC Updates vs What Persists:**

| Location | Updated by RAUC? | Description |
|----------|------------------|-------------|
| `/etc/` | ✅ Yes | System configs (OVERWRITTEN!) |
| `/opt/` | ✅ Yes | Applications (OVERWRITTEN!) |
| `/home/` | ✅ Yes | User data in rootfs (OVERWRITTEN!) |
| `/data/` (p4) | ❌ No | **Persistent** - survives updates |

⚠️ **Important:** Custom configurations in `/etc/` or `/opt/` **will be lost** after RAUC update. Use `/data/` partition for persistent configs.

**Files Created:**
```
src/ota/rauc/
├── system.conf.rpi4              # RAUC config for RPi4
├── system.conf.rpi5              # RAUC config for RPi5
├── bootloader-custom-backend.sh  # Custom backend for RPi bootloader
├── post-install.sh               # Post-install handler
├── setup-rauc.sh                 # Device setup script
├── create-bundle.sh              # Bundle creation script
├── install-bundle.sh             # Bundle installation script (NEW)
├── post-reboot-verify.sh         # Post-reboot verification (NEW)
├── ca.cert.pem                   # CA certificate
├── dev-cert.pem                  # Dev signing certificate
├── dev-key.pem                   # Dev signing key
└── README.md                     # Documentation
```

**RAUC vs Current OTA:**

| Feature | Current OTA (Phase C) | RAUC (Phase D) |
|---------|----------------------|----------------|
| Scope | Application binaries | Full rootfs |
| Package Size | 1-5 MB | 1-5 GB |
| Downtime | ~6 seconds | Reboot required (~30s) |
| Rollback | Version directory | A/B partition (automatic) |
| Signing | SHA256 hash | X.509 certificate |
| Use case | Frequent app updates | Major releases |

#### 11.3.1 RAUC Bundle Structure

The RAUC bundle (`.raucb`) is a signed package containing:

```
update-rpi5-20260304.raucb
├── manifest.raucm          # Metadata (version, compatibility, checksums)
│   ├── [update]
│   │   ├── compatible=seame-team6-rpi5
│   │   ├── version=20260304
│   │   └── description=SEA:ME Team 6 OTA Update
│   └── [image.rootfs]
│       ├── filename=rootfs.img
│       ├── sha256sum=abc123...
│       └── size=2147483648
├── rootfs.img              # Full filesystem image (ext4/squashfs)
└── hook.sh                 # Pre/post install scripts (optional)
```

#### 11.3.2 RAUC Installation Flow

```
┌─────────────────────────────────────────────────────────────────┐
│                    RAUC Update Flow                             │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  1. Receive bundle (.raucb)                                     │
│     └── Via GitHub Release, SCP, or download                    │
│                                                                 │
│  2. Verify bundle                                               │
│     └── rauc info /tmp/update.raucb                             │
│     └── Checks signature + hash                                 │
│                                                                 │
│  3. Install to inactive slot                                    │
│     └── rauc install /tmp/update.raucb                          │
│     └── Writes to rootfs-B (p3) while system runs on A          │
│                                                                 │
│  4. Reboot                                                      │
│     └── System boots from rootfs-B                              │
│                                                                 │
│  5. Verify & mark good                                          │
│     └── /opt/ota/rauc/post-reboot-verify.sh                     │
│     └── rauc status mark-good                                   │
│                                                                 │
│  6. Rollback (automatic if boot fails 3x)                       │
│     └── RAUC automatically returns to rootfs-A                  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

**Installation Commands:**

```bash
# 1. Copy bundle to device
scp update-rpi5.raucb root@10.21.220.191:/tmp/

# 2. Verify bundle
ssh root@10.21.220.191 "rauc info /tmp/update-rpi5.raucb"

# 3. Install
ssh root@10.21.220.191 "rauc install /tmp/update-rpi5.raucb"

# 4. Reboot
ssh root@10.21.220.191 "reboot"

# 5. After reboot - verify and confirm
ssh root@10.21.220.191 "rauc status && rauc status mark-good"
```

#### 11.3.3 Persistent Data Strategy

Since RAUC replaces the entire rootfs, custom configurations must be stored on the persistent `data` partition:

**Recommended `/data/` Structure:**
```
/data/                          # Partition p4 (persistent)
├── config/                     # Configs that survive updates
│   ├── etc/
│   │   ├── kuksa/
│   │   │   └── vss_custom.json
│   │   └── network/
│   │       └── interfaces
│   └── opt/
│       └── ota/
│           └── settings.conf
├── backup/                     # Automatic backups
│   ├── pre-update/
│   └── rollback/
├── logs/                       # Persistent logs
│   └── ota.log
├── certs/                      # Certificates
│   ├── ca.cert.pem
│   └── device.cert.pem
└── state/                      # Application state
    └── last-known-good.json
```

**Config Persistence Methods:**

| Method | Description | Use Case |
|--------|-------------|----------|
| **Symlinks** | `/etc/kuksa → /data/config/etc/kuksa` | Simple, transparent |
| **RAUC Post-Install Hook** | Copy from `/data/` after install | Flexible, scriptable |
| **Bind Mounts** | Mount `/data/config/etc/kuksa` on `/etc/kuksa` | No file duplication |

### 11.4 Phase E: RAUC Delta Updates (Future)

Delta updates send only the **differences** between versions instead of the full rootfs:

| Update Type | Size | Download Time (4G) | Use Case |
|-------------|------|-------------------|----------|
| Full rootfs | 2 GB | 15-30 min | Major releases |
| Delta | 50-200 MB | 1-3 min | Minor updates |
| tar.gz (apps) | 1-5 MB | 5 sec | Hotfixes |

**Delta Tools:**
- **casync** - Content-addressable sync (recommended)
- **desync** - Alternative to casync
- **bsdiff** - Classic binary diff

**Future Implementation:**
- [ ] casync chunk store on CDN/S3
- [ ] Delta bundle creation in CI/CD
- [ ] Local chunk cache on devices

### 11.5 FOTA for STM32 📋 Planned

- [ ] Bootloader with A/B slots
- [ ] UDS over CAN protocol
- [ ] AGL as FOTA gateway
- [ ] Secure boot verification

---

## 12. Troubleshooting

### 12.1 Common Issues

**Issue: Hash mismatch**
```bash
# Symptom
sha256sum: WARNING: 1 computed checksum did NOT match

# Solution
# Re-download the file or check network issues
rm /opt/ota/downloads/update.tar.gz
# Retry download
```

**Issue: Service fails to start**
```bash
# Check service status
systemctl status hello-ota

# Check journal
journalctl -u hello-ota -n 50

# Manual rollback
cp /opt/ota/backup/* /usr/bin/
systemctl restart hello-ota
```

**Issue: Download fails**
```bash
# Check network
ping github.com

# Check DNS
nslookup github.com

# Test curl
curl -I https://github.com
```

### 12.2 Log Locations

| Log | Path |
|-----|------|
| OTA operations | `/opt/ota/logs/ota.log` |
| Hello OTA service | `/var/log/hello-ota.log` |
| systemd journal | `journalctl -u hello-ota` |

---

## 13. References

### 13.1 Internal Documentation

- [TSF Implementation Guide](../TSF/tsf_implementation/TSF_docs/README.md)
- [KUKSA Integration](../../src/kuksa/README.md)

### 13.2 External Resources

- [RAUC Documentation](https://rauc.readthedocs.io/)
- [SWUpdate](https://sbabic.github.io/swupdate/)
- [Uptane Framework](https://uptane.github.io/)
- [AGL Documentation](https://docs.automotivelinux.org/)

### 13.3 Standards

- ISO 14229 (UDS)
- ISO 26262 (Functional Safety)
- UNECE WP.29 (Cybersecurity Regulations)

---

## 14. Phase C Implementation (2026-02-10)

Phase C adds production-ready features:
- **Automatic OTA polling** with systemd timer
- **Atomic symlinks** for zero-downtime updates
- **Qt Cluster service** (Wayland-based dashboard)
- **Version history** with improved rollback

### 14.1 New File Structure

```
src/ota/
├── install.sh              # Installation script for AGL
├── README.md               # OTA documentation
├── scripts/
│   ├── ota-check.sh        # GitHub API polling script
│   └── ota-update.sh       # v2 with atomic symlinks
└── systemd/
    ├── cluster.service     # Qt Cluster Dashboard (Wayland)
    ├── ota-check.service   # OTA check oneshot
    └── ota-check.timer     # 15-minute polling timer
```

### 14.2 Qt Cluster Service

The `cluster.service` runs the Qt6 Dashboard on Wayland:

```ini
[Unit]
Description=Qt Cluster Dashboard
After=weston.service graphical.target
Wants=weston.service

[Service]
Type=simple
User=root
Environment=XDG_RUNTIME_DIR=/run/user/0
Environment=WAYLAND_DISPLAY=wayland-0
Environment=QT_QPA_PLATFORM=wayland
ExecStart=/opt/cluster/current/HelloQt6Qml
Restart=on-failure
RestartSec=5

[Install]
WantedBy=graphical.target
```

### 14.3 Automatic OTA Polling

The `ota-check.timer` polls GitHub every 15 minutes for new releases:

```ini
[Unit]
Description=OTA Update Check Timer

[Timer]
OnBootSec=2min
OnUnitActiveSec=15min
RandomizedDelaySec=30
Persistent=true

[Install]
WantedBy=timers.target
```

The `ota-check.sh` script:
1. Reads current version from `/etc/ota-version`
2. Queries GitHub API for latest release
3. Compares versions
4. If newer, downloads and runs `ota-update.sh`

### 14.4 Atomic Symlinks

Phase C introduces atomic symlink switching for zero-downtime updates:

```bash
# Directory structure with versioned releases
/opt/ota/releases/v1.5.0/
/opt/ota/releases/v1.6.0/
/opt/ota/current -> releases/v1.6.0  # Atomic symlink

/opt/cluster/releases/v1.5.0/HelloQt6Qml
/opt/cluster/releases/v1.6.0/HelloQt6Qml
/opt/cluster/current -> releases/v1.6.0  # Atomic symlink
```

**Update Flow:**
1. Download new version to `/opt/ota/releases/v1.6.0/`
2. Verify integrity with SHA-256 hash
3. Atomic symlink switch: `ln -sfn releases/v1.6.0 current`
4. Reload affected services

### 14.5 Version History

Phase C maintains a version history file:

```bash
$ cat /opt/ota/version-history.log
2026-02-10T14:30:00Z v1.5.0 installed
2026-02-10T16:45:00Z v1.6.0 installed
```

### 14.6 Improved Rollback

If update fails, rollback uses atomic symlink:

```bash
# Get previous version
PREV=$(sed -n '2p' /opt/ota/version-history.log | awk '{print $2}')

# Atomic rollback
ln -sfn "releases/$PREV" current

# Restart services
systemctl restart cluster can-to-kuksa
```

### 14.7 Installation on AGL

```bash
# On development machine
scp -r src/ota/* root@10.21.220.191:/tmp/ota-install/

# On AGL (Raspberry Pi 5)
cd /tmp/ota-install
chmod +x install.sh
./install.sh

# Enable services
systemctl enable --now cluster.service
systemctl enable --now ota-check.timer
```

### 14.8 Phase C Status

| Component | File | Status |
|-----------|------|--------|
| Qt Cluster Service | `systemd/cluster.service` | ✅ Created |
| OTA Check Timer | `systemd/ota-check.timer` | ✅ Active |
| OTA Check Service | `systemd/ota-check.service` | ✅ Working |
| Polling Script | `scripts/ota-check.sh` | ✅ Tested |
| Atomic Symlinks | `scripts/ota-update.sh` | ✅ Tested |
| Install Script | `install.sh` | ✅ Created |
| Documentation | `README.md` | ✅ Created |
| Deploy to AGL | - | ✅ **Complete** |

### 14.9 Phase C Validation (2026-02-10 18:13 UTC)

**Automatic OTA Update v1.5.0 → v1.6.0:**

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

**Phase C Features Validated:**

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

**Verification:**
```bash
$ cat /etc/ota-version
v1.6.0

$ ls -la /opt/ota/current
lrwxrwxrwx 1 root root 26 Feb 10 18:13 /opt/ota/current -> /opt/ota/releases/v1.6.0

$ ls /opt/ota/releases/
v1.0.1  v1.5.0  v1.6.0
```

---

## Appendix A: OTA Manual vs RAUC Comparison

| Criteria | OTA Manual | RAUC |
|----------|------------|------|
| Atomicity | ❌ Limited | ✅ Full |
| Auto rollback | ⚠️ Service-level | ✅ Boot-level |
| Full system update | ❌ No | ✅ Yes |
| Complexity | ⭐ Low | ⭐⭐⭐⭐ High |
| Brick risk | Low (user-space) | Very low |
| Implementation time | Very low | High |
| Academic suitability | ✅ Excellent | ✅ Good |

---

## Appendix B: Academic Justification

> "Although automotive-grade OTA frameworks such as RAUC or Uptane exist, for this project we adopted a service-level OTA strategy. This approach avoids full system reflashes, minimizes risks during development, and aligns with the modular update philosophy promoted by modern automotive architectures."

This implementation demonstrates:
- ✅ Understanding of OTA principles
- ✅ CI/CD integration
- ✅ Security awareness (hash verification)
- ✅ Robustness (rollback capability)
- ✅ Scalability path (RAUC-ready architecture)

---

## Appendix C: Quick Reference

### Essential Commands

```bash
# Check current version
cat /etc/ota-version

# View OTA logs
cat /opt/ota/logs/ota.log

# Manual update
/opt/ota/ota-update.sh <VERSION> <URL> <HASH>

# Service status
systemctl status hello-ota

# Create release tag
git tag vX.Y.Z && git push origin vX.Y.Z
```

### Key Paths

| What | Where |
|------|-------|
| OTA script | `/opt/ota/ota-update.sh` |
| Version file | `/etc/ota-version` |
| Logs | `/opt/ota/logs/ota.log` |
| Backup | `/opt/ota/backup/` |
| Current | `/opt/ota/current/` |
| GitHub workflow | `.github/workflows/ota.yml` |

---

## 13. Testing Strategy

### 13.1 Current Tests Implemented

| Test Type | What it tests | Status | Implementation |
|-----------|---------------|--------|----------------|
| **Hash verification** | Package integrity | ✅ Implemented | SHA256 check before install |
| **Architecture check** | Binary matches CPU | ✅ Implemented | `file` command verification |
| **Health check** | Service starts without crash | ✅ Implemented | Restart loop detection (3+ in 10s = failure) |
| **Rollback test** | Reversion works | ✅ Tested | Tested v1.9.0→v1.8.0 |
| **Integration tests** | App works after update | ❌ Manual | Not automated yet |
| **Signature verification** | Bundle not tampered | ⚠️ Partial | RAUC has X.509, scripts use SHA256 only |

### 13.2 Smoke Test (Post-Update Verification)

A **smoke test** is a quick, basic check that verifies the application "breathes" after an update.

```
┌─────────────────────────────────────────────────────────────────┐
│                      SMOKE TEST                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  After update v1.9.0 → v1.10.0:                                 │
│                                                                 │
│  1. Service running?                                            │
│     $ systemctl is-active can-to-kuksa.service                  │
│     → "active" ✅  or  "failed" ❌                               │
│                                                                 │
│  2. Binary responds?                                            │
│     $ /opt/cluster/HelloQt6Qml --version                        │
│     → "v1.10.0" ✅  or  timeout ❌                               │
│                                                                 │
│  3. Port listening? (for network services)                      │
│     $ curl -s http://localhost:8080/health                      │
│     → "OK" ✅  or  connection refused ❌                         │
│                                                                 │
│  If any test fails → AUTOMATIC ROLLBACK!                        │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

**Example implementation for KUKSA publisher:**
```bash
smoke_test() {
    # 1. Service active?
    systemctl is-active can-to-kuksa.service || return 1
    
    # 2. Process exists?
    pgrep -f can_to_kuksa_publisher || return 1
    
    # 3. Not in restart loop?
    restarts=$(systemctl show can-to-kuksa.service -p NRestarts --value)
    [ "$restarts" -lt 3 ] || return 1
    
    return 0  # Passed!
}
```

### 13.3 Canary Deployment

**Canary deployment** means updating **only 1 device first**, verifying it works, and only then updating the rest.

```
┌─────────────────────────────────────────────────────────────────┐
│                    CANARY DEPLOYMENT                            │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  Imagine 100 cars in a fleet:                                   │
│                                                                 │
│  PHASE 1 (Canary):                                              │
│  ┌─────┐                                                        │
│  │ 🐤  │ ← Car #1 receives v1.10.0                              │
│  └─────┘                                                        │
│  ┌─────┐ ┌─────┐ ┌─────┐ ... ┌─────┐                            │
│  │v1.9 │ │v1.9 │ │v1.9 │     │v1.9 │ ← 99 cars stay on v1.9.0   │
│  └─────┘ └─────┘ └─────┘     └─────┘                            │
│                                                                 │
│  Wait 24h... Car #1 is OK? ✅                                   │
│                                                                 │
│  PHASE 2 (10%):                                                 │
│  Update 10 cars → wait 24h → all OK? ✅                         │
│                                                                 │
│  PHASE 3 (100%):                                                │
│  Update entire fleet!                                           │
│                                                                 │
│  IF problem on Canary → STOP and investigate!                   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

**In our case (2 devices):**
- Canary: RPi5 receives update first
- If OK → Update RPi4
- If NOT OK → Investigate, don't update RPi4

### 13.4 A/B Comparison Test (RAUC vs tar.gz)

Execute **both update methods** and compare metrics:

```
┌─────────────────────────────────────────────────────────────────┐
│                    A/B COMPARISON TEST                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  TEST: Update v1.9.0 → v1.10.0                                  │
│                                                                 │
│  ┌──────────────────────┐    ┌──────────────────────┐           │
│  │    OTA Scripts       │    │       RAUC           │           │
│  │    (tar.gz)          │    │      (.raucb)        │           │
│  ├──────────────────────┤    ├──────────────────────┤           │
│  │ Size: 4.6 MB         │    │ Size: 1.2 GB         │           │
│  │ Download: 1.2s       │    │ Download: 45s        │           │
│  │ Install: 3.1s        │    │ Install: 120s        │           │
│  │ Downtime: 0s         │    │ Downtime: 35s        │           │
│  │ Total: 4.3s          │    │ Total: 200s          │           │
│  │ Rollback: Manual     │    │ Rollback: Auto boot  │           │
│  │ Signature: SHA256    │    │ Signature: X.509     │           │
│  └──────────────────────┘    └──────────────────────┘           │
│                                                                 │
│  CONCLUSION:                                                    │
│  - OTA Scripts: Better for frequent app updates                 │
│  - RAUC: Better for full system updates                         │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 13.5 Testing Scripts (Created and Ready to Use)

| Script | Location | Purpose | Status |
|--------|----------|---------|--------|
| `smoke-test.sh` | `src/ota/scripts/` | 7+ automated post-update tests | ✅ Created |
| `canary-check.sh` | `src/ota/scripts/` | Canary deployment support | ✅ Created |
| `benchmark-ota.sh` | `src/ota/scripts/` | Performance comparison tar.gz vs RAUC | ✅ Created |

#### How to Deploy and Use the Testing Scripts

**Step 1: Copy scripts to devices**
```bash
# Copy all test scripts to RPi5
scp src/ota/scripts/smoke-test.sh src/ota/scripts/canary-check.sh \
    root@10.21.220.191:/opt/ota/

# Copy all test scripts to RPi4
scp src/ota/scripts/smoke-test.sh src/ota/scripts/canary-check.sh \
    root@10.21.220.192:/opt/ota/

# Make them executable
ssh root@10.21.220.191 "chmod +x /opt/ota/*.sh"
ssh root@10.21.220.192 "chmod +x /opt/ota/*.sh"
```

**Step 2: Run smoke test after an update**
```bash
# On RPi5
ssh root@10.21.220.191 "/opt/ota/smoke-test.sh"

# On RPi4
ssh root@10.21.220.192 "/opt/ota/smoke-test.sh"
```

**Step 3: Configure canary deployment**
```bash
# Set RPi5 as canary (receives updates first)
ssh root@10.21.220.191 "/opt/ota/canary-check.sh set-role canary"

# Set RPi4 as production (waits 24h after canary)
ssh root@10.21.220.192 "/opt/ota/canary-check.sh set-role production"

# Check status
ssh root@10.21.220.191 "/opt/ota/canary-check.sh status"
```

**Step 4: Run benchmark comparison**
```bash
# Run from development machine (requires SSH access)
./src/ota/scripts/benchmark-ota.sh v1.10.0 rpi5
```

---

## 14. FAQ - Frequently Asked Questions

### Q1: Can I use only RAUC instead of OTA scripts?

**Yes!** Technically you can use RAUC for everything, but there are trade-offs:

| Aspect | OTA Scripts | RAUC |
|--------|-------------|------|
| **Update time** | ~6 seconds | ~30-60s (reboot required) |
| **Package size** | 260KB-4.6MB (binary only) | 1-5GB (entire rootfs) |
| **Downtime** | Zero (hot swap) | 30s+ (reboot) |
| **Practical frequency** | Several times/day | 1x per week/month |
| **Bandwidth** | Low | High |

**Recommendation:**
- **Production (limited 4G):** OTA scripts for patches, RAUC for major releases
- **Development:** Can use only RAUC if you prefer simplicity

### Q2: What's currently implemented for SOTA/COTA?

| Component | Type | Status | Details |
|-----------|------|--------|---------|
| `can_to_kuksa_publisher` | SOTA | ✅ Working | Binary at `/home/kuksa_RPi5/bin/` |
| `can-to-kuksa.service` | SOTA | ✅ Working | systemd service, updated via OTA |
| `HelloQt6Qml` | SOTA | ✅ Working | Qt6 Cluster UI on RPi4 |
| `vss_min.json` | COTA | ✅ Working | Included in `update-rpi5.tar.gz` |

### Q3: What about FOTA (STM32)?

**Not yet implemented!** Marked as "📋 Planned" in the roadmap.

Next steps would be:
- Send firmware `.bin` via CAN or UART to STM32
- STM32 bootloader receives and flashes
- More complex because it requires custom bootloader on STM32

### Q4: Can updates be done while the car is moving?

**Depends on the update type:**

| Type | Car Stopped? | Reason |
|------|--------------|--------|
| **COTA** (configs) | ❌ Can be moving | Only config files, no restart |
| **SOTA** (apps) | ⚠️ Recommended stopped | Service restart may affect UI |
| **RAUC** (rootfs) | ✅ **Must be stopped** | Requires full reboot |
| **FOTA** (STM32) | ✅ **Must be stopped** | Microcontroller controls CAN! |

**In the automotive industry:**
- Updates are scheduled when the car is parked
- Usually at night, connected to charger (EVs)
- Some critical updates require driver consent

### Q5: Does OTA testing make sense?

**Absolutely yes!** OTA tests are critical:

| Test Type | What it tests | Status |
|-----------|---------------|--------|
| Hash verification | Package integrity | ✅ Implemented |
| Architecture check | Correct binary for CPU | ✅ Implemented |
| Health check | Service starts without crash | ✅ Implemented |
| Rollback test | Reversion works | ✅ Tested |
| Integration tests | App works after update | ❌ Manual |
| Signature verification | Bundle not tampered | ⚠️ Partial |

### Q6: What's the difference between Smoke Test, Canary, and A/B Comparison?

```
┌─────────────────────────────────────────────────────────────────┐
│                    3 QUALITY CONCEPTS                           │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  SMOKE TEST       = "Does the app still work?" (post-update)    │
│  ───────────                                                    │
│  • Quick verification (seconds)                                 │
│  • Automatic after each update                                  │
│  • Failure → Immediate rollback                                 │
│                                                                 │
│  CANARY           = "Test on one before updating all"           │
│  ──────                                                         │
│  • 1 device first                                               │
│  • Wait X hours                                                 │
│  • OK → Update the rest                                         │
│                                                                 │
│  A/B COMPARISON   = "Which method is better?"                   │
│  ──────────────                                                 │
│  • Test both methods                                            │
│  • Measure metrics (time, size, etc.)                           │
│  • Document results                                             │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 15. References

- [RAUC Documentation](https://rauc.readthedocs.io/)
- [AGL OTA Best Practices](https://docs.automotivelinux.org/)
- [Uptane Security Framework](https://uptane.github.io/)
- [systemd Service Management](https://www.freedesktop.org/software/systemd/man/)

---

**Document Version:** 2.0  
**Author:** SEA:ME Team 6  
**Last Updated:** 24 February 2026
