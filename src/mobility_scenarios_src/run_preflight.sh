#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(cd "${ROOT_DIR}/../.." && pwd)"

echo "[Preflight] Repo: ${REPO_DIR}"

cd "${REPO_DIR}"

echo "[Preflight] Syntax checks"
python3 -m py_compile \
  src/mobility_scenarios_src/v2i/barrier_backend.py \
  src/mobility_scenarios_src/v2i/barrier_rules.py \
  src/mobility_scenarios_src/v2i/barrier_simulator.py \
  src/mobility_scenarios_src/v2i/kitronik_barrier.py \
  src/mobility_scenarios_src/v2i/v2i_client.py \
  src/mobility_scenarios_src/emergency_priority/traffic_light_rules.py \
  src/mobility_scenarios_src/emergency_priority/trafficlight_simulator.py \
  src/mobility_scenarios_src/emergency_priority/emergency_client.py \
  src/mobility_scenarios_src/emergency_priority/coordinator.py \
  src/mobility_scenarios_src/emergency_priority/unified_demo.py

echo "[Preflight] Rule validation"
python3 - <<'PY'
import sys
sys.path.insert(0, 'src/mobility_scenarios_src/v2i')
sys.path.insert(0, 'src/mobility_scenarios_src/emergency_priority')

from barrier_rules import decide_motion_from_barrier_state
from traffic_light_rules import decide_motion_from_traffic_light_state

assert decide_motion_from_barrier_state('open') == 'advance'
assert decide_motion_from_barrier_state('closed') == 'stop'
assert decide_motion_from_traffic_light_state('green') == 'advance'
assert decide_motion_from_traffic_light_state('yellow') == 'slow_down'
assert decide_motion_from_traffic_light_state('red') == 'stop'
print('Rules: PASS')
PY

echo "[Preflight] Unified demo"
python3 src/mobility_scenarios_src/emergency_priority/unified_demo.py --config src/mobility_scenarios_src/emergency_priority/config.json

echo "[Preflight] DONE"
