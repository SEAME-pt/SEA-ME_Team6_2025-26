import socket
import struct
import queue
import threading

LANE_SOCKET_PATH   = "/tmp/adas_lane.sock"
OBJECT_SOCKET_PATH = "/tmp/adas_objects.sock"

# lane_status encoding — must match LaneFrame in socket_receiver.hpp
_STATUS_ENCODE = {"none": 0, "left": 1, "right": 2, "both": 3}

# LaneFrame layout (359 bytes)
_LANE_OBJECT_MOCK = (0,) + (0.0,) * 40 + (0.0,) * 4   # B + 40f + 4f
_LANE_FMT = '=fB' + 'B40f4f' + 'B40f4f'

# ObjectFrame layout — must match socket_receiver.hpp
#   uint8  count
#   [MAX_OBJECTS=4] × { uint8 class_id, float confidence, float distance }
MAX_OBJECTS  = 4
_OBJECT_FMT  = '=B' + 'Bff' * MAX_OBJECTS   # 1 + 4×9 = 37 bytes

# SignClass mapping — must match SignClass enum in socket_receiver.hpp
SIGN_UNKNOWN  = 0
SIGN_STOP     = 1
SIGN_YIELD    = 2
SIGN_SPEED_30 = 3
SIGN_SPEED_50 = 4

_lane_queue   = queue.Queue(maxsize=1)
_object_queue = queue.Queue(maxsize=1)


def _lane_worker(q):
    print("[Socket] Lane thread iniciada")
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
    while True:
        deviation, status = q.get()
        try:
            payload = struct.pack(
                _LANE_FMT,
                deviation, _STATUS_ENCODE.get(status, 0),
                *_LANE_OBJECT_MOCK,
                *_LANE_OBJECT_MOCK,
            )
            sock.sendto(payload, LANE_SOCKET_PATH)
        except Exception as e:
            print(f"[Socket] Lane erro: {e}")


def _object_worker(q):
    print("[Socket] Object thread iniciada")
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
    while True:
        objects = q.get()   # list of (class_id, confidence, distance)
        try:
            count   = min(len(objects), MAX_OBJECTS)
            entries = list(objects[:count])
            # Preenche até MAX_OBJECTS com zeros
            while len(entries) < MAX_OBJECTS:
                entries.append((SIGN_UNKNOWN, 0.0, 0.0))
            flat = [count]
            for cls_id, conf, dist in entries:
                flat += [int(cls_id), float(conf), float(dist)]
            payload = struct.pack(_OBJECT_FMT, *flat)
            sock.sendto(payload, OBJECT_SOCKET_PATH)
        except Exception as e:
            print(f"[Socket] Object erro: {e}")


def start_socket_thread():
    threading.Thread(target=_lane_worker,   args=(_lane_queue,),   daemon=True).start()
    threading.Thread(target=_object_worker, args=(_object_queue,), daemon=True).start()


def send_perception(deviation: float, status: str):
    try:
        _lane_queue.put_nowait((deviation if deviation is not None else 0.0, status))
    except queue.Full:
        pass


def send_objects(objects: list):
    """
    objects: list of (class_id, confidence, distance_m)
    class_id deve usar as constantes SIGN_* deste módulo.
    distance_m: estimativa em metros (0.0 se não disponível).
    """
    try:
        _object_queue.put_nowait(objects)
    except queue.Full:
        pass
