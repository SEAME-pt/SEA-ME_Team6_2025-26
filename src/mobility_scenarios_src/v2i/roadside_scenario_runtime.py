#!/usr/bin/env python3
"""Real-time roadside scenario runtime (normal + emergency).

This runner combines:
- live traffic-light input from micro:bit serial gateway (R/Y/G)
- keyboard toggles for vehicle mode and barrier state
- coordinator policy resolution for normal/emergency scenarios
- native V2I injection to /tmp/adas_v2i.sock
- optional legacy ADAS object injection to /tmp/adas_objects.sock

Keyboard controls:
  t -> toggle vehicle mode NORMAL/EMERGENCY
  a -> toggle approaching flag
  l -> toggle same-lane flag
  o -> barrier open
  m -> barrier mid
  c -> barrier closed
  q -> quit
"""

from __future__ import annotations

import argparse
import re
import select
import serial
import socket
import struct
import sys
import termios
import time
import tty
from dataclasses import dataclass

from coordinator import Coordinator, RoadScenarioInput, VehicleMode

SIGN_TL_GREEN = 8
SIGN_TL_RED = 9
SIGN_TL_YELLOW = 10
MAX_OBJECTS = 4


@dataclass
class RuntimeState:
    vehicle_mode: VehicleMode = VehicleMode.NORMAL
    approaching: bool = True
    same_lane: bool = True
    barrier_state: str = "open"
    traffic_light_state: str = "red"


class AdasObjectSender:
    def __init__(self, socket_path: str):
        self.socket_path = socket_path
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)

    def close(self) -> None:
        self.sock.close()

    def send_sign(self, class_id: int, confidence: float = 1.0, distance_m: float = 2.0, theta_deg: float = 0.0) -> None:
        payload = struct.pack("<B", 1)
        payload += struct.pack("<Bfff", int(class_id), float(confidence), float(distance_m), float(theta_deg))
        for _ in range(MAX_OBJECTS - 1):
            payload += struct.pack("<Bfff", 0, 0.0, 0.0, 0.0)
        self.sock.sendto(payload, self.socket_path)


class AdasV2ISender:
    # Must match V2I enums in src/ADAS-Manager/socket_receiver.hpp
    TL_UNKNOWN = 0
    TL_RED = 1
    TL_YELLOW = 2
    TL_GREEN = 3

    BARRIER_UNKNOWN = 0
    BARRIER_OPEN = 1
    BARRIER_CLOSED = 2
    BARRIER_MOVING = 3

    def __init__(self, socket_path: str):
        self.socket_path = socket_path
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)

    def close(self) -> None:
        self.sock.close()

    @staticmethod
    def _map_tl_state(tl_state: str) -> int:
        if tl_state == "red":
            return AdasV2ISender.TL_RED
        if tl_state == "yellow":
            return AdasV2ISender.TL_YELLOW
        if tl_state == "green":
            return AdasV2ISender.TL_GREEN
        return AdasV2ISender.TL_UNKNOWN

    @staticmethod
    def _map_barrier_state(barrier_state: str) -> int:
        if barrier_state == "open":
            return AdasV2ISender.BARRIER_OPEN
        if barrier_state == "closed":
            return AdasV2ISender.BARRIER_CLOSED
        if barrier_state == "mid":
            return AdasV2ISender.BARRIER_MOVING
        return AdasV2ISender.BARRIER_UNKNOWN

    def send(self, traffic_light_state: str, barrier_state: str, priority_active: bool) -> None:
        payload = struct.pack(
            "<BBBB",
            self._map_tl_state(traffic_light_state),
            self._map_barrier_state(barrier_state),
            1 if priority_active else 0,
            0,
        )
        self.sock.sendto(payload, self.socket_path)


TL_TOKEN = re.compile(r"(?:TL_STATE\s*:\s*)?(RED|YELLOW|GREEN|R|Y|G)\b", re.IGNORECASE)


def parse_light_line(line: str) -> tuple[str, bool]:
    """Parse gateway line and return (state, ambiguous).

    If multiple colors are present in one line, choose the safest result:
    red > yellow > green.
    """
    v = (line or "").strip().upper()
    if not v:
        return "unknown", False

    found: set[str] = set()
    for token in TL_TOKEN.findall(v):
        t = token.upper()
        if t in {"R", "RED"}:
            found.add("red")
        elif t in {"Y", "YELLOW"}:
            found.add("yellow")
        elif t in {"G", "GREEN"}:
            found.add("green")

    if not found:
        return "unknown", False

    ambiguous = len(found) > 1
    if "red" in found:
        return "red", ambiguous
    if "yellow" in found:
        return "yellow", ambiguous
    return "green", ambiguous


