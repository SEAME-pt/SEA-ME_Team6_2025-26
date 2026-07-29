#!/usr/bin/env bash
set -euo pipefail

ROOT="/data/ADAS-Manager-Modular11"
cd "$ROOT"

export PYTHONUNBUFFERED="${PYTHONUNBUFFERED:-1}"

STOPPING=0

require_file() {
    local p="$1"
    if [[ ! -e "$p" ]]; then
        echo "[stack:v2i-mod11] missing required file: $ROOT/$p"
        exit 2
    fi
}

require_file "bin/adas_manager"
require_file "joystick_control.py"
require_file "roadside_scenario_runtime.py"

cleanup() {
    jobs -p | xargs -r kill 2>/dev/null || true
    wait || true
}
on_term() {
    STOPPING=1
}

trap on_term INT TERM
trap cleanup EXIT

./bin/adas_manager &
ADAS_PID=$!

# If ADAS manager fails early (lock/CAN/etc), abort this whole stack.
sleep 1
if ! kill -0 "$ADAS_PID" 2>/dev/null; then
    echo "[stack:v2i-mod11] adas_manager exited during startup"
    exit 1
fi

/usr/bin/python3 joystick_control.py <<'EOF' &
1
EOF
JOY_PID=$!

# Mirror manual flow (mode 3 + U): force AUTONOMOUS once ADAS joystick socket is up.
/usr/bin/python3 - <<'PY'
import os
import socket
import time

sock_path = "/tmp/adas_joystick.sock"
msg = b"A\n"

for _ in range(80):
    if os.path.exists(sock_path):
        s = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
        try:
            s.sendto(msg, sock_path)
            break
        except OSError:
            pass
        finally:
            s.close()
    time.sleep(0.1)
PY

/usr/bin/python3 roadside_scenario_runtime.py \
    --port /dev/serial/by-id/usb-Arm_BBC_micro:bit_CMSIS-DAP_9906360200052820bfc3e066718d1667000000006e052820-if01 \
    --adas-v2i-socket /tmp/adas_v2i.sock &
V2I_PID=$!

# Keep service bound to all three processes; if any exits, fail and restart.
while true; do
    if [[ "$STOPPING" -eq 1 ]]; then
        echo "[stack:v2i-mod11] stop requested"
        exit 0
    fi

    if ! kill -0 "$ADAS_PID" 2>/dev/null; then
        echo "[stack:v2i-mod11] adas_manager exited"
        exit 1
    fi
    if ! kill -0 "$JOY_PID" 2>/dev/null; then
        echo "[stack:v2i-mod11] joystick_control exited"
        exit 1
    fi
    if ! kill -0 "$V2I_PID" 2>/dev/null; then
        echo "[stack:v2i-mod11] roadside_scenario_runtime exited"
        exit 1
    fi
    sleep 1
done
