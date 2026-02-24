#!/bin/bash
# benchmark-ota.sh - Compare OTA Scripts vs RAUC performance
#
# Usage: ./benchmark-ota.sh <version> [device]
# Example: ./benchmark-ota.sh v1.10.0 rpi5
#
# This script measures and compares:
# - Package/bundle size
# - Download time
# - Installation time
# - Total update time
# - Service restart time

set -e

VERSION="${1:-v1.10.0}"
DEVICE="${2:-rpi5}"
GITHUB_REPO="SEAME-pt/SEA-ME_Team6_2025-26"

# Device configuration
case "$DEVICE" in
    rpi5)
        IP="10.21.220.191"
        SERVICE="can-to-kuksa.service"
        PACKAGE="update-rpi5.tar.gz"
        ;;
    rpi4)
        IP="10.21.220.192"
        SERVICE="helloqt-app.service"
        PACKAGE="update-rpi4.tar.gz"
        ;;
    *)
        echo "Unknown device: $DEVICE"
        exit 1
        ;;
esac

RESULTS_FILE="benchmark-results-$(date +%Y%m%d-%H%M%S).md"

log() {
    echo "[$(date '+%H:%M:%S')] $1"
}

measure_time() {
    local start=$(date +%s.%N)
    eval "$1" >/dev/null 2>&1
    local end=$(date +%s.%N)
    echo "scale=2; $end - $start" | bc
}

# ==============================================================================
# Header
# ==============================================================================
cat > "$RESULTS_FILE" << EOF
# OTA Benchmark Results

**Date:** $(date '+%Y-%m-%d %H:%M:%S')  
**Version:** $VERSION  
**Device:** $DEVICE ($IP)  
**Service:** $SERVICE

---

EOF

echo "=============================================="
echo "   OTA BENCHMARK: tar.gz vs RAUC"
echo "   Version: $VERSION"
echo "   Device: $DEVICE ($IP)"
echo "=============================================="
echo ""

# ==============================================================================
# Test 1: OTA Scripts (tar.gz)
# ==============================================================================
log ">>> Testing OTA Scripts (tar.gz)..."

# Get current version for rollback later
CURRENT_VERSION=$(ssh root@$IP "cat /etc/ota-version 2>/dev/null || echo 'unknown'")
log "Current version: $CURRENT_VERSION"

# 1.1 Package Size
log "Measuring package size..."
PACKAGE_URL="https://github.com/$GITHUB_REPO/releases/download/$VERSION/$PACKAGE"
SCRIPTS_SIZE=$(curl -sI "$PACKAGE_URL" 2>/dev/null | grep -i content-length | awk '{print $2}' | tr -d '\r' || echo "0")
SCRIPTS_SIZE_KB=$((SCRIPTS_SIZE / 1024))
log "Package size: ${SCRIPTS_SIZE_KB} KB"

# 1.2 Download Time
log "Measuring download time..."
SCRIPTS_DOWNLOAD_START=$(date +%s.%N)
ssh root@$IP "curl -sLo /tmp/test-package.tar.gz '$PACKAGE_URL'" 2>/dev/null
SCRIPTS_DOWNLOAD_END=$(date +%s.%N)
SCRIPTS_DOWNLOAD_TIME=$(echo "scale=2; $SCRIPTS_DOWNLOAD_END - $SCRIPTS_DOWNLOAD_START" | bc)
log "Download time: ${SCRIPTS_DOWNLOAD_TIME}s"

# 1.3 Full Update Time
log "Measuring full update time..."
SCRIPTS_UPDATE_START=$(date +%s.%N)
ssh root@$IP "/opt/ota/ota-update.sh $VERSION" 2>/dev/null
SCRIPTS_UPDATE_END=$(date +%s.%N)
SCRIPTS_UPDATE_TIME=$(echo "scale=2; $SCRIPTS_UPDATE_END - $SCRIPTS_UPDATE_START" | bc)
log "Update time: ${SCRIPTS_UPDATE_TIME}s"

# 1.4 Service Status
SCRIPTS_SERVICE_STATUS=$(ssh root@$IP "systemctl is-active $SERVICE 2>/dev/null || echo 'failed'")
log "Service status: $SCRIPTS_SERVICE_STATUS"

# Rollback to test again
log "Rolling back to $CURRENT_VERSION for RAUC test..."
ssh root@$IP "/opt/ota/ota-update.sh $CURRENT_VERSION" 2>/dev/null || true

echo ""

# ==============================================================================
# Test 2: RAUC (.raucb)
# ==============================================================================
log ">>> Testing RAUC (.raucb)..."

# Check if RAUC bundle exists
RAUC_BUNDLE="/tmp/update-$DEVICE.raucb"
RAUC_AVAILABLE=$(ssh root@$IP "[ -f $RAUC_BUNDLE ] && echo 'yes' || echo 'no'")

