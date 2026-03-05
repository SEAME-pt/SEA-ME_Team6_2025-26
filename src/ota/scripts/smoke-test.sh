#!/bin/bash
# smoke-test.sh - Post-update smoke test for OTA
# Verifies that services are running correctly after an update
#
# Usage: ./smoke-test.sh [platform]
# Example: ./smoke-test.sh rpi5
#
# Exit codes:
#   0 = All tests passed
#   1 = Test failed

set -e

PLATFORM="${1:-auto}"
LOG_FILE="/opt/ota/logs/smoke-test.log"

# Auto-detect platform
if [ "$PLATFORM" = "auto" ]; then
    ARCH=$(uname -m)
    case "$ARCH" in
        aarch64) PLATFORM="rpi5" ;;
        armv7l)  PLATFORM="rpi4" ;;
        *)       echo "Unknown architecture: $ARCH"; exit 1 ;;
    esac
fi

log() {
    local msg="[$(date '+%Y-%m-%d %H:%M:%S')] $1"
    echo "$msg"
    echo "$msg" >> "$LOG_FILE"
}

test_passed() {
    log "✅ PASS: $1"
}

test_failed() {
    log "❌ FAIL: $1"
    return 1
}

# Create log directory if needed
mkdir -p "$(dirname "$LOG_FILE")"

log "=== Smoke Test Started ==="
log "Platform: $PLATFORM"

TESTS_PASSED=0
TESTS_FAILED=0

# ==============================================================================
# Test 1: Service is active
# ==============================================================================
log ""
log "Test 1: Service Status"

if [ "$PLATFORM" = "rpi5" ]; then
    SERVICE="can-to-kuksa.service"
    BINARY="can_to_kuksa_publisher"
    BINARY_PATH="/home/kuksa_RPi5/bin/can_to_kuksa_publisher"
else
    SERVICE="helloqt-app.service"
    BINARY="HelloQt6Qml"
    BINARY_PATH="/opt/cluster/HelloQt6Qml"
fi

if systemctl is-active --quiet "$SERVICE"; then
    test_passed "Service $SERVICE is active"
    ((TESTS_PASSED++))
else
    test_failed "Service $SERVICE is not active"
    ((TESTS_FAILED++))
fi

# ==============================================================================
# Test 2: Process is running
# ==============================================================================
log ""
log "Test 2: Process Running"

if pgrep -f "$BINARY" > /dev/null; then
    test_passed "Process $BINARY is running"
    ((TESTS_PASSED++))
else
    test_failed "Process $BINARY is not running"
    ((TESTS_FAILED++))
fi

# ==============================================================================
# Test 3: No restart loop
# ==============================================================================
log ""
log "Test 3: Restart Loop Detection"

RESTARTS=$(systemctl show "$SERVICE" -p NRestarts --value 2>/dev/null || echo "0")
if [ "$RESTARTS" -lt 3 ]; then
    test_passed "Service restarts: $RESTARTS (< 3 threshold)"
    ((TESTS_PASSED++))
else
    test_failed "Service in restart loop: $RESTARTS restarts"
    ((TESTS_FAILED++))
fi

# ==============================================================================
# Test 4: Binary exists and is executable
# ==============================================================================
log ""
log "Test 4: Binary Verification"

if [ -x "$BINARY_PATH" ]; then
    test_passed "Binary $BINARY_PATH exists and is executable"
    ((TESTS_PASSED++))
else
    test_failed "Binary $BINARY_PATH not found or not executable"
    ((TESTS_FAILED++))
fi

# ==============================================================================
# Test 5: Binary architecture matches system
# ==============================================================================
log ""
log "Test 5: Architecture Check"

ARCH=$(uname -m)
BINARY_INFO=$(file "$BINARY_PATH" 2>/dev/null || echo "unknown")

case "$ARCH" in
    aarch64)
        if echo "$BINARY_INFO" | grep -q "64-bit\|aarch64"; then
            test_passed "Binary is 64-bit (matches $ARCH)"
            ((TESTS_PASSED++))
        else
            test_failed "Binary architecture mismatch for $ARCH"
            ((TESTS_FAILED++))
        fi
        ;;
    armv7l)
        if echo "$BINARY_INFO" | grep -q "32-bit\|ARM"; then
            test_passed "Binary is 32-bit ARM (matches $ARCH)"
            ((TESTS_PASSED++))
        else
            test_failed "Binary architecture mismatch for $ARCH"
            ((TESTS_FAILED++))
        fi
        ;;
esac

# ==============================================================================
# Test 6: Version file exists
# ==============================================================================
log ""
log "Test 6: Version Tracking"

if [ -f "/etc/ota-version" ]; then
    VERSION=$(cat /etc/ota-version)
    test_passed "Version file exists: $VERSION"
    ((TESTS_PASSED++))
else
    test_failed "Version file /etc/ota-version not found"
    ((TESTS_FAILED++))
fi

# ==============================================================================
# Test 7: OTA symlink is valid
# ==============================================================================
log ""
log "Test 7: OTA Symlink"

if [ -L "/opt/ota/current" ] && [ -d "/opt/ota/current" ]; then
    SYMLINK_TARGET=$(readlink -f /opt/ota/current)
    test_passed "OTA symlink valid: $SYMLINK_TARGET"
    ((TESTS_PASSED++))
else
    test_failed "OTA symlink /opt/ota/current is invalid"
    ((TESTS_FAILED++))
fi

# ==============================================================================
# Platform-specific tests
# ==============================================================================

if [ "$PLATFORM" = "rpi5" ]; then
    log ""
    log "Test 8: VSS Configuration (RPi5 specific)"
    
    if [ -f "/home/kuksa_RPi5/vss_min.json" ]; then
        test_passed "VSS config vss_min.json exists"
        ((TESTS_PASSED++))
    else
        test_failed "VSS config vss_min.json not found"
        ((TESTS_FAILED++))
    fi
fi

# ==============================================================================
# Summary
# ==============================================================================
log ""
log "=== Smoke Test Summary ==="
log "Passed: $TESTS_PASSED"
log "Failed: $TESTS_FAILED"
log "Total:  $((TESTS_PASSED + TESTS_FAILED))"

if [ "$TESTS_FAILED" -eq 0 ]; then
    log ""
    log "✅ ALL TESTS PASSED - Update is healthy!"
    exit 0
else
    log ""
    log "❌ SOME TESTS FAILED - Consider rollback!"
    exit 1
fi
