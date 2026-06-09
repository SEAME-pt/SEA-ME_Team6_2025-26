# Mobility Scenarios Source (Module 3)

Implementation code for Module 3: V2I barriers, emergency priority traffic lights,
and unified coordination.

## Quick Start

```bash
cd src/mobility_scenarios_src
bash run_preflight.sh
```

## Structure

- `v2i/`: Phase 1 barrier communication and backend adapters.
- `emergency_priority/`: Phase 2-3 traffic-light and emergency orchestration.
- `shared/`: shared utilities and common references.
- `run_preflight.sh`: local readiness validation.

## Main Entry Points

- `v2i/barrier_simulator.py`
- `v2i/v2i_client.py`
- `emergency_priority/trafficlight_simulator.py`
- `emergency_priority/trafficlight_vehicle_bridge.py`
- `emergency_priority/unified_demo.py`

## Hardware Integration Path

- Approved immediate path: **USB-direct** micro:bit -> AGL (`/dev/ttyACM1`) -> ADAS socket.
- Wireless fallback path: host-bridge transmitter/receiver over network.

## Documentation

Single source of truth:
- `docs/guides/mobility_scenarios/planning_V2I.md`

## Integration Context

- Vehicle architecture: STM32 -> CAN -> Pi5/AGL -> KUKSA/VSS -> Qt cluster.
- MVP is simulation-first, then hardware bring-up.
- Safety fallback defaults are stop-oriented (`red` / `closed` / throttle `0`).