def looks_like_non_gateway_telemetry(line: str) -> bool:
    return any(
        token in line
        for token in (
            "[CC]",
            "[AEB]",
            "[IMU]",
            "[INA226]",
            "[Speedometer]",
            "[SRF08]",
        )
    )


def map_motion_to_sign_class(motion: str) -> int:
    if motion == "advance":
        return SIGN_TL_GREEN
    if motion == "slow_down":
        return SIGN_TL_YELLOW
    return SIGN_TL_RED


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(description="Run real-time roadside scenario runtime")
    p.add_argument("--port", default="/dev/ttyACM1", help="micro:bit serial gateway port")
    p.add_argument("--baud", type=int, default=115200, help="serial baud")
    p.add_argument("--adas-v2i-socket", default="/tmp/adas_v2i.sock", help="ADAS V2I unix datagram socket")
    p.add_argument(
        "--legacy-adas-socket",
        default="",
        help="optional legacy ObjectFrame socket path (disabled by default)",
    )
    p.add_argument("--loop-s", type=float, default=0.05, help="main loop interval")
    p.add_argument(
        "--yellow-stop-after-s",
        type=float,
        default=2.0,
        help="seconds to keep slow_down on yellow before forcing stop",
    )
    p.add_argument(
        "--tl-stale-stop-s",
        type=float,
        default=2.5,
        help="if no valid TL update for this long, force TL=red (fail-safe)",
    )
    p.add_argument(
        "--tl-stable-samples",
        type=int,
        default=2,
        help="number of consecutive equal TL samples required before state change",
    )
    return p.parse_args()


def open_serial(port: str, baud: int) -> serial.Serial:
    ser = serial.Serial(port, baud, timeout=0.02)
    time.sleep(0.2)
    return ser


def print_help() -> None:
    print("\n=== Roadside Scenario Runtime ===")
    print("t: toggle NORMAL/EMERGENCY")
    print("a: toggle approaching")
    print("l: toggle same-lane")
    print("o/m/c: barrier open/mid/closed")
    print("q: quit")


