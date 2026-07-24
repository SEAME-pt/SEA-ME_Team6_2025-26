# Emergency Priority (Phase 2 + 3) — Technical Setup

## Quick Start

```bash
cd /path/to/src/mobility_scenarios_src/emergency_priority
pip install -r requirements.txt

# Terminal 1: Smoke test traffic light simulator
python trafficlight_simulator.py --config config.json

# Terminal 2: Run emergency client (local mode)
python emergency_client.py --config config.json

# Terminal 3: Run unified demo (traffic light + barrier + emergency override)
python unified_demo.py --config config.json

# Terminal 4: Real micro:bit -> vehicle motion bridge (stop/slow/advance)
python trafficlight_vehicle_bridge.py --config config.json
```

For autonomous driving integration, this bridge sends traffic-light objects to
ADAS Manager via UNIX socket `/tmp/adas_objects.sock`.

- `red` -> `SIGN_TL_RED` (ADAS throttle limit: stop)
- `yellow` -> `SIGN_TL_YELLOW` (ADAS throttle limit: slow)
- `yellow` persisting longer than `yellow_stop_after_s` -> forced `SIGN_TL_RED`
- `green`/`emergency_green` -> `SIGN_TL_GREEN` (continue)

### Integration with KUKSA/VSS and Qt Cluster

The bridge also publishes to KUKSA Databroker for the instrument cluster:

```
Semaforo State → Bridge → KUKSA Databroker (Vehicle.Infra.TrafficLight.Status)
                       └→ ADAS Manager socket (/tmp/adas_objects.sock)
                       └→ Qt Cluster display
```

This allows:
- **Cluster**: Display current traffic-light color in real-time
- **ADAS Manager**: Apply throttle limits without additional integration
- **Vehicle Motion**: Automatic stop/slow/continue based on light color

### Wireless Deployment (Remote Testing)

**For testing without physical cables between micro:bit and AGL**:

1. **On PC** (with micro:bit over serial):
   ```bash
   python trafficlight_vehicle_bridge.py --bridge-mode transmitter \
     --agl-host <AGL-IP> \
     --agl-port 5555 \
     --config config.json
   ```

2. **On AGL** (receives and bridges to KUKSA + ADAS Manager):
   ```bash
  python trafficlight_vehicle_bridge.py --bridge-mode receiver \
     --config config.json
   ```

**Key properties**:
- No changes needed to ADAS Manager, KUKSA Databroker, or Qt Cluster
- Bridge automatically publishes traffic light state to both KUKSA/VSS (cluster display) and ADAS Manager socket (car throttle)
- Safety: defaults to `red` if wireless connection fails
- Heartbeat: receiver triggers safe stop if no update for 5 seconds

**Update `config.json` with wireless settings**:
```json
{
  "adas_bridge": {
    "enabled": true,
    "object_socket": "/tmp/adas_objects.sock",
    "yellow_stop_after_s": 2.0
  },
  "wireless": {
    "mode": "receiver",
    "listen_port": 5555,
    "heartbeat_timeout_sec": 5.0
  },
  "kuksa": {
    "enabled": true,
    "signal_path": "Vehicle.Infra.TrafficLight.Status"
  }
}
```

**Read more**: Full architecture, deployment steps, and troubleshooting are consolidated in:
`docs/guides/mobility_scenarios/planning_V2I.md`

## Real Hardware Note

After flashing, the traffic light starts in safe mode (`RED`).
This is expected behavior.

## Dependencies

- `kuksa-client` (optional in local MVP)
- `pytest`

## Project Layout

```
src/mobility_scenarios_src/emergency_priority/
├── README.md
├── requirements.txt
├── config.json
├── trafficlight_simulator.py    (Phase 2 — traffic light service)
├── emergency_client.py          (Vehicle-side emergency trigger)
├── coordinator.py               (Phase 3 — unified orchestrator)
├── unified_demo.py              (Deterministic local demo evidence)
├── microbit_trafficlight_firmware.py (Flashed to micro:bit)
├── trafficlight_vehicle_bridge.py (Real traffic light -> vehicle command)
├── serial_step_by_step.py       (Manual hardware color validation)
├── serial_smoke_test.py         (Serial protocol smoke test)
└── tests/
    ├── test_trafficlight_simulator.py
    ├── test_emergency_client.py
    └── test_coordinator.py
```

## Configuration

Use `config.json` for timeout, trigger timings, source, signal paths, serial bridge settings, and ADAS socket bridge settings.

## Running Tests

```bash
pytest tests/ -v
```

## Extensions

### Real Traffic Light Hardware (Phase 2b)

Replace `TrafficLightHardwarePlaceholder` logic in `trafficlight_simulator.py` with real GPIO logic:

```python
# trafficlight_hardware.py
import RPi.GPIO as GPIO

class TrafficLightHardware:
    def __init__(self, red_pin=17, green_pin=27):
        self.red_pin = red_pin
        self.green_pin = green_pin
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup([self.red_pin, self.green_pin], GPIO.OUT)
    
    def set_green(self):
        GPIO.output(self.red_pin, GPIO.LOW)
        GPIO.output(self.green_pin, GPIO.HIGH)
        self.state = "green"
        self._publish_status()
    
    def set_red(self):
        GPIO.output(self.green_pin, GPIO.LOW)
        GPIO.output(self.red_pin, GPIO.HIGH)
        self.state = "red"
        self._publish_status()
```

---

**Design & Rationale**: `docs/guides/mobility_scenarios/planning_V2I.md`
