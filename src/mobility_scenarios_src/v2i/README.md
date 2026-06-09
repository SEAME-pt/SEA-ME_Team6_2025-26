# V2I - Barrier Communication (Phase 1) — Technical Setup

## Quick Start (Local MVP)

```bash
cd /path/to/src/mobility_scenarios_src/v2i
pip install -r requirements.txt

# Terminal 1: start barrier simulator (local mode)
python barrier_simulator.py --config config.json

# Terminal 2: start vehicle client
python v2i_client.py --config config.json
```

## Dependencies

- `pytest` — testing
- `kuksa-client` — optional for real KUKSA publishing integration

## Project Layout

```text
src/mobility_scenarios_src/v2i/
├── README.md
├── config.json
├── requirements.txt
├── barrier_backend.py
├── barrier_rules.py
├── barrier_simulator.py
├── kitronik_barrier.py
├── v2i_client.py
└── tests/
    ├── test_barrier_backend.py
    ├── test_barrier_rules.py
    ├── test_barrier_simulator.py
    └── test_v2i_client.py
```

## Vehicle Motion Rule (Barrier Use Case)

- Barrier `open` -> `advance`
- Barrier `closed` -> `stop`
- Barrier `unknown/timeout` -> `stop` (safety fallback)

## Running Tests

```bash
pytest tests/ -v
```

## Extensions (Hardware Day)

To adapt for real Kitronik barrier:

1. Set `barrier.backend = "hardware"` in `config.json`.
2. Replace placeholder logic in `kitronik_barrier.py` with real GPIO/serial control.
3. Add required hardware libs (`RPi.GPIO`, `pyserial`, or board-specific library).
4. Keep the same `handle_request(payload)` contract so `v2i_client.py` stays unchanged.

---

**Design & Rationale**: `docs/guides/mobility_scenarios/planning_V2I.md`
