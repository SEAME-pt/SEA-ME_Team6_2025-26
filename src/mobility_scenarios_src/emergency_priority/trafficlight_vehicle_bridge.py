#!/usr/bin/env python3
"""Bridge real micro:bit traffic-light state to vehicle motion command.

Supports two modes:
1. Local (serial): Reads from micro:bit serial -> ADAS socket
2. Wireless (UDP):
   - Transmitter (PC): Reads from micro:bit serial -> sends UDP to AGL
   - Receiver (AGL): Receives UDP -> ADAS socket + KUKSA Databroker
"""

from __future__ import annotations

import argparse
import json
import socket
import struct
import time
from dataclasses import dataclass
from typing import Optional

import serial

from traffic_light_rules import decide_motion_from_traffic_light_state


@dataclass
class BridgeConfig:
    port: str
    baud: int
    mode: str
    poll_interval_s: float
    adas_enabled: bool
    adas_socket: str
    yellow_stop_after_s: float
    wireless_mode: Optional[str] = None  # "transmitter" or "receiver"
    agl_host: Optional[str] = None
    agl_port: int = 5555
    listen_host: str = "0.0.0.0"
    listen_port: int = 5555
    heartbeat_timeout_s: float = 5.0


# Must match src/ADAS-Manager/socket_receiver.hpp SignClass enum.
SIGN_TL_GREEN = 8
SIGN_TL_RED = 9
SIGN_TL_YELLOW = 10
MAX_OBJECTS = 4


class MicrobitSerialClient:
    def __init__(self, port: str, baud: int, timeout_s: float = 0.25):
        self._ser = serial.Serial(port, baud, timeout=timeout_s)
        time.sleep(2.0)

    def close(self) -> None:
        self._ser.close()

    def soft_reset(self) -> list[str]:
        self._ser.write(b"\x03\x03")
        self._ser.flush()
        time.sleep(0.1)
        self._ser.write(b"\x04")
        self._ser.flush()
        return self._read_lines(2.0)

    def send(self, cmd: str, window_s: float = 1.0) -> list[str]:
        self._ser.write((cmd + "\r\n").encode("utf-8"))
        self._ser.flush()
        return self._read_lines(window_s)

    def _read_lines(self, window_s: float) -> list[str]:
        out: list[str] = []
        deadline = time.time() + window_s
        while time.time() < deadline:
            raw = self._ser.readline()
            if not raw:
                continue
            line = raw.decode("utf-8", errors="ignore").strip()
            if line:
                out.append(line)
        return out


def parse_state(lines: list[str]) -> str:
    """Extract traffic light state from serial response lines."""
    direct_map = {
        "R": "red",
        "Y": "yellow",
        "G": "green",
        "RED": "red",
        "YELLOW": "yellow",
        "GREEN": "green",
    }

    for line in reversed(lines):
        upper = line.upper()
        # Radio gateway path: one-state-per-line payload (R/Y/G or full names).
        if upper in direct_map:
            return direct_map[upper]
        if "ACK STATE " in upper:
            payload = upper.split("ACK STATE ", 1)[1].strip()
            return payload.split()[0].lower()
        if upper.startswith("STATE "):
            return upper.split("STATE ", 1)[1].split()[0].lower()
    return "unknown"


def load_config(config_path: str) -> BridgeConfig:
    """Load configuration from JSON file."""
    with open(config_path, "r", encoding="utf-8") as f:
        cfg = json.load(f)

    serial_cfg = cfg.get("traffic_light_serial", {})
    adas_cfg = cfg.get("adas_bridge", {})
    wireless_cfg = cfg.get("wireless", {})
    
    return BridgeConfig(
        port=str(serial_cfg.get("port", "/dev/ttyACM0")),
        baud=int(serial_cfg.get("baud", 115200)),
        mode=str(serial_cfg.get("mode", "HIGH")).upper(),
        poll_interval_s=float(serial_cfg.get("poll_interval_s", 0.5)),
        adas_enabled=bool(adas_cfg.get("enabled", True)),
        adas_socket=str(adas_cfg.get("object_socket", "/tmp/adas_objects.sock")),
        yellow_stop_after_s=float(adas_cfg.get("yellow_stop_after_s", 2.0)),
        wireless_mode=wireless_cfg.get("mode"),
        agl_host=wireless_cfg.get("agl_host"),
        agl_port=int(wireless_cfg.get("agl_port", 5555)),
        listen_host=wireless_cfg.get("listen_host", "0.0.0.0"),
        listen_port=int(wireless_cfg.get("listen_port", 5555)),
        heartbeat_timeout_s=float(wireless_cfg.get("heartbeat_timeout_s", 5.0)),
    )


