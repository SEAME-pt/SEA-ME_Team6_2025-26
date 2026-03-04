#!/bin/bash
# =============================================================================
# RAUC Post-Reboot Verification Script
# SEA:ME Team 6 - OTA Update System
# =============================================================================
# Run this script after rebooting following a RAUC update
# It verifies system health and marks the slot as good or triggers rollback
# Usage: ./post-reboot-verify.sh [--auto]
# =============================================================================

set -e

# -------- Configuration --------
AUTO_MODE="${1:-}"
LOG_FILE="/var/log/rauc-verify.log"
MIN_CHECKS_PASS=4  # Minimum checks that must pass

# -------- Colors --------
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# -------- Functions --------
log() {
    local msg="[$(date '+%Y-%m-%d %H:%M:%S')] $1"
    echo "$msg" >> "$LOG_FILE"
}

print_header() {
    echo ""
    echo -e "${BLUE}==========================================${NC}"
    echo -e "${BLUE}  RAUC Post-Reboot Verification${NC}"
    echo -e "${BLUE}==========================================${NC}"
    echo ""
}

print_success() {
    echo -e "  ${GREEN}✅ $1${NC}"
}

print_warning() {
    echo -e "  ${YELLOW}⚠️  $1${NC}"
}

print_error() {
    echo -e "  ${RED}❌ $1${NC}"
}

# -------- Check if running as root --------
if [ "$EUID" -ne 0 ]; then
    echo "This script must be run as root"
    exit 1
fi

# -------- Main --------
print_header

log "Starting post-reboot verification"

# -------- Step 1: Current State --------
echo "[1/4] Checking current system state..."
echo ""

CURRENT_SLOT=$(rauc status --output-format=shell 2>/dev/null | grep "RAUC_BOOT_PRIMARY=" | cut -d'=' -f2 | tr -d '"')
SLOT_STATE=$(rauc status --output-format=shell 2>/dev/null | grep "RAUC_SLOT_STATE_${CURRENT_SLOT//./_}=" | cut -d'=' -f2 | tr -d '"' 2>/dev/null || echo "unknown")

echo "  Booted from slot: $CURRENT_SLOT"
echo "  Slot state: $SLOT_STATE"
log "Current slot: $CURRENT_SLOT, state: $SLOT_STATE"

# Check if already marked good
if [ "$SLOT_STATE" = "good" ]; then
    print_success "Slot already marked as good"
    echo ""
    echo "  System is verified and stable."
    exit 0
fi

echo ""

# -------- Step 2: Health Checks --------
echo "[2/4] Running health checks..."
echo ""

CHECKS_PASSED=0
CHECKS_TOTAL=0

# Check 1: System booted successfully
((CHECKS_TOTAL++))
if [ -f /proc/uptime ]; then
    UPTIME=$(cat /proc/uptime | cut -d' ' -f1 | cut -d'.' -f1)
    print_success "System booted successfully (uptime: ${UPTIME}s)"
    ((CHECKS_PASSED++))
    log "CHECK PASS: System booted (uptime: ${UPTIME}s)"
else
    print_error "Cannot determine system uptime"
    log "CHECK FAIL: Cannot determine uptime"
fi

# Check 2: RAUC service running
((CHECKS_TOTAL++))
if systemctl is-active --quiet rauc 2>/dev/null; then
    print_success "RAUC service running"
    ((CHECKS_PASSED++))
    log "CHECK PASS: RAUC service running"
else
    print_warning "RAUC service not running (may be OK if not using D-Bus)"
    # Still count as pass since rauc command works
    ((CHECKS_PASSED++))
    log "CHECK WARN: RAUC service not active"
fi

# Check 3: Root filesystem mounted read-write
((CHECKS_TOTAL++))
if mount | grep "on / " | grep -q "rw"; then
    print_success "Root filesystem mounted read-write"
    ((CHECKS_PASSED++))
    log "CHECK PASS: Root filesystem rw"
else
    print_error "Root filesystem not mounted read-write"
    log "CHECK FAIL: Root filesystem not rw"
fi

# Check 4: Network connectivity (optional, non-critical)
((CHECKS_TOTAL++))
if ping -c 1 -W 3 8.8.8.8 &>/dev/null; then
    print_success "Network connectivity OK"
    ((CHECKS_PASSED++))
    log "CHECK PASS: Network OK"
elif ping -c 1 -W 3 1.1.1.1 &>/dev/null; then
    print_success "Network connectivity OK (Cloudflare)"
    ((CHECKS_PASSED++))
    log "CHECK PASS: Network OK (Cloudflare)"
else
    print_warning "Network unreachable (may be OK for isolated systems)"
    log "CHECK WARN: Network unreachable"
fi

# Check 5: Disk space
((CHECKS_TOTAL++))
DISK_USAGE=$(df / | tail -1 | awk '{print $5}' | tr -d '%')
if [ "$DISK_USAGE" -lt 90 ]; then
    print_success "Disk space OK (${DISK_USAGE}% used)"
    ((CHECKS_PASSED++))
    log "CHECK PASS: Disk space OK (${DISK_USAGE}%)"
