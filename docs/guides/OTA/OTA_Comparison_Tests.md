# OTA Comparison Tests: tar.gz Scripts vs RAUC

**Last Updated:** 4 March 2026  
**Author:** SEA:ME Team 6  
**Status:** Template Ready for Testing

---

## 1. Overview

This document provides a framework for comparing the two OTA update methods implemented in this project:

| Method | Package Format | Scope | Use Case |
|--------|----------------|-------|----------|
| **OTA Scripts** | `.tar.gz` | Application binaries | Frequent, small updates |
| **RAUC** | `.raucb` | Full rootfs image | Major system releases |

---

## 2. Test Scenarios

### 2.1 Test Environment

| Device | IP | Architecture | Platform |
|--------|-----|--------------|----------|
| **RPi5** | 10.21.220.191 | aarch64 | KUKSA Publisher |
| **RPi4** | 10.21.220.192 | armv7l | Qt6 Cluster |

### 2.2 Test Versions

| From Version | To Version | Test Date | Tester |
|--------------|------------|-----------|--------|
| v1.9.0 | v1.10.0 | TBD | |
| v1.10.0 | v1.9.0 | TBD | (Rollback test) |

---

## 3. Metrics to Measure

### 3.1 Performance Metrics

| Metric | OTA Scripts | RAUC | Winner |
|--------|-------------|------|--------|
| **Package Size** | ___ MB | ___ GB | |
| **Download Time** | ___ s | ___ s | |
| **Installation Time** | ___ s | ___ s | |
| **Service Downtime** | ___ s | ___ s | |
| **Total Update Time** | ___ s | ___ s | |
| **Reboot Required** | No | Yes | |

### 3.2 Reliability Metrics

| Metric | OTA Scripts | RAUC | Winner |
|--------|-------------|------|--------|
| **Rollback Mechanism** | Symlink switch | A/B partition | |
| **Automatic Rollback** | Service-level | Boot-level | |
| **Brick Risk** | Low | Very Low | |
| **Atomic Update** | Partial | Full | |

### 3.3 Security Metrics

| Metric | OTA Scripts | RAUC | Winner |
|--------|-------------|------|--------|
| **Hash Verification** | SHA256 | SHA256 | Tie |
| **Signature** | None | X.509 | RAUC |
| **Bundle Encryption** | No | Optional | RAUC |

---

## 4. Test Procedures

### 4.1 OTA Scripts Test (tar.gz)

```bash
#!/bin/bash
# test-ota-scripts.sh - Benchmark OTA Scripts update

VERSION="${1:-v1.10.0}"
DEVICE="${2:-rpi5}"
IP="10.21.220.191"
[ "$DEVICE" = "rpi4" ] && IP="10.21.220.192"

echo "=== OTA Scripts Benchmark ==="
echo "Device: $DEVICE ($IP)"
echo "Version: $VERSION"
echo ""

# Get package size
echo "1. Package Size:"
PACKAGE_SIZE=$(ssh root@$IP "curl -sI https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/releases/download/$VERSION/update-$DEVICE.tar.gz | grep -i content-length | awk '{print \$2}' | tr -d '\r'")
echo "   $((PACKAGE_SIZE / 1024)) KB"

# Measure download time
echo ""
echo "2. Download Time:"
DOWNLOAD_START=$(date +%s.%N)
ssh root@$IP "curl -sLO https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/releases/download/$VERSION/update-$DEVICE.tar.gz -o /tmp/test-package.tar.gz"
DOWNLOAD_END=$(date +%s.%N)
DOWNLOAD_TIME=$(echo "$DOWNLOAD_END - $DOWNLOAD_START" | bc)
echo "   ${DOWNLOAD_TIME}s"

# Measure full update time
echo ""
echo "3. Full Update Time:"
UPDATE_START=$(date +%s.%N)
ssh root@$IP "/opt/ota/ota-update.sh $VERSION"
UPDATE_END=$(date +%s.%N)
UPDATE_TIME=$(echo "$UPDATE_END - $UPDATE_START" | bc)
echo "   ${UPDATE_TIME}s"

# Verify service status
echo ""
echo "4. Service Status:"
if [ "$DEVICE" = "rpi5" ]; then
    ssh root@$IP "systemctl is-active can-to-kuksa.service"
else
    ssh root@$IP "systemctl is-active helloqt-app.service"
fi

echo ""
echo "=== OTA Scripts Results ==="
echo "Package Size: $((PACKAGE_SIZE / 1024)) KB"
echo "Download Time: ${DOWNLOAD_TIME}s"
echo "Total Update Time: ${UPDATE_TIME}s"
echo "Reboot Required: No"
```