class AdasObjectSender:
    """Send ObjectFrame to ADAS Manager via UNIX socket."""
    
    def __init__(self, socket_path: str):
        self.socket_path = socket_path
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)

    def close(self) -> None:
        self.sock.close()

    def send_sign(self, class_id: int, confidence: float = 1.0, distance_m: float = 0.2, theta_deg: float = 0.0) -> None:
        """Send ObjectFrame with traffic light sign class to ADAS socket."""
        payload = struct.pack("<B", 1)
        payload += struct.pack("<Bfff", int(class_id), float(confidence), float(distance_m), float(theta_deg))
        for _ in range(MAX_OBJECTS - 1):
            payload += struct.pack("<Bfff", 0, 0.0, 0.0, 0.0)
        self.sock.sendto(payload, self.socket_path)


class KuksaPublisher:
    """Publish traffic light state to KUKSA Databroker via gRPC (stub for now)."""
    
    def __init__(self, signal_path: str = "Vehicle.Infra.TrafficLight.Status"):
        self.signal_path = signal_path
        # TODO: Implement gRPC client when KUKSA Python SDK is available
        print(f"[KUKSA] Publisher initialized for signal: {self.signal_path}")

    def publish(self, state: str) -> None:
        """Publish traffic light state to KUKSA."""
        # For now, just print. In production, use grpcio to connect to KUKSA databroker.
        print(f"[KUKSA] Publishing {self.signal_path} = {state}")


def map_light_to_sign_class(light_state: str, yellow_elapsed_s: float, yellow_stop_after_s: float) -> int:
    """Map traffic light state to ADAS sign class with yellow timeout escalation."""
    state = (light_state or "unknown").lower()
    if state in ("green", "emergency_green"):
        return SIGN_TL_GREEN
    if state == "yellow":
        if yellow_elapsed_s >= yellow_stop_after_s:
            return SIGN_TL_RED
        return SIGN_TL_YELLOW
    return SIGN_TL_RED


def parse_args() -> argparse.Namespace:
    """Parse command-line arguments."""
    parser = argparse.ArgumentParser(description="Bridge traffic light state to vehicle motion")
    parser.add_argument("--config", default="config.json", help="Path to JSON config")
    parser.add_argument("--port", default=None, help="Override serial port")
    parser.add_argument("--baud", type=int, default=None, help="Override serial baudrate")
    parser.add_argument("--mode", choices=["LOW", "HIGH"], default=None, help="Override board mode")
    parser.add_argument("--bridge-mode", choices=["local", "transmitter", "receiver"], default=None,
                        help="Bridge mode: local (serial->ADAS), transmitter (serial->UDP), receiver (UDP->ADAS+KUKSA)")
    parser.add_argument("--agl-host", default=None, help="AGL host for transmitter mode")
    parser.add_argument("--agl-port", type=int, default=5555, help="AGL UDP port for transmitter")
    return parser.parse_args()


def main_local(bridge_cfg: BridgeConfig) -> int:
    """Run local mode: serial -> ADAS socket."""
    print("[Bridge] Running in LOCAL mode (serial -> ADAS socket)")
    
    client = MicrobitSerialClient(port=bridge_cfg.port, baud=bridge_cfg.baud)
    adas_sender = AdasObjectSender(bridge_cfg.adas_socket) if bridge_cfg.adas_enabled else None
    yellow_since: Optional[float] = None
    
    try:
        boot = client.soft_reset()
        print("[Bridge] Boot lines:", boot)

        mode_resp = client.send(f"MODE {bridge_cfg.mode}")
        print(f"[Bridge] MODE {bridge_cfg.mode} -> {mode_resp}")

        print("[Bridge] LOCAL mode running. Ctrl+C to stop.")
        while True:
            status_lines = client.send("STATUS", window_s=0.8)
            light_state = parse_state(status_lines)
            motion = decide_motion_from_traffic_light_state(light_state)

            now = time.time()
            if light_state == "yellow":
                if yellow_since is None:
                    yellow_since = now
                yellow_elapsed_s = now - yellow_since
            else:
                yellow_since = None
                yellow_elapsed_s = 0.0

            sign_class = map_light_to_sign_class(light_state, yellow_elapsed_s, bridge_cfg.yellow_stop_after_s)

            if adas_sender is not None:
                try:
                    adas_sender.send_sign(sign_class)
                except Exception as e:
                    print(f"[Bridge] ADAS socket send error: {e}")

            print(f"[Bridge] light_state={light_state} -> vehicle_motion={motion} -> adas_sign_class={sign_class}")
            time.sleep(bridge_cfg.poll_interval_s)
    except KeyboardInterrupt:
        print("[Bridge] Stopped by user")
        return 0
    finally:
        if adas_sender is not None:
            adas_sender.close()
        client.close()


