# Wireless Traffic Light Integration for Module 3

## Overview

This document describes how to deploy the traffic-light control system **without physical cables** between the micro:bit and the autonomous vehicle (AGL).

The solution reutilizes existing components (ADAS Manager, KUKSA/VSS, Qt Cluster) and adds a **lightweight wireless bridge** to transmit traffic-light state over the network.

Important: the current micro:bit firmware does **not** send traffic-light state over wireless by itself. Today it exposes state over USB serial, and a host process forwards that state over the network. A direct micro:bit wireless design would require different firmware and a matching receiver on AGL.

## Architecture

### Component Layout

```
┌─────────────────────────────────────┐
│  Your PC (Windows/Mac/Linux)        │
│  ┌──────────────────────────────┐   │
│  │ Micro:bit + Kitronik Light   │   │
│  │ (Serial over USB)            │   │
│  └────────────┬─────────────────┘   │
│               │ (read serial state)  │
│               ▼                      │
│  ┌──────────────────────────────┐   │
│  │ Transmitter Bridge           │   │
│  │ (trafficlight_vehicle_bridge │   │
│  │  --bridge-mode transmitter)  │   │
│  └────────┬──────────────────────┘   │
│           │ UDP/TCP packet           │
└───────────┼──────────────────────────┘
            │  light_state: {red|yellow|green}
            │  (WiFi/Ethernet)
            ▼
┌─────────────────────────────────────┐
│  AGL / Raspberry Pi 5               │
│                                     │
│  ┌──────────────────────────────┐   │
│  │ Receiver Bridge              │   │
│  │ (trafficlight_vehicle_bridge │   │
│  │  --bridge-mode receiver)     │   │
│  └────┬─────────────────┬────────┘   │
│       │                 │            │
│       ▼ (publish)       ▼ (inject)  │
│  ┌─────────────┐   ┌──────────────┐ │
│  │ KUKSA       │   │ /tmp/adas    │ │
│  │ Databroker  │   │ _objects.sock│ │
│  │             │   │ (ObjectFrame)│ │
│  └──────┬──────┘   └──────┬───────┘ │
│         │                 │         │
│         │ Vehicle.Infra   │ reads   │
│         │ .TrafficLight   │         │
│         │ .Status         ▼         │
│         │           ┌─────────────┐ │
│         │           │ ADAS Manager│ │
│         │           │ (adas_mgr)  │ │
│         │           └──────┬──────┘ │
│         │                  │        │
│         │                  ▼        │
│         │           ┌─────────────┐ │
│         │           │ CAN Sender  │ │
│         │           │ (STM32 ctrl)│ │
│         │           └─────────────┘ │
│         │                  │        │
│         └────┬─────────────┘        │
│              │                      │
│              ▼ (via CAN Bus)        │
│         ┌──────────────┐            │
│         │ Motor/Brake  │            │
│         │ (STM32)      │            │
│         └──────────────┘            │
│                                     │
└─────────────────────────────────────┘
         ▲
         │ Qt Cluster (via KUKSA)
         │
    Display Screen
```

## Data Flow

### 1. Micro:bit Exposes Traffic Light State

- Firmware continuously reads LED pins (`pin0=red`, `pin1=yellow`, `pin2=green`)
- Returns state via serial as: `ACK STATE red|yellow|green MODE HIGH`
- The wireless hop is performed by the host bridge, not by the micro:bit itself

### 2. Host Transmitter Bridge (PC Side)

```
micro:bit → exposes state on USB serial
host PC → reads serial from micro:bit
  → parses state (red / yellow / green)
  → creates UDP packet: {"state": "red", "timestamp": 12345}
  → sends to AGL at port 5555
```

**Command**:
```bash
python src/mobility_scenarios_src/emergency_priority/trafficlight_vehicle_bridge.py \
  --bridge-mode transmitter \
  --agl-host 10.21.220.191 \
  --agl-port 5555 \
  --config config.json
```

### 3. Receiver Bridge (AGL Side)