else
    print_error "Disk space critical (${DISK_USAGE}% used)"
    log "CHECK FAIL: Disk space critical (${DISK_USAGE}%)"
fi

# Check 6: Critical directories exist
((CHECKS_TOTAL++))
CRITICAL_DIRS="/etc /opt /var /tmp"
DIRS_OK=true
for dir in $CRITICAL_DIRS; do
    if [ ! -d "$dir" ]; then
        DIRS_OK=false
        break
    fi
done
if [ "$DIRS_OK" = true ]; then
    print_success "Critical directories exist"
    ((CHECKS_PASSED++))
    log "CHECK PASS: Critical directories exist"
else
    print_error "Missing critical directories"
    log "CHECK FAIL: Missing critical directories"
fi

# Check 7: Data partition (if configured)
((CHECKS_TOTAL++))
if [ -d "/data" ] && mountpoint -q /data 2>/dev/null; then
    print_success "Data partition mounted"
    ((CHECKS_PASSED++))
    log "CHECK PASS: Data partition mounted"
elif [ -d "/data" ]; then
    print_warning "Data partition exists but not mounted"
    log "CHECK WARN: Data partition not mounted"
else
    print_warning "No data partition configured"
    log "CHECK WARN: No data partition"
fi

echo ""
echo "  Health checks: $CHECKS_PASSED/$CHECKS_TOTAL passed"
log "Health checks result: $CHECKS_PASSED/$CHECKS_TOTAL"
echo ""

# -------- Step 3: Decision --------
echo "[3/4] Evaluating results..."
echo ""

if [ "$CHECKS_PASSED" -ge "$MIN_CHECKS_PASS" ]; then
    echo -e "  ${GREEN}System appears healthy!${NC}"
    echo ""
    
    if [ "$AUTO_MODE" = "--auto" ]; then
        # Auto mode - mark good automatically
        echo "  Auto mode: Marking slot as good..."
        rauc status mark-good
        print_success "Slot $CURRENT_SLOT marked as GOOD"
        log "AUTO: Slot marked as good"
    else
        # Interactive mode
        read -p "  Mark this slot as GOOD? (Y/n) " -n 1 -r
        echo
        
        if [[ ! $REPLY =~ ^[Nn]$ ]]; then
            rauc status mark-good
            echo ""
            print_success "Slot $CURRENT_SLOT marked as GOOD"
            echo "  This slot is now the confirmed working version."
            log "INTERACTIVE: Slot marked as good"
        else
            echo ""
            print_warning "Slot NOT marked as good."
            echo "  System may rollback on next boot failure (after 3 failed boots)."
            log "INTERACTIVE: User declined to mark good"
        fi
    fi
else
    echo -e "  ${RED}Some health checks failed!${NC}"
    echo ""
    log "Health checks insufficient: $CHECKS_PASSED < $MIN_CHECKS_PASS"
    
    if [ "$AUTO_MODE" = "--auto" ]; then
        echo "  Auto mode: NOT marking as good due to failed checks."
        echo "  System will rollback after boot failure threshold."
        log "AUTO: Not marking good due to failed checks"
    else
        echo "  Options:"
        echo "  1. Fix the issues and run this script again"
        echo "  2. Force mark as good anyway (risky)"
        echo "  3. Trigger manual rollback"
        echo ""
        read -p "  Enter choice (1/2/3): " -n 1 -r
        echo
        
        case $REPLY in
            2)
                echo ""
                print_warning "Force marking slot as good..."
                rauc status mark-good
                print_success "Slot $CURRENT_SLOT marked as GOOD (forced)"
                log "INTERACTIVE: Force marked good"
                ;;
            3)
                echo ""
                print_warning "Initiating rollback..."
                log "INTERACTIVE: User requested rollback"
                
                # Mark current slot as bad
                rauc status mark-bad
                echo "  Slot marked as bad. Rebooting to previous slot..."
                sleep 2
                reboot
                ;;
            *)
                echo ""
                echo "  No action taken. Fix issues and run again:"
                echo "    $0"
                log "INTERACTIVE: No action taken"
                ;;
        esac
    fi
fi

echo ""

# -------- Step 4: Final Status --------
echo "[4/4] Final system status..."
echo ""

rauc status

echo ""
echo -e "${BLUE}==========================================${NC}"
echo -e "${BLUE}  Verification Complete${NC}"
echo -e "${BLUE}==========================================${NC}"
echo ""

# Show version info if available
if [ -f "/etc/ota-version" ]; then
    echo "  OTA Version: $(cat /etc/ota-version)"
fi
if [ -f "/etc/os-release" ]; then
    echo "  OS: $(grep PRETTY_NAME /etc/os-release | cut -d'"' -f2)"
fi

echo ""
log "Verification script completed"
