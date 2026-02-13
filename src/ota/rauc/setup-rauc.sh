#!/bin/bash
# RAUC Setup Script for AGL Devices
# SEA:ME Team 6 - Phase D: A/B rootfs updates
#
# Usage: ./setup-rauc.sh
# Run this script ON the AGL device (RPi4 or RPi5)

set -e

log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

error() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] ERROR: $1" >&2
    exit 1
}

# Detect platform
detect_platform() {
    ARCH=$(uname -m)
    if [ -f /proc/device-tree/model ]; then
        MODEL=$(cat /proc/device-tree/model | tr -d '\0')
    else
        MODEL="unknown"
    fi
    
    case "$ARCH" in
        aarch64)
            PLATFORM="rpi5"
            ;;
        armv7l)
            PLATFORM="rpi4"
            ;;
        *)
            error "Unknown architecture: $ARCH"
            ;;
    esac
    
    log "Detected platform: $PLATFORM ($ARCH)"
    log "Device model: $MODEL"
}

# Check if RAUC is installed
check_rauc() {
    if ! command -v rauc &> /dev/null; then
        error "RAUC is not installed. Please install RAUC first."
    fi
    
    RAUC_VERSION=$(rauc --version)
    log "RAUC version: $RAUC_VERSION"
}

# Check partitions
check_partitions() {
    log "Checking partitions..."
    
    # Check rootfs-A (current)
    if [ ! -b /dev/mmcblk0p2 ]; then
        error "Partition /dev/mmcblk0p2 (rootfs-A) not found"
    fi
    
    # Check rootfs-B
    if [ ! -b /dev/mmcblk0p3 ]; then
        error "Partition /dev/mmcblk0p3 (rootfs-B) not found"
    fi
    
    # Check data partition
    if [ ! -b /dev/mmcblk0p4 ]; then
        log "WARNING: Partition /dev/mmcblk0p4 (/data) not found"
    fi
    
    log "Partitions OK"
    lsblk /dev/mmcblk0
}

# Install RAUC configuration
install_config() {
    log "Installing RAUC configuration for $PLATFORM..."
    
    # Create directories
    mkdir -p /etc/rauc
    mkdir -p /usr/lib/rauc
    mkdir -p /data
    
    # Determine config file
    CONFIG_FILE="system.conf.$PLATFORM"
    
    if [ ! -f "$CONFIG_FILE" ]; then
        error "Configuration file $CONFIG_FILE not found in current directory"
    fi
    
    # Install system.conf
    cp "$CONFIG_FILE" /etc/rauc/system.conf
    log "Installed /etc/rauc/system.conf"
    
    # Install certificate
    if [ -f "ca.cert.pem" ]; then
        cp ca.cert.pem /etc/rauc/ca.cert.pem
        log "Installed /etc/rauc/ca.cert.pem"
    else
        error "Certificate ca.cert.pem not found"
    fi
    
    # Install custom bootloader backend
    if [ -f "bootloader-custom-backend.sh" ]; then
        cp bootloader-custom-backend.sh /usr/lib/rauc/bootloader-custom-backend.sh
        chmod +x /usr/lib/rauc/bootloader-custom-backend.sh
        log "Installed /usr/lib/rauc/bootloader-custom-backend.sh"
    else
        error "Custom bootloader backend script not found"
    fi
    
    # Install post-install handler
    if [ -f "post-install.sh" ]; then
        cp post-install.sh /usr/lib/rauc/post-install.sh
        chmod +x /usr/lib/rauc/post-install.sh
        log "Installed /usr/lib/rauc/post-install.sh"
    else
        error "Post-install script not found"
    fi
}

# Initialize RAUC status
init_status() {
    log "Initializing RAUC status..."
    
    # Mount /data if not mounted
    if ! mountpoint -q /data; then
        mount /dev/mmcblk0p4 /data 2>/dev/null || log "WARNING: Could not mount /data"
    fi
    
    # Create initial status file
    if [ ! -f /data/rauc.status ]; then
        cat > /data/rauc.status << EOF
[slot.rootfs.0]
status=ok
boot-status=good

[slot.rootfs.1]
status=ok
boot-status=good
EOF
        log "Created /data/rauc.status"
    fi
}

# Enable RAUC service
enable_service() {
    log "Enabling RAUC service..."
    
    if systemctl list-unit-files | grep -q rauc; then
        systemctl enable rauc || log "WARNING: Could not enable rauc service"
        systemctl start rauc || log "WARNING: Could not start rauc service"
        log "RAUC service enabled and started"
    else
        log "WARNING: RAUC service not found in systemd"
    fi
}

# Verify installation
verify() {
    log "Verifying RAUC installation..."
    
    # Check config
    if rauc status 2>/dev/null; then
        log "RAUC status: OK"
    else
        log "WARNING: 'rauc status' failed - this might be normal if bootloader integration is pending"
    fi
    
    # Show configuration
    log "Configuration files:"
    ls -la /etc/rauc/
    
    log "Handler scripts:"
    ls -la /usr/lib/rauc/
}

# Main
main() {
    log "=== RAUC Setup for SEA:ME Team 6 ==="
    
    detect_platform
    check_rauc
    check_partitions
    install_config
    init_status
    enable_service
    verify
    
    log "=== RAUC Setup Complete ==="
    log ""
    log "Next steps:"
    log "1. Create a RAUC bundle (.raucb) with the rootfs image"
    log "2. Install the bundle: rauc install <bundle.raucb>"
    log "3. Reboot to test A/B switching"
    log ""
    log "Current slot: rootfs.0 (A) at /dev/mmcblk0p2"
    log "Inactive slot: rootfs.1 (B) at /dev/mmcblk0p3"
}

main "$@"
