#!/usr/bin/env python3
"""Host-side controller for the full emergency roadside scenario.

This script is the practical bridge between the tested Coordinator policy and
real roadside devices connected over serial.

It does three things at the same time:
- toggles the scenario state from the keyboard;
- commands the real roadside devices (traffic light, barrier, street lights);
- injects the resulting V2I frame into ADAS Manager.

Hardware topology
-----------------
Pi (AGL) ──USB serial──► micro:bit B (gateway)  [microbit_gateway_makecode.js]
                                                            │  radio group 23
                                                 micro:bit A (semáforo)  [microbit_trafficlight_tx_makecode.js]
                                                 micro:bit C (cancela)    [microbit_barrier_radio_makecode.js]
                                                 micro:bit D (candeeiro)  [microbit_streetlight_radio_makecode.js]

The Pi talks only to the gateway micro:bit over USB serial.
The gateway relays commands over radio to all roadside micro:bits.

Default port mapping:
    --gateway-port  /dev/ttyACM1  → micro:bit B gateway (adjust if needed)

Serial protocol to gateway:
    TL RED/YELLOW/GREEN
    BAR OPEN/CLOSE
    LGT ON/OFF/BLINK
    STATUS

Gateway replies with ACK lines plus the latest TL/BAR/LGT states.

Keyboard controls:
  t -> toggle vehicle mode NORMAL/EMERGENCY
  a -> toggle approaching flag
  l -> toggle same-lane flag
  r/y/g -> force traffic-light state for normal-mode tests
  o/m/c -> force barrier state for normal-mode tests
  q -> quit

Notes:
- This script provides the software-side emergency toggle now.
- The physical joystick button for emergency mode does not exist yet; David will
  wire that into ADAS Manager later.
"""

from __future__ import annotations

import argparse
import os
import select
import socket
import struct
import sys
import termios
import time
import tty
from dataclasses import dataclass

import serial

from coordinator import Coordinator, RoadScenarioInput, VehicleMode


@dataclass
class RuntimeState:
    vehicle_mode: VehicleMode = VehicleMode.NORMAL
    approaching: bool = True
    same_lane: bool = True
    traffic_light_state: str = "red"
    barrier_state: str = "closed"
    streetlight_state: str = "off"


class SerialCommandDevice:
    def __init__(self, port: str, baud: int, name: str, startup_delay_s: float = 2.0):
        self.port = port
        self.baud = baud
        self.name = name
        self.startup_delay_s = startup_delay_s
        self.ser: serial.Serial | None = None
        self._last_by_prefix: dict[str, str] = {}

    @staticmethod
    def _is_gateway_reply(line: str) -> bool:
        return any(
            token in line
            for token in (
                "GW_",
                "TL_STATE",
                "BAR_STATE",
                "LGT_STATE",
                "ACK",
                "STATUS",
                "PONG",
            )
        )

    @staticmethod
    def _is_non_gateway_telemetry(line: str) -> bool:
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

    def connect(self) -> None:
        if self.ser is not None and self.ser.is_open:
            return
        self.ser = serial.Serial(self.port, self.baud, timeout=0.2)
        time.sleep(self.startup_delay_s)
        self._drain()
        print(f"[{self.name}] Connected on {self.port} @ {self.baud}")

    def close(self) -> None:
        if self.ser is None:
            return
        try:
            if self.ser.is_open:
                self.ser.close()
        finally:
            self.ser = None
            self._last_by_prefix.clear()

    def _drain(self, duration_s: float = 0.3) -> list[str]:
        if self.ser is None:
            return []
        out: list[str] = []
        deadline = time.time() + duration_s
        while time.time() < deadline:
            raw = self.ser.readline()
            if not raw:
                continue
            line = raw.decode("utf-8", errors="ignore").strip()
            if line:
                out.append(line)
        return out

    def verify_gateway(self) -> None:
        replies = self.send("STATUS", read_window_s=0.8, dedupe=False)
        if not replies:
            raise RuntimeError(
                f"No reply from gateway on {self.port}. Check micro:bit gateway USB mapping."
            )
        if any(self._is_non_gateway_telemetry(line) for line in replies):
            sample = " | ".join(replies[:3])
            raise RuntimeError(
                "Serial port looks like ADAS telemetry, not micro:bit gateway "
                f"({self.port}). Sample: {sample}"
            )
        if not any(self._is_gateway_reply(line) for line in replies):
            sample = " | ".join(replies[:3])
            raise RuntimeError(
                "Unexpected serial replies while probing gateway on "
                f"{self.port}. Sample: {sample}"
            )

    def send(self, command: str, read_window_s: float = 0.8, dedupe: bool = True) -> list[str]:
        self.connect()
        assert self.ser is not None
        prefix = command.split(" ", 1)[0]
        if dedupe and self._last_by_prefix.get(prefix) == command:
            return []
        self.ser.reset_input_buffer()
        self.ser.write((command + "\r\n").encode("utf-8"))
        self.ser.flush()
        replies = self._drain(read_window_s)
        self._last_by_prefix[prefix] = command
        if replies:
            print(f"[{self.name}] {command} -> {' | '.join(replies)}")
        else:
            print(f"[{self.name}] {command} -> <no reply>")
        return replies