if [ "$RAUC_AVAILABLE" = "yes" ]; then
    # 2.1 Bundle Size
    RAUC_SIZE=$(ssh root@$IP "ls -l $RAUC_BUNDLE | awk '{print \$5}'" 2>/dev/null || echo "0")
    RAUC_SIZE_MB=$((RAUC_SIZE / 1024 / 1024))
    log "Bundle size: ${RAUC_SIZE_MB} MB"
    
    # 2.2 Installation Time (without reboot)
    log "Measuring RAUC install time..."
    RAUC_INSTALL_START=$(date +%s.%N)
    ssh root@$IP "rauc install $RAUC_BUNDLE" 2>/dev/null || true
    RAUC_INSTALL_END=$(date +%s.%N)
    RAUC_INSTALL_TIME=$(echo "scale=2; $RAUC_INSTALL_END - $RAUC_INSTALL_START" | bc)
    log "Install time: ${RAUC_INSTALL_TIME}s (excluding reboot)"
    
    RAUC_REBOOT="Required (~30s)"
else
    log "RAUC bundle not found at $RAUC_BUNDLE"
    log "To test RAUC, create a bundle using: src/ota/rauc/create-bundle.sh"
    RAUC_SIZE_MB="N/A"
    RAUC_INSTALL_TIME="N/A"
    RAUC_REBOOT="N/A"
fi

echo ""

# ==============================================================================
# Results Summary
# ==============================================================================
echo "=============================================="
echo "   RESULTS SUMMARY"
echo "=============================================="
echo ""

cat >> "$RESULTS_FILE" << EOF
## Results Comparison

| Metric | OTA Scripts (tar.gz) | RAUC (.raucb) |
|--------|---------------------|---------------|
| **Package Size** | ${SCRIPTS_SIZE_KB} KB | ${RAUC_SIZE_MB} MB |
| **Download Time** | ${SCRIPTS_DOWNLOAD_TIME}s | N/A (pre-loaded) |
| **Install Time** | ${SCRIPTS_UPDATE_TIME}s | ${RAUC_INSTALL_TIME}s |
| **Reboot Required** | No | $RAUC_REBOOT |
| **Service Status** | $SCRIPTS_SERVICE_STATUS | N/A |

---

## Detailed Metrics

### OTA Scripts (tar.gz)

- **Package URL:** \`$PACKAGE_URL\`
- **Package Size:** ${SCRIPTS_SIZE_KB} KB (${SCRIPTS_SIZE} bytes)
- **Download Time:** ${SCRIPTS_DOWNLOAD_TIME}s
- **Total Update Time:** ${SCRIPTS_UPDATE_TIME}s
- **Service Status:** $SCRIPTS_SERVICE_STATUS
- **Rollback:** Symlink switch (instant)

### RAUC (.raucb)

- **Bundle Path:** \`$RAUC_BUNDLE\`
- **Bundle Size:** ${RAUC_SIZE_MB} MB
- **Install Time:** ${RAUC_INSTALL_TIME}s
- **Reboot Required:** Yes (~30 seconds)
- **Rollback:** Automatic on boot failure

---

## Conclusions

EOF

printf "| %-20s | %-20s | %-20s |\n" "Metric" "OTA Scripts" "RAUC"
printf "| %-20s | %-20s | %-20s |\n" "--------------------" "--------------------" "--------------------"
printf "| %-20s | %-20s | %-20s |\n" "Package Size" "${SCRIPTS_SIZE_KB} KB" "${RAUC_SIZE_MB} MB"
printf "| %-20s | %-20s | %-20s |\n" "Download Time" "${SCRIPTS_DOWNLOAD_TIME}s" "N/A"
printf "| %-20s | %-20s | %-20s |\n" "Install Time" "${SCRIPTS_UPDATE_TIME}s" "${RAUC_INSTALL_TIME}s"
printf "| %-20s | %-20s | %-20s |\n" "Reboot Required" "No" "$RAUC_REBOOT"
printf "| %-20s | %-20s | %-20s |\n" "Service Status" "$SCRIPTS_SERVICE_STATUS" "N/A"

echo ""

# ==============================================================================
# Recommendations
# ==============================================================================

cat >> "$RESULTS_FILE" << EOF
### Recommendations

Based on the benchmark results:

1. **For frequent updates (daily/weekly):** Use **OTA Scripts**
   - Much smaller packages (${SCRIPTS_SIZE_KB} KB vs ${RAUC_SIZE_MB} MB)
   - No reboot required
   - Total time: ~${SCRIPTS_UPDATE_TIME}s

2. **For major releases (monthly/quarterly):** Use **RAUC**
   - Full system update
   - Automatic rollback on boot failure
   - More secure (X.509 signatures)

3. **For limited bandwidth environments:** Use **OTA Scripts**
   - Package size difference is significant

---

*Generated by benchmark-ota.sh on $(date)*
EOF

echo "Results saved to: $RESULTS_FILE"
echo ""
log "Benchmark complete!"
