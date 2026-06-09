#!/usr/bin/env python3
"""
BLE Central Receiver for Traffic Light State (Path B)

Connects to micro:bit BLE peripheral, receives traffic-light state,
and injects into ADAS Manager socket.

Usage:
    python3 ble_trafficlight_receiver.py --config config.json [--device-name "Trafficlight"] [--verbose]
"""

import asyncio
import json
import logging
import struct
import time
from pathlib import Path
from argparse import ArgumentParser
from typing import Optional

try:
    from bleak import BleakClient, BleakScanner
    from bleak.exc import BleakDeviceNotFoundError, BleakError
except ImportError:
    print("ERROR: bleak library not found. Install: pip install bleak")
    exit(1)

# Logging setup
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s [%(levelname)s] %(name)s: %(message)s'
)
logger = logging.getLogger("BLE-TL-Receiver")

# BLE configuration
NORDIC_UART_SERVICE_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
NORDIC_UART_RX_UUID = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"  # read/notify
NORDIC_UART_TX_UUID = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"  # write

# State mapping
STATE_MAP = {
    b'R': 'RED',
    b'Y': 'YELLOW',
    b'G': 'GREEN',
    'R': 'RED',
    'Y': 'YELLOW',
    'G': 'GREEN',
}

SIGN_MAP = {
    'RED': 'SIGN_TL_RED',
    'YELLOW': 'SIGN_TL_YELLOW',
    'GREEN': 'SIGN_TL_GREEN',
}


