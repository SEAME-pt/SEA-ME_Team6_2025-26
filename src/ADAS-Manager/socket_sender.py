#!/usr/bin/env python3
"""
Socket sender — INFERENCE_DUAL → ADAS Manager (C++)

Sends:
  LaneFrame   → /tmp/adas_lane.sock
  ObjectFrame → /tmp/adas_objects.sock

Structs must match socket_receiver.hpp exactly (little-endian, packed).
"""
import socket
import struct

LANE_SOCKET   = "/tmp/adas_lane.sock"
OBJECT_SOCKET = "/tmp/adas_objects.sock"

# SignClass enum — must match socket_receiver.hpp
SIGN_UNKNOWN    = 0
SIGN_STOP       = 1
SIGN_YIELD      = 2
SIGN_SPEED_30   = 3
SIGN_SPEED_50   = 4
SIGN_SPEED_80   = 5
SIGN_OBSTACLE   = 6
SIGN_PEDESTRIAN = 7
SIGN_TL_GREEN   = 8
SIGN_TL_RED     = 9
SIGN_TL_YELLOW  = 10

_STATUS_MAP = {"none": 0, "left": 1, "right": 2, "both": 3}
MAX_OBJECTS = 4

# Pre-built zero LaneObject (177 bytes):
#   uint8  waypoint_count  = 0
#   float  waypoints[20][2] = zeros  (20 × 2 floats = 40 floats)
#   float  polyfit[4]       = zeros
_LANE_OBJ_ZERO = struct.pack('<B44f', 0, *([0.0] * 44))

_lane_sock   = None
_object_sock = None


def start_socket_thread():
    """Initialise UNIX DGRAM sockets. Call once before the inference loop."""
    global _lane_sock, _object_sock
    _lane_sock   = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
    _object_sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)


def send_perception(deviation: float, status: str):
    """
    Pack LaneFrame and send to ADAS Manager.

    LaneFrame layout (359 bytes):
      float   lateral_deviation
      uint8   lane_status          (0=none 1=left 2=right 3=both)
      LaneObject lane_left         (177 bytes, zeros — MPC not implemented)
      LaneObject lane_right        (177 bytes, zeros — MPC not implemented)
    """
    if _lane_sock is None:
        return
    status_byte = _STATUS_MAP.get(status, 0)
    payload = (struct.pack('<fB', float(deviation), status_byte)
               + _LANE_OBJ_ZERO
               + _LANE_OBJ_ZERO)
    try:
        _lane_sock.sendto(payload, LANE_SOCKET)
    except Exception:
        pass


def send_objects(obj_list):
    """
    Pack ObjectFrame and send to ADAS Manager.

    obj_list: list of (class_id, confidence, distance_m, theta_cam_deg)
              max MAX_OBJECTS entries — extras are silently dropped.

    ObjectFrame layout:
      uint8  count
      N × DetectedObject {
          uint8   class_id
          float32 confidence
          float32 distance      (metres)
          float32 theta_cam     (degrees: 0=straight, +=right, -=left)
      }
    """
    if _object_sock is None:
        return
    count = min(len(obj_list), MAX_OBJECTS)
    payload = struct.pack('<B', count)
    for cls, conf, dist, theta in obj_list[:count]:
        payload += struct.pack('<Bfff', int(cls), float(conf), float(dist), float(theta))
    # Pad remaining slots — C++ expects fixed sizeof(ObjectFrame) = 1 + MAX_OBJECTS×13 bytes
    for _ in range(MAX_OBJECTS - count):
        payload += struct.pack('<Bfff', 0, 0.0, 0.0, 0.0)
    try:
        _object_sock.sendto(payload, OBJECT_SOCKET)
    except Exception:
        pass