class AdasV2ISender:
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
        if tl_state in {"green", "emergency_green"}:
            return AdasV2ISender.TL_GREEN
        return AdasV2ISender.TL_UNKNOWN

    @staticmethod
    def _map_barrier_state(barrier_state: str) -> int:
        if barrier_state == "open":
            return AdasV2ISender.BARRIER_OPEN
        if barrier_state == "closed":
            return AdasV2ISender.BARRIER_CLOSED
        if barrier_state in {"mid", "moving"}:
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


class EmergencyToggleReceiver:
    def __init__(self, socket_path: str):
        self.socket_path = socket_path
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
        self.sock.setblocking(False)

        try:
            os.unlink(self.socket_path)
        except FileNotFoundError:
            pass

        self.sock.bind(self.socket_path)

    def close(self) -> None:
        try:
            self.sock.close()
        finally:
            try:
                os.unlink(self.socket_path)
            except FileNotFoundError:
                pass

    def poll(self) -> VehicleMode | None:
        latest: VehicleMode | None = None
        while True:
            try:
                payload = self.sock.recv(16)
            except BlockingIOError:
                break

            value = payload.decode("utf-8", errors="ignore").strip()
            if value == "1":
                latest = VehicleMode.EMERGENCY
            elif value == "0":
                latest = VehicleMode.NORMAL

        return latest


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Emergency roadside device controller")
    parser.add_argument("--gateway-port", default="/dev/ttyACM1", help="Gateway micro:bit serial port")
    parser.add_argument("--baud", type=int, default=115200, help="Serial baud rate")
    parser.add_argument("--adas-v2i-socket", default="/tmp/adas_v2i.sock", help="ADAS V2I unix datagram socket")
    parser.add_argument(
        "--adas-emergency-socket",
        default="/tmp/adas_emergency.sock",
        help="ADAS emergency toggle unix datagram socket",
    )
    parser.add_argument("--loop-s", type=float, default=0.1, help="Main loop interval")
    return parser.parse_args()


def print_help() -> None:
    print("\n=== Emergency Roadside Controller ===")
    print("t: toggle NORMAL/EMERGENCY")
    print("a: toggle approaching")
    print("l: toggle same-lane")
    print("r/y/g: traffic light red/yellow/green")
    print("o/m/c: barrier open/mid/closed")
    print("s: streetlight on/off/blink cycle")
    print("q: quit")


def apply_roadside_outputs(
    gateway: SerialCommandDevice,
    traffic_light_state: str,
    barrier_action: str,
    street_lights_action: str,
    priority_active: bool,
) -> None:
    if priority_active:
        gateway.send("TL GREEN")
    else:
        tl_command = str(traffic_light_state or "red").upper()
        if tl_command not in {"RED", "YELLOW", "GREEN"}:
            tl_command = "RED"
        gateway.send(f"TL {tl_command}")

    if barrier_action == "open":
        gateway.send("BAR OPEN")
    elif barrier_action == "close" or barrier_action == "closed":
        gateway.send("BAR CLOSE")

    if street_lights_action == "blink":
        gateway.send("LGT BLINK")
    else:
        gateway.send("LGT OFF")


def force_safe_state(gateway: SerialCommandDevice, v2i: AdasV2ISender) -> None:
    print("[Controller] Safe-state -> TL RED", flush=True)
    gateway.send("TL RED", dedupe=False)
    print("[Controller] Safe-state -> BAR CLOSE", flush=True)
    gateway.send("BAR CLOSE", dedupe=False)
    print("[Controller] Safe-state -> LGT OFF", flush=True)
    gateway.send("LGT OFF", dedupe=False)
    print("[Controller] Safe-state -> V2I red/closed/priority=0", flush=True)
    v2i.send(
        traffic_light_state="red",
        barrier_state="closed",
        priority_active=False,
    )


