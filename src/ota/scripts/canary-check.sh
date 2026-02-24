#!/bin/bash
# canary-check.sh - Canary deployment support for OTA
# 
# This script implements a canary deployment strategy:
# - "canary" devices receive updates immediately
# - "production" devices wait a configurable delay after canary
#
# Usage: Source this script in ota-check.sh or use standalone
# Example: ./canary-check.sh <version>

CANARY_DELAY_HOURS="${CANARY_DELAY_HOURS:-24}"
DEVICE_ROLE_FILE="/etc/ota-device-role"
CANARY_STATUS_FILE="/opt/ota/canary-status.json"
LOG_FILE="/opt/ota/logs/canary.log"

log() {
    local msg="[$(date '+%Y-%m-%d %H:%M:%S')] $1"
    echo "$msg"
    echo "$msg" >> "$LOG_FILE"
}

# Get device role (canary or production)
get_device_role() {
    if [ -f "$DEVICE_ROLE_FILE" ]; then
        cat "$DEVICE_ROLE_FILE"
    else
        # Default: check by IP or hostname
        local hostname=$(hostname)
        case "$hostname" in
            *rpi5*|*seame-agl*) echo "canary" ;;
            *rpi4*) echo "production" ;;
            *) echo "production" ;;
        esac
    fi
}

# Set device role
set_device_role() {
    local role="$1"
    if [ "$role" = "canary" ] || [ "$role" = "production" ]; then
        echo "$role" > "$DEVICE_ROLE_FILE"
        log "Device role set to: $role"
    else
        log "ERROR: Invalid role. Use 'canary' or 'production'"
        return 1
    fi
}

# Check if canary period has passed
check_canary_period() {
    local version="$1"
    
    # If we're the canary, always allow update
    local role=$(get_device_role)
    if [ "$role" = "canary" ]; then
        log "Device is canary - update allowed immediately"
        return 0
    fi
    
    # For production devices, check if canary period has passed
    if [ ! -f "$CANARY_STATUS_FILE" ]; then
        log "No canary status found - checking GitHub release time"
        
        # Get release publish time from GitHub
        local release_time=$(curl -s "https://api.github.com/repos/SEAME-pt/SEA-ME_Team6_2025-26/releases/tags/$version" | \
            grep '"published_at"' | head -1 | cut -d'"' -f4)
        
        if [ -z "$release_time" ]; then
            log "Could not get release time - defaulting to allow update"
            return 0
        fi
        
        # Calculate hours since release
        local release_epoch=$(date -d "$release_time" +%s 2>/dev/null || date -j -f "%Y-%m-%dT%H:%M:%SZ" "$release_time" +%s 2>/dev/null || echo "0")
        local now_epoch=$(date +%s)
        local hours_since=$(( (now_epoch - release_epoch) / 3600 ))
        
        log "Release time: $release_time"
        log "Hours since release: $hours_since"
        log "Required canary period: $CANARY_DELAY_HOURS hours"
        
        if [ "$hours_since" -ge "$CANARY_DELAY_HOURS" ]; then
            log "Canary period passed - update allowed"
            return 0
        else
            local remaining=$(( CANARY_DELAY_HOURS - hours_since ))
            log "Canary period not passed - $remaining hours remaining"
            return 1
        fi
    fi
    
    return 0
}

# Report canary status (called by canary device after successful update)
report_canary_success() {
    local version="$1"
    local status="$2"  # "success" or "failure"
    
    cat > "$CANARY_STATUS_FILE" << EOF
{
    "version": "$version",
    "status": "$status",
    "timestamp": "$(date -Iseconds)",
    "device": "$(hostname)",
    "role": "$(get_device_role)"
}
EOF
    
    log "Canary status reported: $version - $status"
}

# Show canary deployment status
show_status() {
    echo "=== Canary Deployment Status ==="
    echo ""
    echo "Device Role: $(get_device_role)"
    echo "Canary Delay: $CANARY_DELAY_HOURS hours"
    echo ""
    
    if [ -f "$CANARY_STATUS_FILE" ]; then
        echo "Last Canary Report:"
        cat "$CANARY_STATUS_FILE"
    else
        echo "No canary status recorded"
    fi
}

# Main logic when run directly
main() {
    local cmd="${1:-status}"
    
    case "$cmd" in
        status)
            show_status
            ;;
        set-role)
            set_device_role "$2"
            ;;
        check)
            check_canary_period "$2"
            ;;
        report)
            report_canary_success "$2" "$3"
            ;;
        *)
            echo "Usage: $0 {status|set-role <canary|production>|check <version>|report <version> <success|failure>}"
            exit 1
            ;;
    esac
}

# Only run main if executed directly (not sourced)
if [ "${BASH_SOURCE[0]}" = "$0" ]; then
    mkdir -p "$(dirname "$LOG_FILE")"
    main "$@"
fi