### 4.2 RAUC Test (.raucb)

```bash
#!/bin/bash
# test-rauc.sh - Benchmark RAUC update

VERSION="${1:-v1.10.0}"
DEVICE="${2:-rpi5}"
IP="10.21.220.191"
[ "$DEVICE" = "rpi4" ] && IP="10.21.220.192"

echo "=== RAUC Benchmark ==="
echo "Device: $DEVICE ($IP)"
echo "Version: $VERSION"
echo ""

# Note: RAUC bundles need to be created separately
# This assumes the bundle exists at /tmp/update.raucb on the device

echo "1. Check current RAUC status:"
ssh root@$IP "rauc status"

echo ""
echo "2. Bundle Size:"
BUNDLE_SIZE=$(ssh root@$IP "ls -l /tmp/update.raucb 2>/dev/null | awk '{print \$5}'" || echo "N/A")
echo "   $BUNDLE_SIZE bytes"

echo ""
echo "3. Installation Time (excluding reboot):"
INSTALL_START=$(date +%s.%N)
ssh root@$IP "rauc install /tmp/update.raucb" || echo "Bundle not found - skip"
INSTALL_END=$(date +%s.%N)
INSTALL_TIME=$(echo "$INSTALL_END - $INSTALL_START" | bc)
echo "   ${INSTALL_TIME}s"

echo ""
echo "4. Reboot and measure boot time:"
echo "   (Manual measurement required - time from reboot command to SSH available)"

echo ""
echo "=== RAUC Results ==="
echo "Bundle Size: $BUNDLE_SIZE bytes"
echo "Install Time: ${INSTALL_TIME}s"
echo "Reboot Required: Yes"
```

### 4.3 Comparison Script

```bash
#!/bin/bash
# benchmark-comparison.sh - Compare both methods side by side

VERSION="${1:-v1.10.0}"

echo "=============================================="
echo "   OTA COMPARISON: tar.gz vs RAUC"
echo "   Version: $VERSION"
echo "=============================================="
echo ""

# Run OTA Scripts test
echo ">>> Testing OTA Scripts (tar.gz)..."
./test-ota-scripts.sh $VERSION rpi5 > /tmp/ota-scripts-results.txt 2>&1
OTA_SCRIPTS_TIME=$(grep "Total Update Time" /tmp/ota-scripts-results.txt | awk '{print $4}')
OTA_SCRIPTS_SIZE=$(grep "Package Size" /tmp/ota-scripts-results.txt | awk '{print $3}')

echo ""
echo ">>> Testing RAUC (.raucb)..."
./test-rauc.sh $VERSION rpi5 > /tmp/rauc-results.txt 2>&1
RAUC_TIME=$(grep "Install Time" /tmp/rauc-results.txt | awk '{print $3}')
RAUC_SIZE=$(grep "Bundle Size" /tmp/rauc-results.txt | awk '{print $3}')

echo ""
echo "=============================================="
echo "   RESULTS COMPARISON"
echo "=============================================="
echo ""
printf "| %-20s | %-15s | %-15s |\n" "Metric" "OTA Scripts" "RAUC"
printf "| %-20s | %-15s | %-15s |\n" "--------------------" "---------------" "---------------"
printf "| %-20s | %-15s | %-15s |\n" "Package Size" "${OTA_SCRIPTS_SIZE}KB" "${RAUC_SIZE}B"
printf "| %-20s | %-15s | %-15s |\n" "Update Time" "${OTA_SCRIPTS_TIME}" "${RAUC_TIME}"
printf "| %-20s | %-15s | %-15s |\n" "Reboot Required" "No" "Yes"
printf "| %-20s | %-15s | %-15s |\n" "Rollback" "Manual" "Auto (boot)"
echo ""
```

---

## 5. Test Results Template

### 5.1 Test Session: [DATE]

**Tester:** [NAME]  
**Devices:** RPi5 (10.21.220.191), RPi4 (10.21.220.192)  
**Update Version:** v___ → v___

#### RPi5 Results (KUKSA Publisher)

| Metric | OTA Scripts | RAUC | Notes |
|--------|-------------|------|-------|
| Package Size | ___ KB | ___ GB | |
| Download Time | ___ s | ___ s | |
| Install Time | ___ s | ___ s | |
| Service Downtime | ___ s | ___ s | |
| Total Time | ___ s | ___ s | |
| Rollback Test | ✅/❌ | ✅/❌ | |