class BLETrafficLightReceiver:
    """
    Connects to micro:bit BLE peripheral, receives traffic-light state,
    and forwards to ADAS Manager socket.
    """

    def __init__(self, config_path: Path, device_name: str = "Trafficlight", verbose: bool = False):
        """
        Initialize receiver.

        Args:
            config_path: Path to config.json
            device_name: BLE device name to search for
            verbose: Enable verbose logging
        """
        self.config_path = Path(config_path)
        self.device_name = device_name
        self.verbose = verbose

        if verbose:
            logger.setLevel(logging.DEBUG)

        # Load config
        try:
            with open(self.config_path) as f:
                self.config = json.load(f)
        except FileNotFoundError:
            logger.error(f"Config file not found: {self.config_path}")
            raise
        except json.JSONDecodeError as e:
            logger.error(f"Invalid JSON in config: {e}")
            raise

        # Extract config values
        self.adas_socket = self.config.get("adas_bridge", {}).get("object_socket", "/tmp/adas_objects.sock")
        self.timeout_sec = self.config.get("traffic_light", {}).get("timeout_sec", 5.0)
        self.yellow_stop_after_s = self.config.get("adas_bridge", {}).get("yellow_stop_after_s", 2.0)
        self.heartbeat_interval = self.config.get("wireless", {}).get("heartbeat_interval_sec", 1.0)

        self.client: Optional[BleakClient] = None
        self.device_address: Optional[str] = None
        self.current_state = 'RED'
        self.last_update_time = time.time()
        self.running = True

        logger.info(f"BLE Receiver initialized (device: {device_name}, timeout: {self.timeout_sec}s)")
        logger.info(f"ADAS socket: {self.adas_socket}")

    async def find_device(self) -> Optional[str]:
        """Scan for BLE device by name."""
        logger.info(f"Scanning for device: {self.device_name}...")

        try:
            devices = await BleakScanner.discover(timeout=10.0)
            for device in devices:
                if self.device_name.lower() in (device.name or "").lower():
                    logger.info(f"Found device: {device.name} ({device.address})")
                    return device.address
        except BleakError as e:
            logger.error(f"Scan error: {e}")
            return None

        logger.error(f"Device not found: {self.device_name}")
        return None

    async def notification_handler(self, sender, data: bytearray):
        """Handle incoming BLE notification."""
        try:
            # Parse state from notification
            if isinstance(data, bytes):
                state_raw = data[0:1]  # First byte
            else:
                state_raw = bytes([data[0]])

            state = STATE_MAP.get(state_raw, STATE_MAP.get(chr(data[0]), 'RED'))

            if self.verbose:
                logger.debug(f"BLE notification: raw={state_raw.hex()}, state={state}")

            self.current_state = state
            self.last_update_time = time.time()

            # Inject into ADAS socket
            self._inject_adas_object(state)

        except Exception as e:
            logger.error(f"Notification parse error: {e}")

    def _inject_adas_object(self, state: str):
        """Write traffic-light object to ADAS socket."""
        try:
            sign_class = SIGN_MAP.get(state, 'SIGN_TL_RED')

            # Simple ObjectFrame format (matches trafficlight_vehicle_bridge.py)
            obj_data = {
                "class": sign_class,
                "confidence": 1.0,
                "state": state,
                "timestamp": time.time(),
            }

            # Try to write to socket
            try:
                import socket
                sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
                sock.connect(self.adas_socket)
                sock.sendall(json.dumps(obj_data).encode() + b'\n')
                sock.close()

                if self.verbose:
                    logger.debug(f"Injected ADAS object: {sign_class}")
            except FileNotFoundError:
                logger.warning(f"ADAS socket not ready: {self.adas_socket}")
            except ConnectionRefusedError:
                logger.warning(f"ADAS socket connection refused")

        except Exception as e:
            logger.error(f"ADAS injection error: {e}")

    async def safety_watchdog(self):
        """Monitor connection and timeout. Force RED on loss."""
        while self.running:
            await asyncio.sleep(self.heartbeat_interval)

            time_since_update = time.time() - self.last_update_time

            # Timeout check
            if time_since_update > self.timeout_sec:
                if self.current_state != 'RED':
                    logger.warning(f"Timeout (>{self.timeout_sec}s), forcing RED")
                    self.current_state = 'RED'
                    self._inject_adas_object('RED')

            # Yellow escalation check
            if self.current_state == 'YELLOW':
                if time_since_update > self.yellow_stop_after_s:
                    logger.warning(f"Yellow timeout (>{self.yellow_stop_after_s}s), escalating to RED")
                    self.current_state = 'RED'
                    self._inject_adas_object('RED')

    async def connect_and_listen(self):
        """Connect to BLE device and start listening."""
        while self.running:
            try:
                # Find device if not already found
                if not self.device_address:
                    self.device_address = await self.find_device()
                    if not self.device_address:
                        logger.error("Failed to find device, retrying in 5s...")
                        await asyncio.sleep(5.0)
                        continue

                # Connect
                logger.info(f"Connecting to {self.device_address}...")
                self.client = BleakClient(self.device_address)

                async with self.client:
                    logger.info("Connected to BLE device")
                    self.current_state = 'RED'  # Safe default on connection
                    self.last_update_time = time.time()

                    # Subscribe to notifications
                    try:
                        await self.client.start_notify(NORDIC_UART_RX_UUID, self.notification_handler)
                        logger.info(f"Subscribed to {NORDIC_UART_RX_UUID}")
                    except Exception as e:
                        logger.warning(f"Could not subscribe to Nordic UART RX: {e}")
                        logger.info("Trying generic notification approach...")

                    # Keep connection alive with watchdog
                    while self.running:
                        await asyncio.sleep(1.0)

            except BleakDeviceNotFoundError:
                logger.error(f"Device disconnected: {self.device_address}")
                self.device_address = None
                self.current_state = 'RED'
                self._inject_adas_object('RED')
                await asyncio.sleep(5.0)

            except BleakError as e:
                logger.error(f"BLE error: {e}")
                self.device_address = None
                self.current_state = 'RED'
                self._inject_adas_object('RED')
                await asyncio.sleep(5.0)

            except Exception as e:
                logger.error(f"Unexpected error: {e}")
                await asyncio.sleep(5.0)

    async def run(self):
        """Main event loop."""
        tasks = [
            asyncio.create_task(self.connect_and_listen()),
            asyncio.create_task(self.safety_watchdog()),
        ]

        try:
            await asyncio.gather(*tasks)
        except KeyboardInterrupt:
            logger.info("Shutdown requested")
            self.running = False


async def main():
    parser = ArgumentParser(description="BLE Traffic Light Receiver for ADAS")
    parser.add_argument("--config", default="config.json", help="Config file path")
    parser.add_argument("--device-name", default="Trafficlight", help="BLE device name")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose logging")

    args = parser.parse_args()

    receiver = BLETrafficLightReceiver(
        config_path=args.config,
        device_name=args.device_name,
        verbose=args.verbose,
    )

    await receiver.run()


if __name__ == "__main__":
    asyncio.run(main())
