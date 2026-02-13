#!/bin/bash
# RAUC Custom Bootloader Backend for Raspberry Pi
# SEA:ME Team 6
#
# This script handles boot slot selection for the native RPi bootloader
# RAUC calls this script with specific commands to get/set boot configuration

CMDLINE="/boot/cmdline.txt"
STATUS_FILE="/data/rauc.status"

# Map partition to bootname
get_bootname_from_partition() {
    local partition="$1"
    case "$partition" in
        /dev/mmcblk0p2) echo "A" ;;
        /dev/mmcblk0p3) echo "B" ;;
        *) echo "A" ;;  # Default to A
    esac
}

# Map bootname to partition
get_partition_from_bootname() {
    local bootname="$1"
    case "$bootname" in
        A) echo "/dev/mmcblk0p2" ;;
        B) echo "/dev/mmcblk0p3" ;;
        *) echo "/dev/mmcblk0p2" ;;
    esac
}

# Get current root partition from cmdline.txt
get_current_root() {
    if [ -f "$CMDLINE" ]; then
        grep -oE 'root=/dev/mmcblk0p[23]' "$CMDLINE" | cut -d= -f2
    else
        echo "/dev/mmcblk0p2"  # Default to slot A
    fi
}

# Get currently booted slot (returns bootname: A or B)
get_primary() {
    local root=$(get_current_root)
    get_bootname_from_partition "$root"
}

# Get the state of a slot (good, bad, or unknown)
get_state() {
    local bootname="$1"
    if [ -f "$STATUS_FILE" ]; then
        grep "^${bootname}=" "$STATUS_FILE" | cut -d= -f2
    else
        echo "good"  # Default to good
    fi
}

# Set the primary boot slot (takes bootname: A or B)
set_primary() {
    local bootname="$1"
    local partition=$(get_partition_from_bootname "$bootname")
    
    # Update cmdline.txt
    if [ -f "$CMDLINE" ]; then
        sed -i "s|root=/dev/mmcblk0p[23]|root=${partition}|g" "$CMDLINE"
        sync
        echo "Set primary boot to $bootname ($partition)"
    else
        echo "ERROR: $CMDLINE not found" >&2
        exit 1
    fi
}

# Set the state of a slot
set_state() {
    local bootname="$1"
    local state="$2"
    
    # Ensure status file exists
    touch "$STATUS_FILE"
    
    # Update or add the state
    if grep -q "^${bootname}=" "$STATUS_FILE" 2>/dev/null; then
        sed -i "s|^${bootname}=.*|${bootname}=${state}|" "$STATUS_FILE"
    else
        echo "${bootname}=${state}" >> "$STATUS_FILE"
    fi
    sync
}

# Get boot attempts remaining (not really used with custom backend)
get_remaining_attempts() {
    local bootname="$1"
    echo "3"
}

# Main command handler
case "$1" in
    get-primary)
        get_primary
        ;;
    get-state)
        get_state "$2"
        ;;
    set-primary)
        set_primary "$2"
        ;;
    set-state)
        set_state "$2" "$3"
        ;;
    get-remaining-attempts)
        get_remaining_attempts
        ;;
    *)
        echo "Usage: $0 {get-primary|get-state|set-primary|set-state|get-remaining-attempts}" >&2
        exit 1
        ;;
esac