#### RPi4 Results (Qt Cluster)

| Metric | OTA Scripts | RAUC | Notes |
|--------|-------------|------|-------|
| Package Size | ___ MB | ___ GB | |
| Download Time | ___ s | ___ s | |
| Install Time | ___ s | ___ s | |
| Service Downtime | ___ s | ___ s | |
| Total Time | ___ s | ___ s | |
| Rollback Test | ✅/❌ | ✅/❌ | |

#### Observations

```
[Notes about the test session - any issues, unexpected behavior, etc.]
```

---

## 6. Rollback Test Procedure

### 6.1 OTA Scripts Rollback

```bash
# Current version
ssh root@$IP "cat /etc/ota-version"

# Rollback to previous version
ssh root@$IP "/opt/ota/ota-update.sh v1.9.0"

# Verify rollback
ssh root@$IP "cat /etc/ota-version"
ssh root@$IP "systemctl is-active can-to-kuksa.service"
```

### 6.2 RAUC Rollback

```bash
# Check current slot
ssh root@$IP "rauc status | grep 'Booted from'"

# RAUC automatic rollback happens on boot failure
# To manually switch slots:
ssh root@$IP "rauc status mark-active other"
ssh root@$IP "reboot"

# Verify after reboot
ssh root@$IP "rauc status | grep 'Booted from'"
```

---

## 7. Conclusions Template

### 7.1 Summary

| Criteria | Recommended Method | Reason |
|----------|-------------------|--------|
| Frequent app updates | OTA Scripts | Faster, smaller packages |
| Major system updates | RAUC | A/B safety, atomic |
| Limited bandwidth | OTA Scripts | Much smaller packages |
| Mission-critical | RAUC | Automatic boot rollback |

### 7.2 Recommendations

Based on our test results:

1. **For development/testing:** Use OTA Scripts for fast iteration
2. **For production releases:** Use RAUC for critical updates
3. **Hybrid approach:** OTA Scripts for patches, RAUC for major versions

---

## 8. References

