import socket
import struct
import queue
import threading

SOCKET_PATH = "/tmp/adas_lane.sock"

# lane_status encoding — must match LaneFrame in socket_receiver.hpp
_STATUS_ENCODE = {"none": 0, "left": 1, "right": 2, "both": 3}

# LaneFrame layout (359 bytes):
#   float  lateral_deviation
#   uint8  lane_status
#   -- LaneObject left --
#   uint8  waypoint_count
#   float  waypoints[20][2]   (40 floats, x then y, mock zeros)
#   float  polyfit[4]         (mock zeros)
#   -- LaneObject right --
#   uint8  waypoint_count
#   float  waypoints[20][2]   (40 floats, mock zeros)
#   float  polyfit[4]         (mock zeros)
_LANE_OBJECT_MOCK = (0,) + (0.0,) * 40 + (0.0,) * 4   # B + 40f + 4f
_PACK_FMT = '=fB' + 'B40f4f' + 'B40f4f'

_socket_queue = queue.Queue(maxsize=1)


def _socket_worker(q):
    print("[Socket] Thread iniciada")
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)

    while True:
        deviation, status = q.get()
        try:
            status_byte = _STATUS_ENCODE.get(status, 0)
            payload = struct.pack(
                _PACK_FMT,
                deviation, status_byte,
                *_LANE_OBJECT_MOCK,   # lane_left  (mock)
                *_LANE_OBJECT_MOCK,   # lane_right (mock)
            )
            sock.sendto(payload, SOCKET_PATH)
        except FileNotFoundError:
            pass


def start_socket_thread():
    t = threading.Thread(target=_socket_worker, args=(_socket_queue,), daemon=True)
    t.start()


def send_perception(deviation: float, status: str):
    try:
        _socket_queue.put_nowait((deviation if deviation is not None else 0.0, status))
    except queue.Full:
        pass
