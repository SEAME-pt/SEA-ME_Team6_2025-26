#!/bin/bash
# RAUC Post-Install Handler for Raspberry Pi
# Updates /boot/cmdline.txt to boot from the newly installed slot
#
# SEA:ME Team 6 - AGL A/B Update System

set -e

SLOT_NAME="$RAUC_SLOT_NAME"
SLOT_BOOTNAME="$RAUC_SLOT_BOOTNAME"

log() {
    echo "[RAUC post-install] $1"
    logger -t rauc-post-install "$1"
}

log "Post-install handler triggered"
log "Slot name: $SLOT_NAME"
log "Slot bootname: $SLOT_BOOTNAME"

# Determine the new root partition based on slot
case "$SLOT_BOOTNAME" in
    A)
        NEW_ROOT="/dev/mmcblk0p2"
        ;;
    B)
        NEW_ROOT="/dev/mmcblk0p3"
        ;;
    *)
        log "ERROR: Unknown slot bootname: $SLOT_BOOTNAME"
        exit 1
        ;;
esac

log "New root partition: $NEW_ROOT"

# Backup current cmdline.txt
CMDLINE="/boot/cmdline.txt"
CMDLINE_BACKUP="/boot/cmdline.txt.bak"

if [ -f "$CMDLINE" ]; then
    cp "$CMDLINE" "$CMDLINE_BACKUP"
    log "Backed up $CMDLINE to $CMDLINE_BACKUP"
fi

# Update root= parameter in cmdline.txt
if [ -f "$CMDLINE" ]; then
    # Read current cmdline
    CURRENT=$(cat "$CMDLINE")
    
    # Replace root= parameter
    NEW_CMDLINE=$(echo "$CURRENT" | sed "s|root=/dev/mmcblk0p[0-9]*|root=$NEW_ROOT|g")
    
    # Write new cmdline
    echo "$NEW_CMDLINE" > "$CMDLINE"
    
    log "Updated $CMDLINE with root=$NEW_ROOT"
    log "New cmdline: $NEW_CMDLINE"
else
    log "ERROR: $CMDLINE not found"
    exit 1
fi

# Sync to ensure changes are written
sync

log "Post-install completed successfully"
exit 0
