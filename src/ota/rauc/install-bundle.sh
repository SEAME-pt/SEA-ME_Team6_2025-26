#!/bin/bash
# =============================================================================
# RAUC Bundle Installation Script
# SEA:ME Team 6 - OTA Update System
# =============================================================================
# Installs a RAUC bundle (.raucb) to the inactive slot
# Usage: ./install-bundle.sh <bundle.raucb> [auto-reboot]
# =============================================================================

set -e

# -------- Configuration --------
BUNDLE_PATH="${1:-}"
AUTO_REBOOT="${2:-no}"
LOG_FILE="/var/log/rauc-install.log"

# -------- Colors --------
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# -------- Functions --------
log() {
    local msg="[$(date '+%Y-%m-%d %H:%M:%S')] $1"
    echo -e "$msg"
    echo "$msg" >> "$LOG_FILE"
}

print_header() {
    echo ""
    echo -e "${BLUE}==========================================${NC}"
    echo -e "${BLUE}  RAUC Bundle Installation${NC}"
    echo -e "${BLUE}==========================================${NC}"
    echo ""
}

print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

# -------- Validation --------
if [ -z "$BUNDLE_PATH" ]; then
    echo "Usage: $0 <bundle.raucb> [auto-reboot]"
    echo ""
    echo "Arguments:"
    echo "  bundle.raucb    Path to the RAUC bundle file"
    echo "  auto-reboot     'yes' to reboot automatically after install (default: no)"
    echo ""
    echo "Examples:"
    echo "  $0 /tmp/update-rpi5.raucb"
    echo "  $0 /tmp/update-rpi5.raucb yes"
    exit 1
fi

if [ ! -f "$BUNDLE_PATH" ]; then
    print_error "Bundle not found: $BUNDLE_PATH"
    exit 1
fi

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    print_error "This script must be run as root"
    exit 1
fi

# Check if RAUC is available
if ! command -v rauc &> /dev/null; then
    print_error "RAUC is not installed"
    exit 1
fi

# -------- Main --------
print_header

log "Starting RAUC bundle installation"
log "Bundle: $BUNDLE_PATH"
log "Auto-reboot: $AUTO_REBOOT"

# -------- Step 1: Pre-install status --------
echo "[1/5] Checking current system status..."
CURRENT_SLOT=$(rauc status --output-format=shell 2>/dev/null | grep "RAUC_BOOT_PRIMARY=" | cut -d'=' -f2 | tr -d '"')

if [ -z "$CURRENT_SLOT" ]; then
    CURRENT_SLOT="unknown"
fi

echo "  Current active slot: $CURRENT_SLOT"
log "Current active slot: $CURRENT_SLOT"
echo ""

# -------- Step 2: Verify bundle --------
echo "[2/5] Verifying bundle integrity..."

if ! rauc info "$BUNDLE_PATH" > /tmp/rauc-bundle-info.txt 2>&1; then
    print_error "Bundle verification failed!"
    cat /tmp/rauc-bundle-info.txt
    exit 1
fi

# Extract bundle info
BUNDLE_COMPATIBLE=$(grep "Compatible:" /tmp/rauc-bundle-info.txt | awk '{print $2}' || echo "unknown")
BUNDLE_VERSION=$(grep "Version:" /tmp/rauc-bundle-info.txt | awk '{print $2}' || echo "unknown")

echo "  Bundle compatible: $BUNDLE_COMPATIBLE"
echo "  Bundle version: $BUNDLE_VERSION"
log "Bundle: compatible=$BUNDLE_COMPATIBLE, version=$BUNDLE_VERSION"

# Check compatibility
SYSTEM_COMPATIBLE=$(grep "compatible=" /etc/rauc/system.conf 2>/dev/null | cut -d'=' -f2 || echo "unknown")
if [ "$BUNDLE_COMPATIBLE" != "$SYSTEM_COMPATIBLE" ] && [ "$BUNDLE_COMPATIBLE" != "unknown" ]; then
    print_warning "Bundle compatible ($BUNDLE_COMPATIBLE) may not match system ($SYSTEM_COMPATIBLE)"
    read -p "  Continue anyway? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        log "Installation aborted by user (compatibility mismatch)"
        exit 1
    fi
fi

print_success "Bundle verified"
echo ""

# -------- Step 3: Backup current config (optional) --------
echo "[3/5] Backing up current configuration..."

BACKUP_DIR="/data/backup/pre-update-$(date +%Y%m%d-%H%M%S)"
if [ -d "/data" ]; then
    mkdir -p "$BACKUP_DIR"
    
    # Backup important configs if they exist
    [ -d "/etc/kuksa" ] && cp -a /etc/kuksa "$BACKUP_DIR/" 2>/dev/null || true
    [ -f "/etc/network/interfaces" ] && cp /etc/network/interfaces "$BACKUP_DIR/" 2>/dev/null || true
    [ -d "/opt/ota/config" ] && cp -a /opt/ota/config "$BACKUP_DIR/" 2>/dev/null || true
    
    echo "  Backup saved to: $BACKUP_DIR"
    log "Config backup: $BACKUP_DIR"
else
    print_warning "Data partition not mounted, skipping backup"
fi

print_success "Backup complete"
echo ""

# -------- Step 4: Install bundle --------
echo "[4/5] Installing bundle to inactive slot..."
log "Starting RAUC install..."

if rauc install "$BUNDLE_PATH" 2>&1 | tee -a "$LOG_FILE"; then
    print_success "Bundle installed successfully"
    log "RAUC install completed successfully"
else
    print_error "Bundle installation failed!"
    log "RAUC install FAILED"
    exit 1
fi
echo ""

# -------- Step 5: Post-install status --------
echo "[5/5] Verifying installation..."
echo ""

rauc status

NEW_PRIMARY=$(rauc status --output-format=shell 2>/dev/null | grep "RAUC_BOOT_PRIMARY=" | cut -d'=' -f2 | tr -d '"')

echo ""
echo -e "${BLUE}==========================================${NC}"
echo -e "${GREEN}  Installation Complete!${NC}"
echo -e "${BLUE}==========================================${NC}"
echo ""
echo "  Previous boot slot: $CURRENT_SLOT"
echo "  Next boot slot: $NEW_PRIMARY"
echo ""

log "Installation complete. Previous=$CURRENT_SLOT, Next=$NEW_PRIMARY"

# -------- Reboot handling --------
if [ "$AUTO_REBOOT" = "yes" ]; then
    echo "  Auto-reboot enabled. Rebooting in 5 seconds..."
    log "Auto-reboot initiated"
    sleep 5
    sync
    reboot
else
    echo "  To activate the update, reboot the system:"
    echo "    sudo reboot"
    echo ""
    echo "  After reboot, run verification:"
    echo "    /opt/ota/rauc/post-reboot-verify.sh"
fi

echo ""
