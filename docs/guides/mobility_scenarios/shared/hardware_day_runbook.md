# Hardware Day Runbook (Monday)

## Objective

Switch from local placeholders to real barrier + traffic-light control with minimum risk.

## 1) Preflight (before wiring)

Run from repo root:

```bash
bash src/mobility_scenarios_src/run_preflight.sh
```

Expected:
- Syntax checks pass.
- Rule validation pass.
- Unified local demo reports `PASS`.

## 2) Barrier bring-up (Kitronik)

Code file:
- `src/mobility_scenarios_src/v2i/kitronik_barrier.py`

Config:
- `src/mobility_scenarios_src/v2i/config.json`
- set `barrier.backend` to `hardware`

Steps:
1. Confirm power, GND, and safe servo range.
2. Implement real `open`/`close` in `KitronikBarrierAdapter.handle_request`.
3. Run dry test (wheels lifted):
   - `open` -> barrier opens
   - `close` -> barrier closes
4. Verify fallback: unknown/timeout -> closed.

## 3) Traffic-light bring-up

Code file:
- `src/mobility_scenarios_src/emergency_priority/trafficlight_simulator.py`

Steps:
1. Replace placeholder with real GPIO path (red/yellow/green).
2. Validate transitions:
   - `green` -> advance
   - `yellow` -> slow_down
   - `red` -> stop
   - `emergency_on` -> emergency_green
   - `emergency_off` -> red

Bridge vehicle motion from real light state:

```bash
python3 src/mobility_scenarios_src/emergency_priority/trafficlight_vehicle_bridge.py \
   --config src/mobility_scenarios_src/emergency_priority/config.json
```

Autonomous driving integration path:
- Bridge writes traffic-light sign class into ADAS Manager object socket:
   - `/tmp/adas_objects.sock`
- ADAS Manager then enforces longitudinal behavior:
   - red -> stop
   - yellow -> slow down (and bridge can escalate to stop after timeout)
   - green -> continue

## 4) Unified integration test

Run scenario:
1. Normal mode -> red + closed
2. Emergency mode -> emergency_green + open
3. Back to normal -> red + closed

Capture:
- terminal logs
- short hardware video
- deviations/fallback notes

## 5) Safety rollback

On any failure:
- barrier -> `closed`
- traffic light -> `red`
- vehicle motion command -> `stop`