- [RAUC Documentation](https://rauc.readthedocs.io/)
- [OTA Implementation Guide](./OTA_Implementation_Guide.md)
- [Multi-Version Architecture](./OTA_multiversion-arch-sprint8.md)

---

## 9. RAUC Bundle Details

### 9.1 What is a RAUC Bundle?

A **RAUC bundle** (`.raucb`) is a **signed package** containing:

```
update-rpi5-20260304.raucb
├── manifest.raucm          # Metadata (version, compatibility, checksums)
├── rootfs.img              # Full filesystem image (ext4/squashfs)
└── hook.sh                 # Pre/post install scripts (optional)
```

**Key characteristics:**
- **Cryptographically signed** (X.509 certificate)
- **Verified** before installation (hash + signature)
- **Atomic** - installs to inactive partition, switches on reboot

### 9.2 Bundle vs tar.gz Content Comparison

| Aspect | tar.gz (Phase C) | RAUC Bundle (Phase D) |
|--------|------------------|----------------------|
| **Contains** | App binaries only | Full rootfs image |
| **Size** | 1-5 MB | 1-5 GB |
| **AGL System** | ❌ Not included | ✅ Included |
| **Kernel** | ❌ Not included | ✅ Included |
| **Apps (KUKSA, Cluster)** | ✅ Included | ✅ Included |
| **Configs (VSS tree)** | ✅ Included | ✅ Included |
| **Signing** | SHA256 hash | X.509 certificate |

### 9.3 When to Use Each

| Scenario | Recommended | Reason |
|----------|-------------|--------|
| Update KUKSA binary | **tar.gz** | Fast, no reboot |
| Update VSS tree config | **tar.gz** | Config only |
| Update Qt Cluster UI | **tar.gz** | Fast, no reboot |
| New AGL version (minor) | **RAUC Delta** | Smaller download |
| New AGL version (major) | **RAUC Full** | Many changes |
| Kernel/driver update | **RAUC Full** | Requires reboot |
| Emergency hotfix | **tar.gz** | Fastest option |
| Security patch (system) | **RAUC** | Full system |

### 9.4 RAUC Directories on AGL Device

RAUC creates several directories on the system:

| Directory | Purpose | Contents |
|-----------|---------|----------|
| `/run/rauc/` | Runtime data | Temporary files, created by RAUC daemon |
| `/etc/rauc/` | Configuration | `system.conf`, keyring certificates |
| `/usr/lib/rauc/` | Libraries | RAUC binaries and libraries |
| `/opt/ota/rauc/` | **Team scripts** | Custom scripts for bundle management |

**Team Scripts Directory (`/opt/ota/rauc/`):**
```
/opt/ota/rauc/
├── install-bundle.sh         # Installs .raucb bundles with validation
└── post-reboot-verify.sh     # Verifies system health after RAUC update
```

**Full OTA Directory Structure:**
```
/opt/ota/
├── ota-update.sh              # Main OTA script (Phase C - tar.gz)
├── ota-check.sh               # GitHub API polling for updates
├── rauc/                      # RAUC-specific scripts (Phase D)
│   ├── install-bundle.sh      # Validates and installs .raucb bundles
│   └── post-reboot-verify.sh  # Health checks + marks slot as good
├── backup/                    # Version backups for rollback
├── current/                   # Current version symlinks
├── downloads/                 # Downloaded update packages
├── logs/                      # OTA operation logs
└── releases/                  # Installed release versions
```

---

## 10. Persistent Data Strategy

### 10.1 The Problem

RAUC replaces the **entire rootfs**, including `/etc/` and `/opt/`. Custom configurations made after deployment are **lost**:

```
Timeline:
─────────────────────────────────────────────────────────────
1. Deploy v1.0 (rootfs-A)
   /etc/network/config → IP: 10.21.220.191

2. Manual change on device:
   /etc/network/config → IP: 10.21.220.200 ✏️

3. RAUC update v1.1 (writes to rootfs-B)
   rootfs-B has: IP: 10.21.220.191 (from bundle!)

4. Reboot to rootfs-B
   IP is back to 10.21.220.191 😱 (custom change lost!)
─────────────────────────────────────────────────────────────
```

### 10.2 The Solution: Data Partition (p4)

The `data` partition is **not managed by RAUC** and survives updates:

```
┌─────────────────────────────────────────────────────────────────┐
│                    Recommended Architecture                     │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  rootfs (A or B)              data (p4) - PERSISTENT            │
│  ┌─────────────────┐          ┌─────────────────────────┐       │
│  │ /etc/default/   │ ───────► │ /data/config/           │       │
│  │ (defaults)      │ symlink  │  ├── network.conf       │       │
│  │                 │   or     │  ├── vss_custom.json    │       │
│  │ /opt/ota/       │  bind    │  ├── certificates/      │       │
│  │ (scripts)       │  mount   │  └── user-settings.json │       │
│  └─────────────────┘          └─────────────────────────┘       │
│                                                                 │
│  Updated by RAUC             Survives updates!                  │
└─────────────────────────────────────────────────────────────────┘
```

### 10.3 Recommended `/data/` Structure

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

---

## 11. RAUC Delta Updates (Future - Phase E)

### 11.1 What is Delta Update?

Instead of sending the **full rootfs** (1-5GB), send only the **differences**:

```
┌─────────────────────────────────────────────────────────────────┐
│                    Full Update (Normal)                         │
├─────────────────────────────────────────────────────────────────┤
│   Bundle v1.0 ──────────────────────────► Bundle v1.1           │
│   (2 GB)                                   (2 GB)               │
│   Download: 2 GB complete 😰                                    │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│                    Delta Update                                 │
├─────────────────────────────────────────────────────────────────┤
│   Bundle v1.0 ───[ DIFF ]───────────────► Bundle v1.1           │
│   (on device)      │                       (2 GB)               │
│                    ▼                                            │
│              Delta: 50-200 MB 🎉                                │
│              (only differences!)                                │
└─────────────────────────────────────────────────────────────────┘
```

### 11.2 Size Comparison

| Update Type | Size | Download Time (4G) | Data Usage |
|-------------|------|-------------------|------------|
| **Full rootfs** | 2 GB | 15-30 min | 2 GB |
| **Delta** | 50-200 MB | 1-3 min | 50-200 MB |
| **tar.gz (apps)** | 1-5 MB | ~5 sec | 1-5 MB |

### 11.3 Delta Tools

| Tool | Description | Compression |
|------|-------------|-------------|
| **casync** | Content-addressable sync (recommended) | Excellent |
| **desync** | Alternative to casync | Very good |
| **bsdiff** | Classic binary diff | Good |

**Future Implementation (Phase E):**
- [ ] casync chunk store on CDN/S3
- [ ] Delta bundle creation in CI/CD
- [ ] Local chunk cache on devices

---

**Document Version:** 1.1  
**Created:** 24 February 2026  
**Last Updated:** 4 March 2026