def main() -> int:
    args = parse_args()
    state = RuntimeState()
    coordinator = Coordinator(traffic_light_service=None, barrier_service=None)
    v2i = AdasV2ISender(args.adas_v2i_socket)
    adas_legacy = AdasObjectSender(args.legacy_adas_socket) if args.legacy_adas_socket else None

    ser = None
    try:
        ser = open_serial(args.port, args.baud)
        print(f"[Runtime] Serial opened on {args.port} @ {args.baud}")
    except Exception as e:
        print(f"[Runtime] Serial open failed ({e}). Will retry in loop.")

    print_help()

    interactive_tty = sys.stdin.isatty()
    fd = None
    old_settings = None
    if interactive_tty:
        try:
            fd = sys.stdin.fileno()
            old_settings = termios.tcgetattr(fd)
            tty.setcbreak(fd)
        except termios.error as exc:
            interactive_tty = False
            fd = None
            old_settings = None
            print(f"[Runtime] TTY init failed ({exc}) -> keyboard toggles disabled")
    else:
        print("[Runtime] Non-interactive mode: keyboard toggles disabled")

    last_line = ""
    last_summary = ""
    yellow_since: float | None = None
    warned_wrong_port = False
    warned_ambiguous = False
    last_tl_update_ts = time.time()
    candidate_tl_state = state.traffic_light_state
    candidate_tl_count = 0

    try:
        while True:
            if interactive_tty:
                dr, _, _ = select.select([sys.stdin], [], [], 0.0)
                if dr:
                    ch = sys.stdin.read(1)
                    if ch == "q":
                        print("\n[Runtime] Exit requested")
                        return 0
                    if ch == "t":
                        state.vehicle_mode = VehicleMode.EMERGENCY if state.vehicle_mode == VehicleMode.NORMAL else VehicleMode.NORMAL
                    elif ch == "a":
                        state.approaching = not state.approaching
                    elif ch == "l":
                        state.same_lane = not state.same_lane
                    elif ch == "o":
                        state.barrier_state = "open"
                    elif ch == "m":
                        state.barrier_state = "mid"
                    elif ch == "c":
                        state.barrier_state = "closed"

            try:
                if ser is None:
                    ser = open_serial(args.port, args.baud)
                    print(f"\n[Runtime] Serial reopened on {args.port}")
                raw = ser.readline().decode("utf-8", errors="ignore").strip()
                if raw:
                    if looks_like_non_gateway_telemetry(raw) and not warned_wrong_port:
                        print(
                            f"\n[Runtime] Warning: serial on {args.port} looks like ADAS telemetry, "
                            "not micro:bit gateway light stream (R/Y/G)."
                        )
                        warned_wrong_port = True
                    parsed, ambiguous = parse_light_line(raw)
                    if ambiguous and not warned_ambiguous:
                        print(
                            f"\n[Runtime] Warning: ambiguous TL line '{raw}' -> forcing safest color RED"
                        )
                        warned_ambiguous = True
                    if parsed != "unknown":
                        last_tl_update_ts = time.time()
                        if parsed == candidate_tl_state:
                            candidate_tl_count += 1
                        else:
                            candidate_tl_state = parsed
                            candidate_tl_count = 1

                        # Debounce TL state transitions to avoid stop-go flicker.
                        if (
                            candidate_tl_count >= max(1, args.tl_stable_samples)
                            and state.traffic_light_state != candidate_tl_state
                        ):
                            state.traffic_light_state = candidate_tl_state
                            last_line = raw
            except serial.SerialException as e:
                print(f"\n[Runtime] Serial drop ({e}) -> reconnecting")
                try:
                    if ser and ser.is_open:
                        ser.close()
                except Exception:
                    pass
                ser = None
                time.sleep(0.2)

            # If gateway stops producing valid TL states, fail safe to red.
            if (time.time() - last_tl_update_ts) > args.tl_stale_stop_s:
                if state.traffic_light_state != "red":
                    print("\n[Runtime] TL stale -> forcing RED fail-safe")
                state.traffic_light_state = "red"
                candidate_tl_state = "red"
                candidate_tl_count = max(1, args.tl_stable_samples)

            result = coordinator.resolve_road_scenario(
                RoadScenarioInput(
                    vehicle_mode=state.vehicle_mode,
                    approaching=state.approaching,
                    same_lane=state.same_lane,
                    traffic_light_state=state.traffic_light_state,
                    barrier_state=state.barrier_state,
                )
            )

            yellow_elapsed_s = 0.0
            if state.vehicle_mode == VehicleMode.NORMAL and state.traffic_light_state == "yellow":
                if yellow_since is None:
                    yellow_since = time.time()
                yellow_elapsed_s = time.time() - yellow_since
                if yellow_elapsed_s >= args.yellow_stop_after_s:
                    result.vehicle_motion = "stop"
            else:
                yellow_since = None

            sign_class = map_motion_to_sign_class(result.vehicle_motion)
            try:
                v2i.send(
                    traffic_light_state=state.traffic_light_state,
                    barrier_state=state.barrier_state,
                    priority_active=result.priority_active,
                )
            except Exception as e:
                print(f"\n[Runtime] ADAS V2I send error: {e}")

            if adas_legacy is not None:
                try:
                    adas_legacy.send_sign(sign_class)
                except Exception as e:
                    print(f"\n[Runtime] ADAS legacy ObjectFrame send error: {e}")

            summary = (
                f"mode={state.vehicle_mode.name} app={state.approaching} lane={state.same_lane} "
                f"tl={state.traffic_light_state}({last_line or '-'}) barrier={state.barrier_state} "
                f"motion={result.vehicle_motion} sign={sign_class} "
                f"yellow_elapsed={yellow_elapsed_s:.1f}s "
                f"priority={result.priority_active} tl_action={result.nearest_traffic_light_action} "
                f"other_tls={result.other_traffic_lights_action} lights={result.street_lights_action} siren={result.ambulance_siren_action}"
            )
            if summary != last_summary:
                print(f"\r[Runtime] {summary}", end="", flush=True)
                last_summary = summary

            time.sleep(args.loop_s)
    finally:
        if interactive_tty and fd is not None and old_settings is not None:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        if ser is not None:
            try:
                ser.close()
            except Exception:
                pass
        v2i.close()
        if adas_legacy is not None:
            adas_legacy.close()


if __name__ == "__main__":
    raise SystemExit(main())