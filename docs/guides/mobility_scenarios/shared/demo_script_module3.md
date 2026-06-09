# Module 3 Demo Script (Local + Hardware Day)

## Goal

Demonstrate end-to-end behavior for:
- V2I barrier flow
- Emergency Priority (traffic lights)
- Unified override behavior

## A. Local Demo (no hardware required)

### 1. Prepare environment

```bash
cd src/mobility_scenarios_src/emergency_priority
python3 -m py_compile trafficlight_simulator.py emergency_client.py coordinator.py unified_demo.py
```

Alternative (single command from repo root):

```bash
bash src/mobility_scenarios_src/run_preflight.sh
```

### 2. Run deterministic unified demo

```bash
python3 unified_demo.py --config config.json
```

Expected result:
- Step 1 normal: red + closed
- Step 2 emergency: emergency_green + open
- Step 3 back to normal: red + closed
- Final line: `PASS`

## B. Hardware Day Checklist (Monday)

### Barrier (Kitronik)

- [ ] Confirm power and common GND with controller.
- [ ] Confirm control signal interface (GPIO/serial/I2C as applicable).
- [ ] Confirm safe servo range and neutral position.
- [ ] Replace placeholder logic in `src/mobility_scenarios_src/v2i/kitronik_barrier.py`.
- [ ] Set `barrier.backend = "hardware"` in `src/mobility_scenarios_src/v2i/config.json`.
- [ ] Run open/close dry test with wheels lifted / safe stand.

### Traffic light hardware

- [ ] Confirm pin map for red/green channels.
- [ ] Validate voltage/current requirements and resistor setup.
- [ ] Replace placeholder logic in `trafficlight_simulator.py` (hardware path).
- [ ] Validate emergency_on -> green and emergency_off -> red.

## C. Acceptance Evidence to collect

- [x] Terminal logs showing each phase transitions.
	- Saved at `docs/guides/mobility_scenarios/shared/evidence/local_demo_2026-06-03.log`
- [x] Rules validation logs for independent use cases.
	- Saved at `docs/guides/mobility_scenarios/shared/evidence/rules_validation_2026-06-03.log`
- [x] Final local preflight log.
	- Saved at `docs/guides/mobility_scenarios/shared/evidence/final_preflight_2026-06-03.log`
- [ ] Short video of local demo (`PASS`).
- [ ] Short video of hardware open/close and emergency priority flow.
- [ ] Note any deviations and fallback behavior.

Suggested command to capture local terminal demo video (run manually):

```bash
# Example using OBS or equivalent screen recorder:
# 1) Start recording
python3 src/mobility_scenarios_src/emergency_priority/unified_demo.py --config src/mobility_scenarios_src/emergency_priority/config.json
# 2) Stop recording and save as: docs/guides/mobility_scenarios/shared/evidence/local_demo_video_2026-06-03.mp4
```

## D. Safety fallback

- Barrier default: `closed` on timeout/failure.
- Traffic light default: `red` on timeout/failure.
- Emergency inactive must never force open/green.

## E. What Can Still Be Done Without Hardware

- Finalize and run all local tests (`py_compile` + unit tests + `unified_demo.py`).
- Validate independent use-case rules:
	- Barrier: `open -> advance`, `closed/unknown -> stop`.
	- Traffic light: `green -> advance`, `yellow -> slow_down`, `red -> stop`.
- Prepare commit split and PR description for Sprint 15 evidence.
- Prepare Monday hardware execution checklist and expected outputs.

## F. Hardware Handoff (Monday)

Detailed step-by-step runbook:

- `docs/guides/mobility_scenarios/shared/hardware_day_runbook.md`

### Inputs to confirm before wiring

- Barrier interface type: GPIO / serial / I2C.
- Traffic light pin map: red, yellow, green channels.
- Ground and power limits for actuator and lights.

### Code touchpoints

- Barrier hardware adapter:
	- `src/mobility_scenarios_src/v2i/kitronik_barrier.py`
	- `src/mobility_scenarios_src/v2i/config.json` -> set `barrier.backend = "hardware"`
- Traffic light hardware path:
	- `src/mobility_scenarios_src/emergency_priority/trafficlight_simulator.py`
	- `src/mobility_scenarios_src/emergency_priority/trafficlight_vehicle_bridge.py`

### Minimal validation sequence on hardware

1. Barrier closed -> vehicle command must be `stop`.
2. Barrier open -> vehicle command must be `advance`.
3. Traffic light green -> `advance`.
4. Traffic light yellow -> `slow_down`.
5. Traffic light red -> `stop`.
6. Emergency override -> `emergency_green` + barrier `open`.

### Safety rollback

- If any controller timeout/error occurs, force:
	- Barrier: `closed`
	- Traffic light: `red`
	- Vehicle command: `stop`
