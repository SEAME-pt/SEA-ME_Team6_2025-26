#!/usr/bin/env python3
"""Serial smoke test for the barrier micro:bit firmware.

Usage:
  python3 src/mobility_scenarios_src/v2i/serial_barrier_smoke_test.py \
    --port /dev/ttyACM1 --baud 115200
"""

from __future__ import annotations

import argparse
import time

import serial


def send_and_read(ser: serial.Serial, command: str, read_window_s: float = 1.5) -> list[str]:
    ser.reset_input_buffer()
    ser.write((command + "\r\n").encode("utf-8"))
    ser.flush()

    lines: list[str] = []
    deadline = time.time() + read_window_s
    while time.time() < deadline:
        raw = ser.readline()
        if not raw:
            continue
        line = raw.decode("utf-8", errors="ignore").strip()
        if line:
            lines.append(line)
    return lines


def main() -> int:
    parser = argparse.ArgumentParser(description="Barrier micro:bit serial smoke test")
    parser.add_argument("--port", default="/dev/ttyACM1", help="Serial device path")
    parser.add_argument("--baud", type=int, default=115200, help="Baud rate")
    args = parser.parse_args()

    print(f"[INFO] Opening serial {args.port} @ {args.baud}")
    with serial.Serial(args.port, args.baud, timeout=0.2) as ser:
        time.sleep(2.0)
        for cmd in ["PING", "STATUS", "OPEN", "STATUS", "CLOSE", "STATUS"]:
            print(f"\n[TX] {cmd}")
            replies = send_and_read(ser, cmd)
            if replies:
                for line in replies:
                    print(f"[RX] {line}")
            else:
                print("[RX] <no response>")

    print("\n[DONE] Barrier serial smoke test finished.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