```
AGL → listens on UDP port 5555
    → receives packet from PC
    → validates state and timestamp
    → publishes to KUKSA Databroker:
         Signal: Vehicle.Infra.TrafficLight.Status
         Value: "red" | "yellow" | "green"
    → injects ObjectFrame into /tmp/adas_objects.sock:
         SIGN_TL_RED (9)    if state == "red"
         SIGN_TL_YELLOW (10) if state == "yellow"
         SIGN_TL_GREEN (8)   if state == "green"
```

**Command**:
```bash
ssh root@10.21.220.191
cd /data/ADAS-Manager-OA-v7
python3 /path/to/trafficlight_vehicle_bridge.py \
  --bridge-mode receiver \
  --config config.json
```

Or as a systemd service:
```bash
sudo systemctl start adas-trafficlight-bridge.service
```

### 4. ADAS Manager Consumes Traffic Light

The `adas_manager` process is already running on AGL (current validated path: `/data/ADAS-Manager-OA-v7/adas_manager`).

It:
- Listens on `/tmp/adas_objects.sock`
- Reads `ObjectFrame` with traffic-light sign class
- Applies throttle override:
  ```cpp
  if (o.class_id == SIGN_STOP || o.class_id == SIGN_TL_RED)    return 0;     // throttle = 0 (stop)
  if (o.class_id == SIGN_YIELD || o.class_id == SIGN_TL_YELLOW) return 50;   // throttle = 50 (slow)
  // else throttle remains 100 (continue)
  ```
- Sends CAN control command to STM32
- Vehicle stops, slows, or continues accordingly

### 5. Qt Cluster Displays State

The instrument cluster application is already running on AGL (via KUKSA subscription).

It:
- Subscribes to `Vehicle.Infra.TrafficLight.Status` (via KUKSA Databroker)
- Updates UI in real-time to show:
  - 🔴 Red (stop)
  - 🟡 Yellow (slow down)
  - 🟢 Green (continue)

**No code changes** to the cluster are needed.

## Configuration

Create or update `config.json` on both PC and AGL:

### Transmitter (PC) Config

```json
{
  "traffic_light_serial": {
    "port": "/dev/ttyACM0",
    "baud": 115200,
    "mode": "HIGH",
    "poll_interval_s": 0.5
  },
  "wireless": {
    "mode": "transmitter",
    "agl_host": "10.21.220.191",
    "agl_port": 5555,
    "heartbeat_interval_s": 1.0,
    "timeout_s": 5.0
  }
}
```

### Receiver (AGL) Config

```json
{
  "wireless": {
    "mode": "receiver",
    "listen_host": "0.0.0.0",
    "listen_port": 5555,
    "heartbeat_timeout_s": 5.0
  },
  "adas_bridge": {
    "enabled": true,
    "object_socket": "/tmp/adas_objects.sock",
    "yellow_stop_after_s": 2.0
  },
  "kuksa": {
    "enabled": true,
    "databroker_host": "localhost",
    "databroker_port": 55555,
    "signal_path": "Vehicle.Infra.TrafficLight.Status"
  }
}
```

## Deployment Checklist

### Prerequisites

- [ ] Micro:bit is flashed with traffic-light firmware (`microbit_trafficlight_firmware.py`)
- [ ] Micro:bit is connected to PC via USB and appears as `/dev/ttyACM0`
- [ ] AGL (Pi5) is running with:
  - [ ] KUKSA Databroker running (`pgrep -af kuksa_databroker`)
  - [ ] ADAS Manager running (`pgrep -af adas_manager`)
  - [ ] Qt Cluster running (if display is needed)
- [ ] Network connectivity exists between PC and AGL (WiFi or Ethernet)

### Setup Steps

#### 1. On PC (Transmitter)

```bash
# Navigate to project
cd ~/SEA-ME_Team6_2025-26

# Install dependencies
pip install -r src/mobility_scenarios_src/emergency_priority/requirements.txt

# Test micro:bit serial connection first
python src/mobility_scenarios_src/emergency_priority/serial_step_by_step.py \
  --port /dev/ttyACM0 \
  --baud 115200 \
  --mode HIGH

# Once colors confirmed, run transmitter
python src/mobility_scenarios_src/emergency_priority/trafficlight_vehicle_bridge.py \
  --bridge-mode transmitter \
  --agl-host 10.21.220.191 \
  --agl-port 5555 \
  --config src/mobility_scenarios_src/emergency_priority/config.json
```

