#!/usr/bin/env python3
"""Serial smoke test for micro:bit traffic light integration.

Usage:
  python3 src/mobility_scenarios_src/emergency_priority/serial_smoke_test.py \
    --port /dev/ttyACM0 --baud 115200
"""

from __future__ import annotations

import argparse
import time

import serial


def _normalize_echo(line: str) -> str:
    return line.replace("\\r", "").replace("\\n", "").strip()


def send_and_read(ser: serial.Serial, command: str, read_window_s: float = 2.5) -> list[str]:
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
            # Ignore command echo so we keep only real responses.
            if _normalize_echo(line) == command:
                continue
            lines.append(line)
    return lines


def reset_and_boot(ser: serial.Serial) -> list[str]:
    """Reset board from REPL and capture initial boot lines."""
    ser.reset_input_buffer()
    ser.write(b"\x03\x03")
    ser.flush()
    time.sleep(0.15)
    ser.write(b"\x04")
    ser.flush()

    lines: list[str] = []
    deadline = time.time() + 2.0
    while time.time() < deadline:
        raw = ser.readline()
        if not raw:
            continue
        line = raw.decode("utf-8", errors="ignore").strip()
        if line:
            lines.append(line)
    return lines


def main() -> int:
    parser = argparse.ArgumentParser(description="Micro:bit serial smoke test")
    parser.add_argument("--port", default="/dev/ttyACM0", help="Serial device path")
    parser.add_argument("--baud", type=int, default=115200, help="Baud rate")
    args = parser.parse_args()

    print(f"[INFO] Opening serial {args.port} @ {args.baud}")
    with serial.Serial(args.port, args.baud, timeout=0.2) as ser:
        # micro:bit often needs a short settle time after opening serial.
        time.sleep(2.0)

        boot_lines = reset_and_boot(ser)
        if boot_lines:
            print("[BOOT]")
            for line in boot_lines:
                print(f"[RX] {line}")

        for cmd in ["PING", "RED", "YELLOW", "GREEN", "STATUS"]:
            print(f"\\n[TX] {cmd}")
            replies = send_and_read(ser, cmd)
            if replies:
                for line in replies:
                    print(f"[RX] {line}")
            else:
                print("[RX] <no response>")

    print("\\n[DONE] Serial smoke test finished.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
