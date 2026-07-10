#!/usr/bin/env python3
"""Interactive serial test for micro:bit traffic lights.

This script sends one command at a time and waits for user confirmation,
so you can visually confirm each color on hardware.

Note: some traffic light boards are active-low, others are active-high.
Use --mode LOW or --mode HIGH to match your board polarity.
"""

from __future__ import annotations

import argparse
import time

import serial


def read_all_lines(ser: serial.Serial, duration_s: float = 1.5) -> list[str]:
    lines: list[str] = []
    deadline = time.time() + duration_s
    while time.time() < deadline:
        raw = ser.readline()
        if not raw:
            continue
        line = raw.decode("utf-8", errors="ignore").strip()
        if line:
            lines.append(line)
    return lines


def send_command(ser: serial.Serial, cmd: str) -> None:
    ser.write((cmd + "\r\n").encode("utf-8"))
    ser.flush()


def main() -> int:
    parser = argparse.ArgumentParser(description="Step-by-step serial traffic light test")
    parser.add_argument("--port", default="/dev/ttyACM0", help="Serial device path")
    parser.add_argument("--baud", type=int, default=115200, help="Baud rate")
    parser.add_argument("--mode", choices=["LOW", "HIGH"], default="HIGH", help="Output polarity mode")
    args = parser.parse_args()

    print(f"[INFO] Opening {args.port} @ {args.baud}")
    with serial.Serial(args.port, args.baud, timeout=0.2) as ser:
        time.sleep(2.0)

        # Clean reset and capture boot lines once.
        ser.write(b"\x03\x03")
        ser.flush()
        time.sleep(0.1)
        ser.write(b"\x04")
        ser.flush()
        print("\n[BOOT]")
        for line in read_all_lines(ser, duration_s=2.0):
            print(f"[RX] {line}")

        print(f"\n[INFO] Testing in MODE {args.mode}")
        print("[INFO] If colors look inverted, rerun with the other mode.")

        steps = [
            (f"MODE {args.mode}", f"Set MODE {args.mode}"),
            ("RED", "Set logical state RED"),
            ("YELLOW", "Set logical state YELLOW"),
            ("GREEN", "Set logical state GREEN"),
            ("RED", "Set logical state RED again"),
        ]

        for cmd, note in steps:
            input(f"\nPress Enter to send `{cmd}` ({note})...")
            print(f"[TX] {cmd}")
            send_command(ser, cmd)
            for line in read_all_lines(ser, duration_s=1.2):
                print(f"[RX] {line}")

        print("\n[DONE] Step-by-step test complete.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
