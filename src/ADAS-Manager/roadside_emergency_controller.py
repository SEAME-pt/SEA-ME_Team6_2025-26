#!/usr/bin/env python3
"""Host-side controller for the full emergency roadside scenario.

This script is the practical bridge between the tested Coordinator policy and
real roadside devices connected over serial.

It does four things at the same time:
- toggles the scenario state from the keyboard, or from the physical
  emergency-priority button on the remote (relayed by ADAS Manager over
  /tmp/adas_emergency.sock);
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

The physical emergency-priority button (joystick_control.py, key 'e' in
interactive mode) has the same effect as pressing 't' here — both flip
state.vehicle_mode. See ADAS-Manager/emergency_sender.hpp on the C++ side.
"""

from __future__ import annotations

import argparse
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

    def send(self, command: str, read_window_s: float = 0.8) -> list[str]:
        self.connect()
        assert self.ser is not None
        self.ser.reset_input_buffer()
        self.ser.write((command + "\r\n").encode("utf-8"))
        self.ser.flush()
        replies = self._drain(read_window_s)
        if replies:
            print(f"[{self.name}] {command} -> {' | '.join(replies)}")
        else:
            print(f"[{self.name}] {command} -> <no reply>")
        return replies

    def send_nowait(self, command: str) -> None:
        """Write only, no reply wait. Use when several commands must go out
        back-to-back (TL/BAR/LGT) — send()'s per-call read_window_s (0.8s
        default) serialises them, giving a visible cascade instead of all
        three devices reacting together."""
        self.connect()
        assert self.ser is not None
        self.ser.write((command + "\r\n").encode("utf-8"))
        self.ser.flush()


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


class AdasEmergencyReceiver:
    """Listens for the physical emergency-toggle button, relayed by ADAS
    Manager over /tmp/adas_emergency.sock ("1\\n" = ON, "0\\n" = OFF).

    ADAS Manager is the sender only (no bind) — this class owns the socket
    file, mirroring how ADAS Manager itself binds adas_v2i.sock/adas_lane.sock
    for its own inbound sockets.
    """

    def __init__(self, socket_path: str):
        self.socket_path = socket_path
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
        try:
            import os
            if os.path.exists(socket_path):
                os.unlink(socket_path)
        except OSError:
            pass
        self.sock.bind(socket_path)
        self.sock.setblocking(False)

    def close(self) -> None:
        self.sock.close()

    def poll(self) -> bool | None:
        """Drains all pending datagrams, returns the latest state or None
        if nothing new arrived this call."""
        latest: bool | None = None
        while True:
            try:
                data = self.sock.recv(8)
            except BlockingIOError:
                break
            if not data:
                continue
            latest = data.strip() == b"1"
        return latest


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Emergency roadside device controller")
    parser.add_argument("--gateway-port", default="/dev/ttyACM1", help="Gateway micro:bit serial port")
    parser.add_argument("--baud", type=int, default=115200, help="Serial baud rate")
    parser.add_argument("--adas-v2i-socket", default="/tmp/adas_v2i.sock", help="ADAS V2I unix datagram socket")
    parser.add_argument("--adas-emergency-socket", default="/tmp/adas_emergency.sock",
                         help="ADAS Manager emergency-toggle unix datagram socket")
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
    # send_nowait: as 3 ordens saem uma a seguir à outra sem esperar ACK —
    # com send() (0.8s de espera por resposta cada) o semáforo mudava,
    # só depois a cancela, só depois as luzes, em cascata visível.
    if priority_active:
        gateway.send_nowait("TL GREEN")
    else:
        tl_command = str(traffic_light_state or "red").upper()
        if tl_command not in {"RED", "YELLOW", "GREEN"}:
            tl_command = "RED"
        gateway.send_nowait(f"TL {tl_command}")

    if barrier_action == "open":
        gateway.send_nowait("BAR OPEN")
    elif barrier_action == "close" or barrier_action == "closed":
        gateway.send_nowait("BAR CLOSE")

    if street_lights_action == "blink":
        gateway.send_nowait("LGT BLINK")
    else:
        gateway.send_nowait("LGT OFF")


def main() -> int:
    args = parse_args()
    state = RuntimeState()
    coordinator = Coordinator(traffic_light_service=None, barrier_service=None)

    gateway = SerialCommandDevice(args.gateway_port, args.baud, "Gateway")
    v2i = AdasV2ISender(args.adas_v2i_socket)
    emergency_rx = AdasEmergencyReceiver(args.adas_emergency_socket)

    print_help()

    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    tty.setcbreak(fd)

    last_summary = ""

    try:
        while True:
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

            # Botão físico no comando, via ADAS Manager — mesmo efeito da tecla 't'.
            emergency_on = emergency_rx.poll()
            if emergency_on is not None:
                state.vehicle_mode = VehicleMode.EMERGENCY if emergency_on else VehicleMode.NORMAL

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
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        gateway.close()
        v2i.close()
        emergency_rx.close()


if __name__ == "__main__":
    raise SystemExit(main())