def main() -> int:
    args = parse_args()
    state = RuntimeState()
    coordinator = Coordinator(traffic_light_service=None, barrier_service=None)

    gateway = SerialCommandDevice(args.gateway_port, args.baud, "Gateway")
    v2i = AdasV2ISender(args.adas_v2i_socket)
    emergency_rx = EmergencyToggleReceiver(args.adas_emergency_socket)

    print_help()

    try:
        gateway.verify_gateway()
    except Exception as exc:
        print(f"[Controller] Gateway probe failed: {exc}")
        gateway.close()
        v2i.close()
        return 2

    interactive_tty = sys.stdin.isatty()
    fd = None
    old_settings = None
    if interactive_tty:
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        tty.setcbreak(fd)
    else:
        print("[Controller] Non-interactive mode: keyboard toggles disabled")

    last_summary = ""

    try:
        while True:
            if interactive_tty:
                dr, _, _ = select.select([sys.stdin], [], [], 0.0)
                if dr:
                    ch = sys.stdin.read(1)
                    if ch == "q":
                        print("\n[Controller] Exit requested")
                        return 0
                    if ch == "t":
                        state.vehicle_mode = VehicleMode.EMERGENCY if state.vehicle_mode == VehicleMode.NORMAL else VehicleMode.NORMAL
                    elif ch == "a":
                        state.approaching = not state.approaching
                    elif ch == "l":
                        state.same_lane = not state.same_lane
                    elif ch == "r":
                        state.traffic_light_state = "red"
                    elif ch == "y":
                        state.traffic_light_state = "yellow"
                    elif ch == "g":
                        state.traffic_light_state = "green"
                    elif ch == "o":
                        state.barrier_state = "open"
                    elif ch == "m":
                        state.barrier_state = "mid"
                    elif ch == "c":
                        state.barrier_state = "closed"
                    elif ch == "s":
                        state.streetlight_state = "blink" if state.streetlight_state != "blink" else "off"

            socket_mode = emergency_rx.poll()
            if socket_mode is not None and socket_mode != state.vehicle_mode:
                state.vehicle_mode = socket_mode
                print(f"\n[Controller] ADAS emergency socket -> {state.vehicle_mode.name}")
                if socket_mode == VehicleMode.NORMAL:
                    # Apply safe defaults immediately on emergency OFF, not only on process exit.
                    force_safe_state(gateway, v2i)
                    state.traffic_light_state = "red"
                    state.barrier_state = "closed"
                    state.streetlight_state = "off"

            result = coordinator.resolve_road_scenario(
                RoadScenarioInput(
                    vehicle_mode=state.vehicle_mode,
                    approaching=state.approaching,
                    same_lane=state.same_lane,
                    traffic_light_state=state.traffic_light_state,
                    barrier_state=state.barrier_state,
                )
            )

            try:
                apply_roadside_outputs(
                    gateway=gateway,
                    traffic_light_state=state.traffic_light_state,
                    barrier_action=result.barrier_action,
                    street_lights_action=result.street_lights_action if result.street_lights_action else state.streetlight_state,
                    priority_active=result.priority_active,
                )
            except Exception as exc:
                print(f"\n[Controller] Roadside output error: {exc}")

            try:
                v2i.send(
                    traffic_light_state="green" if result.priority_active else state.traffic_light_state,
                    barrier_state=result.barrier_action,
                    priority_active=result.priority_active,
                )
            except Exception as exc:
                print(f"\n[Controller] ADAS V2I send error: {exc}")

            summary = (
                f"mode={state.vehicle_mode.name} app={state.approaching} lane={state.same_lane} "
                f"tl={state.traffic_light_state} barrier={state.barrier_state} "
                f"priority={result.priority_active} tl_action={result.nearest_traffic_light_action} "
                f"barrier_action={result.barrier_action} street_lights={result.street_lights_action} "
                f"siren={result.ambulance_siren_action}"
            )
            if summary != last_summary:
                print(f"\r[Controller] {summary}", end="", flush=True)
                last_summary = summary

            time.sleep(args.loop_s)
    finally:
        # On shutdown, force roadside back to safe NORMAL defaults.
        try:
            print("\n[Controller] Shutdown safe-state begin", flush=True)
            force_safe_state(gateway, v2i)
            print("[Controller] Shutdown safe-state done", flush=True)
        except Exception as exc:
            print(f"\n[Controller] Shutdown safe-state error: {exc}")
        if interactive_tty and fd is not None and old_settings is not None:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        emergency_rx.close()
        gateway.close()
        v2i.close()


if __name__ == "__main__":
    raise SystemExit(main())
