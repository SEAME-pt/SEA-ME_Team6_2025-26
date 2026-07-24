#!/usr/bin/env bash
set -euo pipefail

cd /data/ADAS-Manager-Modular10

export PYTHONUNBUFFERED="${PYTHONUNBUFFERED:-1}"

STOPPING=0

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

sleep 1
if ! kill -0 "$ADAS_PID" 2>/dev/null; then
    echo "[stack:normal] adas_manager exited during startup"
    exit 1
fi

/usr/bin/python3 joystick_control.py <<'EOF' &
1
EOF
JOY_PID=$!

# Force autonomous mode once the joystick socket is up.
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

while true; do
    if [[ "$STOPPING" -eq 1 ]]; then
        echo "[stack:normal] stop requested"
        exit 0
    fi

    if ! kill -0 "$ADAS_PID" 2>/dev/null; then
        echo "[stack:normal] adas_manager exited"
        exit 1
    fi
    if ! kill -0 "$JOY_PID" 2>/dev/null; then
        echo "[stack:normal] joystick_control exited"
        exit 1
    fi
    sleep 1
done
