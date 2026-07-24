#!/usr/bin/env bash
set -euo pipefail

cd /data/ADAS-Manager-Modular10

export PYTHONUNBUFFERED="${PYTHONUNBUFFERED:-1}"

STOPPING=0

cleanup() {
    # During intentional stop, some children may already be gone.
    jobs -p | xargs -r kill 2>/dev/null || true
    wait || true
}
on_term() {
    echo "[stack:emergency] stop requested"
    /usr/bin/python3 - <<'PY'
import os
import socket
import time

sock_path = "/tmp/adas_emergency.sock"
msg = b"0\n"

for _ in range(30):
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
    /usr/bin/python3 - <<'PY'
import time

import serial

port = "/dev/serial/by-id/usb-Arm_BBC_micro:bit_CMSIS-DAP_9906360200052820bfc3e066718d1667000000006e052820-if01"
cmds = ["TL RED", "BAR CLOSE", "LGT OFF"]

try:
    ser = serial.Serial(port, 115200, timeout=0.2)
    time.sleep(0.5)
    for cmd in cmds:
        print(f"[stack:emergency] safe-state -> {cmd}", flush=True)
        ser.write((cmd + "\r\n").encode("utf-8"))
        ser.flush()
        deadline = time.time() + 0.25
        while time.time() < deadline:
            raw = ser.readline()
            if not raw:
                continue
            line = raw.decode("utf-8", errors="ignore").strip()
            if line:
                print(f"[stack:emergency] gateway: {line}", flush=True)
    ser.close()
except Exception as exc:
    print(f"[stack:emergency] safe-state serial error: {exc}", flush=True)
PY
    sleep 3
    STOPPING=1
}

trap on_term INT TERM
trap cleanup EXIT

./bin/adas_manager &
ADAS_PID=$!

# If ADAS manager fails early (lock/CAN/etc), abort this whole stack.
sleep 1
if ! kill -0 "$ADAS_PID" 2>/dev/null; then
    echo "[stack:emergency] adas_manager exited during startup"
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

/usr/bin/python3 src/mobility_scenarios_src/emergency_priority/roadside_emergency_controller.py \
    --gateway-port /dev/serial/by-id/usb-Arm_BBC_micro:bit_CMSIS-DAP_9906360200052820bfc3e066718d1667000000006e052820-if01 \
    --adas-v2i-socket /tmp/adas_v2i.sock \
    --adas-emergency-socket /tmp/adas_emergency.sock &
EMERG_PID=$!

# Mirror manual roadside toggle (t): set emergency ON once roadside socket is up.
/usr/bin/python3 - <<'PY'
import os
import socket
import time

sock_path = "/tmp/adas_emergency.sock"
msg = b"1\n"

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

# Keep service bound to all three processes; if any exits, fail and restart.
while true; do
    if [[ "$STOPPING" -eq 1 ]]; then
        echo "[stack:emergency] stop requested"
        /usr/bin/python3 - <<'PY'
import os
import socket
import time

sock_path = "/tmp/adas_emergency.sock"
msg = b"0\n"

for _ in range(20):
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
        sleep 2
        exit 0
    fi

    if ! kill -0 "$ADAS_PID" 2>/dev/null; then
        echo "[stack:emergency] adas_manager exited"
        exit 1
    fi
    if ! kill -0 "$JOY_PID" 2>/dev/null; then
        echo "[stack:emergency] joystick_control exited"
        exit 1
    fi
    if ! kill -0 "$EMERG_PID" 2>/dev/null; then
        echo "[stack:emergency] roadside_emergency_controller exited"
        exit 1
    fi
    sleep 1
done
