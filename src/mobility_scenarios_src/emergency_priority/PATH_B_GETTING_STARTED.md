# Path B Implementation (BLE Direct) — Getting Started

**Status:** In implementation  
**Date:** Tuesday, June 9, 2026  
**Target:** Friday presentation + end-to-end BLE validation

---

## What We Just Created

### 1. AGL Side: BLE Central Receiver (`ble_trafficlight_receiver.py`)

Python service that:
- Scans for micro:bit BLE peripheral (`Trafficlight`)
- Connects and subscribes to Nordic UART notifications
- Receives traffic-light state (R/Y/G)
- Injects into `/tmp/adas_objects.sock` for ADAS Manager
- Auto-reconnects on disconnect
- Forces RED on timeout (safety)

**Dependencies:**
```bash
pip install bleak
```

### 2. Micro:bit Side: BLE Firmware Guide (`MAKECODE_BLE_FIRMWARE_GUIDE.md`)

Two ready-to-use templates:
- **Template A**: Simple (RED only) — fastest prototype
- **Template B**: Full state machine with yellow timeout

Both use Nordic UART Service (standard BLE).

### 3. Configuration (`config_ble.json`)

All settings in one file:
- BLE service/characteristic UUIDs
- ADAS socket path
- Timeouts (5s total, 2s yellow)
- Device name to scan for

### 4. Tests (`test_ble_receiver.py`)

Validates logic without hardware:
- State parsing (R/Y/G → state names)
- ADAS object format
- Yellow timeout escalation
- Connection timeout to RED
- Config loading

---

## Quick Start (Today)

### Step 1: Setup AGL (now)

On your AGL/Pi5:

```bash
cd ~/Documents/SEA-ME_Team6_2025-26

# Install BLE library
pip install bleak

# Copy BLE config
cp src/mobility_scenarios_src/emergency_priority/config_ble.json .

# Verify Bluetooth is available
bluetoothctl show
# Output should show adapter info
```

### Step 2: Flash Firmware (now)

On your PC:

1. Go to https://makecode.microbit.org
2. Create new project
3. Add BLE extension
4. Copy **Template A** code from `MAKECODE_BLE_FIRMWARE_GUIDE.md`
5. Download `.hex` file
6. Flash to micro:bit V2 via USB-C

**Test on micro:bit:**
- Press button A → Red LED flash
- Look for "Trafficlight" in Bluetooth settings (optional)

### Step 3: Run BLE Receiver (when ready)

On AGL in terminal 1:

```bash
cd ~/Documents/SEA-ME_Team6_2025-26

python3 src/mobility_scenarios_src/emergency_priority/ble_trafficlight_receiver.py \
  --config config_ble.json \
  --device-name "Trafficlight" \
  --verbose
```

Expected output:
```
Scanning for device: Trafficlight...
Found device: Trafficlight (XX:XX:XX:XX:XX:XX)
Connecting to XX:XX:XX:XX:XX:XX...
Connected to BLE device
Subscribed to 6e400003-b5a3-f393-e0a9-e50e24dcca9e
BLE notification: raw=52, state=RED
Injected ADAS object: SIGN_TL_RED
```

### Step 4: Verify ADAS Integration (when ready)

In terminal 2 on AGL:

```bash
# Watch ADAS socket
socat - UNIX-CONNECT:/tmp/adas_objects.sock
```

You should see JSON objects arrive every 1-2 seconds:
```json
{"class": "SIGN_TL_RED", "confidence": 1.0, "state": "RED", "timestamp": 1717939234.5}
```

### Step 5: Test State Changes

On micro:bit:
- **Button A** → RED
- **Button B** → YELLOW (escalates to RED after 2s)
- **Button A+B** → GREEN

On AGL, watch socket receive corresponding state changes.

---

## Troubleshooting

### Issue: "Device not found"

**Cause:** micro:bit not advertising  
**Fix:**
1. Restart micro:bit (press RESET on back)
2. Check firmware flashed correctly
3. Run `bluetoothctl scan on` on AGL to manually search

### Issue: Connection fails with timeout

**Cause:** BLE radio range or interference  
**Fix:**
1. Move micro:bit closer to Pi5 (within 1m)
2. Check `bluetoothctl info <device>` on AGL
3. Restart both devices

### Issue: No notifications arriving

**Cause:** Wrong UUID or notification not enabled  
**Fix:**
1. Verify micro:bit firmware calls `bluetooth.uartWriteText()`
2. Check Nordic UART service UUID: `6e400001-b5a3-f393-e0a9-e50e24dcca9e`
3. Run with `--verbose` to see detailed BLE communication

### Issue: ADAS socket not ready

**Cause:** ADAS Manager not running  
**Fix:**
1. Start ADAS Manager: `/data/ADAS-Manager-tuning-trafficlight/adas_manager`
2. Or just watch socat output (socket doesn't need ADAS running to test receiver)

---

## Running Tests (No Hardware Required)

Validate logic locally:

```bash
python3 src/mobility_scenarios_src/emergency_priority/test_ble_receiver.py -v
```

Expected: All tests pass (state parsing, timeout logic, config validation, etc.)

---

## File Structure

```
src/mobility_scenarios_src/emergency_priority/
├── ble_trafficlight_receiver.py         ← Main BLE receiver (Python)
├── MAKECODE_BLE_FIRMWARE_GUIDE.md       ← Firmware templates
├── config_ble.json                      ← BLE configuration
├── test_ble_receiver.py                 ← Unit tests
├── trafficlight_vehicle_bridge.py       ← Original USB-direct bridge (reference)
└── microbit_trafficlight_firmware.py    ← Original MicroPython (reference)
```

---

## Timeline for Friday

- **Today (Tue):** Setup + Template A flash + BLE receiver test
- **Wed:** Template B firmware upgrade + full state machine test
- **Thu:** ADAS Manager integration + vehicle throttle validation
- **Fri:** Demo ready (BLE working end-to-end OR fallback to USB-direct)

---

## Next Steps

1. **Immediate:** Flash Template A to micro:bit
2. **Then:** Run BLE receiver on AGL
3. **Validate:** Watch state changes in socat
4. **Troubleshoot:** Use `--verbose` to debug any issues
5. **Upgrade:** Move to Template B once Template A works

---

## Safety Notes

- BLE receiver always defaults to RED on startup
- Any timeout (>5s) forces RED automatically
- Yellow state escalates to RED after 2s (no indefinite "slow down")
- If receiver crashes, vehicle stops (safe fail)
- USB-direct path (validated) remains as fallback for Friday

---

## Reference Files

- [BLE Receiver Source](ble_trafficlight_receiver.py)
- [MakeCode Firmware Guide](MAKECODE_BLE_FIRMWARE_GUIDE.md)
- [Config Template](config_ble.json)
- [Master Planning Docs](../../docs/guides/mobility_scenarios/planning_V2I.md#path-b-implementation-checklist-ble-direct)