**Output should show**:
```
[Bridge] Running in transmitter mode. Sending to 10.21.220.191:5555
[Bridge] light_state=red -> vehicle_motion=stop -> adas_sign_class=9
...
```

#### 2. On AGL (Receiver)

```bash
# SSH to AGL
ssh root@10.21.220.191

# Navigate to ADAS Manager directory
cd /data/ADAS-Manager-OA-v7

# Copy bridge script (or ensure it's in Python path)
# Assuming it's already copied or symlinked

# Run receiver
python3 trafficlight_vehicle_bridge.py \
  --bridge-mode receiver \
  --config /data/ADAS-Manager-OA-v7/config.json

# Or run as background service
nohup python3 trafficlight_vehicle_bridge.py \
  --bridge-mode receiver \
  --config /data/ADAS-Manager-OA-v7/config.json > bridge.log 2>&1 &
```

**Output should show**:
```
[Bridge] Running in receiver mode. Listening on port 5555
[Bridge] Received light_state=red. Publishing to KUKSA and ADAS socket.
...
```

#### 3. Verify Integration

**Check KUKSA signal**:
```bash
# On AGL, in another terminal
python3 -c "import grpcio; from kuksa.val.v2 import *; ..."
# Or use kuksa-client CLI
```

**Check ADAS Manager logs**:
```bash
# On AGL
tail -f /tmp/adas_manager.log  # if logging is enabled
# Or watch for CAN messages
```

**Test vehicle motion**:
1. Set traffic light to **RED** → vehicle should stop
2. Set traffic light to **YELLOW** → vehicle should slow down
3. Set traffic light to **GREEN** → vehicle should continue

## Safety & Fallback Behavior

### Wireless Link Failure

If the transmitter crashes or the network is down:

1. **Receiver**: Starts a heartbeat timer (default 5 seconds)
2. **After timeout**: Defaults traffic light to **RED** (safe stop)
3. **ADAS Manager**: Reads the stop signal and commands motor stop
4. **Recovery**: When transmitter resumes, receiver re-syncs

### Yellow Timeout Escalation

Yellow is a transient state. If not cleared within `yellow_stop_after_s` (default 2.0 seconds):

- Bridge auto-escalates to RED
- Vehicle slows, then stops
- This prevents indefinite "prepare to stop" behavior

### Heartbeat Monitoring

Both transmitter and receiver include heartbeat/watchdog:

- **Transmitter**: Confirms serial connection to micro:bit every 1 second
- **Receiver**: Confirms network packet arrival every 5 seconds
- **Failed heartbeat**: Triggers safe fallback (RED)

## Troubleshooting

### Bridge won't start

**Error**: `Address already in use` on port 5555

**Solution**:
```bash
# Kill existing process
pkill -f "trafficlight_vehicle_bridge"
# Wait 5 seconds, then retry
sleep 5
python3 trafficlight_vehicle_bridge.py --bridge-mode receiver --config config.json
```

### ADAS Manager doesn't see traffic light

**Check**:
```bash
# Verify socket exists
ls -la /tmp/adas_objects.sock

# Verify ADAS Manager is running
pgrep -af adas_manager

# Check bridge is sending to socket
grep "ADAS socket send" /tmp/bridge.log  # or wherever logs are
```

### Qt Cluster doesn't display traffic light

**Check**:
```bash
# Verify KUKSA Databroker is running
pgrep -af kuksa_databroker

# Check signal is being published
# (Use KUKSA client tools to query Vehicle.Infra.TrafficLight.Status)
```

## Future Enhancements

1. **Redundancy**: Run multiple receivers with failover.
2. **Persistence**: Log all state transitions for debugging.
3. **Remote control**: Add web UI to manually toggle light for testing.
4. **Encryption**: Add TLS/mTLS for secure wireless communication.
5. **Multi-light**: Support multiple traffic lights in different zones.

---

**Last Updated**: 2026-06-09  
**Owner**: Joao  
**Status**: Ready for hardware day integration
