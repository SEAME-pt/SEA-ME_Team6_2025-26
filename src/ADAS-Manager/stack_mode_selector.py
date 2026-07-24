#!/usr/bin/env python3
"""Interactive stack selector for ADAS modes.

Keys:
  N -> NORMAL stack
  V -> V2I stack
  E -> EMERGENCY stack
  S -> STOP all stacks
  Q -> Quit selector
"""

from __future__ import annotations

import os
import subprocess
import sys
import termios
import tty
from pathlib import Path

ROOT = Path("/data/ADAS-Manager-Modular10")
SWITCH = ROOT / "stack-switch.sh"


def run_mode(mode: str) -> None:
    cmd = [str(SWITCH), mode]
    print(f"\n[selector] -> {mode.upper()}")
    proc = subprocess.run(cmd, cwd=str(ROOT), text=True, capture_output=True)
    if proc.stdout:
        print(proc.stdout.strip())
    if proc.returncode != 0:
        err = (proc.stderr or "").strip()
        print(f"[selector] command failed ({proc.returncode}) {err}")


def main() -> int:
    if not SWITCH.exists():
        print(f"Missing switch script: {SWITCH}")
        return 1

    print("=== ADAS Stack Selector ===")
    print("N: NORMAL   V: V2I   E: EMERGENCY   S: STOP   Q: QUIT")

    fd = sys.stdin.fileno()
    old = termios.tcgetattr(fd)
    tty.setcbreak(fd)
    try:
        while True:
            ch = sys.stdin.read(1)
            if not ch:
                continue
            key = ch.lower()
            if key == "n":
                run_mode("normal")
            elif key == "v":
                run_mode("v2i")
            elif key == "e":
                run_mode("emergency")
            elif key == "s":
                run_mode("stop")
            elif key == "q":
                print("\n[selector] bye")
                return 0
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old)


if __name__ == "__main__":
    raise SystemExit(main())