def main_transmitter(bridge_cfg: BridgeConfig) -> int:
    """Run transmitter mode: serial -> UDP to AGL."""
    print(f"[Bridge] Running in TRANSMITTER mode. Sending to {bridge_cfg.agl_host}:{bridge_cfg.agl_port}")
    
    if not bridge_cfg.agl_host:
        print("[ERROR] --agl-host required for transmitter mode")
        return 1
    
    client = MicrobitSerialClient(port=bridge_cfg.port, baud=bridge_cfg.baud)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    try:
        boot = client.soft_reset()
        print("[Bridge] Boot lines:", boot)

        mode_resp = client.send(f"MODE {bridge_cfg.mode}")
        print(f"[Bridge] MODE {bridge_cfg.mode} -> {mode_resp}")

        print("[Bridge] TRANSMITTER mode running. Ctrl+C to stop.")
        while True:
            status_lines = client.send("STATUS", window_s=0.8)
            light_state = parse_state(status_lines)
            
            # Send UDP packet with traffic light state
            payload = json.dumps({
                "state": light_state,
                "timestamp": time.time(),
                "mode": bridge_cfg.mode
            }).encode("utf-8")
            
            try:
                sock.sendto(payload, (bridge_cfg.agl_host, bridge_cfg.agl_port))
                print(f"[Bridge] TX: state={light_state} to {bridge_cfg.agl_host}:{bridge_cfg.agl_port}")
            except Exception as e:
                print(f"[Bridge] UDP send error: {e}")
            
            time.sleep(bridge_cfg.poll_interval_s)
    except KeyboardInterrupt:
        print("[Bridge] Stopped by user")
        return 0
    finally:
        sock.close()
        client.close()


def main_receiver(bridge_cfg: BridgeConfig) -> int:
    """Run receiver mode: UDP -> ADAS socket + KUKSA."""
    print(f"[Bridge] Running in RECEIVER mode. Listening on {bridge_cfg.listen_host}:{bridge_cfg.listen_port}")
    
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((bridge_cfg.listen_host, bridge_cfg.listen_port))
    sock.settimeout(1.0)  # 1 second timeout for non-blocking check
    
    adas_sender = AdasObjectSender(bridge_cfg.adas_socket) if bridge_cfg.adas_enabled else None
    kuksa_pub = KuksaPublisher()
    
    last_state = "red"  # Start in safe mode
    last_update = time.time()
    yellow_since: Optional[float] = None
    
    try:
        print("[Bridge] RECEIVER mode running. Ctrl+C to stop.")
        while True:
            try:
                data, addr = sock.recvfrom(1024)
                payload = json.loads(data.decode("utf-8"))
                light_state = payload.get("state", "unknown").lower()
                last_update = time.time()
                
                print(f"[Bridge] RX: state={light_state} from {addr}")
                last_state = light_state
                
            except socket.timeout:
                # Check if we've had no updates (heartbeat timeout)
                elapsed = time.time() - last_update
                if elapsed > bridge_cfg.heartbeat_timeout_s:
                    print(f"[Bridge] Heartbeat timeout ({elapsed:.1f}s) - defaulting to RED")
                    light_state = "red"
                    last_update = time.time()
                else:
                    light_state = last_state
            except json.JSONDecodeError:
                print("[Bridge] JSON decode error")
                light_state = last_state
            except Exception as e:
                print(f"[Bridge] Receive error: {e}")
                light_state = last_state
            
            # Map state to ADAS sign class with yellow timeout
            now = time.time()
            if light_state == "yellow":
                if yellow_since is None:
                    yellow_since = now
                yellow_elapsed_s = now - yellow_since
            else:
                yellow_since = None
                yellow_elapsed_s = 0.0
            
            sign_class = map_light_to_sign_class(light_state, yellow_elapsed_s, bridge_cfg.yellow_stop_after_s)
            
            # Send to ADAS socket
            if adas_sender is not None:
                try:
                    adas_sender.send_sign(sign_class)
                except Exception as e:
                    print(f"[Bridge] ADAS socket send error: {e}")
            
            # Publish to KUKSA
            try:
                kuksa_pub.publish(light_state)
            except Exception as e:
                print(f"[Bridge] KUKSA publish error: {e}")
            
            print(f"[Bridge] RX->ADAS: light_state={light_state} -> adas_sign_class={sign_class}")
    
    except KeyboardInterrupt:
        print("[Bridge] Stopped by user")
        return 0
    finally:
        if adas_sender is not None:
            adas_sender.close()
        sock.close()


def main() -> int:
    """Main entry point."""
    args = parse_args()
    bridge_cfg = load_config(args.config)

    # Override with command-line arguments
    if args.port:
        bridge_cfg.port = args.port
    if args.baud:
        bridge_cfg.baud = args.baud
    if args.mode:
        bridge_cfg.mode = args.mode.upper()
    if args.bridge_mode:
        bridge_cfg.wireless_mode = args.bridge_mode
    if args.agl_host:
        bridge_cfg.agl_host = args.agl_host

    # Determine mode
    mode = bridge_cfg.wireless_mode or "local"
    
    if mode == "local":
        return main_local(bridge_cfg)
    elif mode == "transmitter":
        return main_transmitter(bridge_cfg)
    elif mode == "receiver":
        return main_receiver(bridge_cfg)
    else:
        print(f"[ERROR] Unknown bridge mode: {mode}")
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
