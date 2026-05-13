#!/usr/bin/env python3
"""
KUKSA Bridge — lançado pelo adas_manager como subprocess.

Lê linhas de stdin escritas pelo adas_manager:
  L <deviation_float> <status_string>
  O <speed_limit_float> <traffic_light_string> <signs_json> <extras_json>

Publica no KUKSA databroker via gRPC v2 (TLS + JWT).
"""

import json
import sys

import grpc
from kuksa.val.v2 import val_pb2, val_pb2_grpc

KUKSA_HOST    = "10.21.220.191"
KUKSA_PORT    = 55555
KUKSA_CA_CERT = "/etc/kuksa/tls/ca.crt"
KUKSA_TOKEN   = "/etc/kuksa/jwt/publisher.jwt"

_P_DEVIATION   = "Vehicle.ADAS.LaneKeepAssist.LateralDeviation"
_P_STATUS      = "Vehicle.ADAS.LaneKeepAssist.LaneStatus"
_P_OBJ_ENABLED = "Vehicle.ADAS.ObjectDetection.IsEnabled"
_P_SPEED_LIMIT = "Vehicle.ADAS.ObjectDetection.SpeedLimit"
_P_TRAFFIC     = "Vehicle.ADAS.ObjectDetection.TrafficLight"
_P_SIGNS       = "Vehicle.ADAS.ObjectDetection.StreetSignals"
_P_EXTRAS      = "Vehicle.ADAS.ObjectDetection.Extras"


def _pub_float(stub, meta, path, value):
    req = val_pb2.PublishValueRequest()
    req.signal_id.path        = path
    req.data_point.value.float = float(value)
    stub.PublishValue(req, metadata=meta)


def _pub_string(stub, meta, path, value):
    req = val_pb2.PublishValueRequest()
    req.signal_id.path         = path
    req.data_point.value.string = str(value)
    stub.PublishValue(req, metadata=meta)


def _pub_bool(stub, meta, path, value):
    req = val_pb2.PublishValueRequest()
    req.signal_id.path       = path
    req.data_point.value.bool = bool(value)
    stub.PublishValue(req, metadata=meta)


def _pub_string_array(stub, meta, path, values):
    req = val_pb2.PublishValueRequest()
    req.signal_id.path = path
    req.data_point.value.string_array.values.extend(values)
    stub.PublishValue(req, metadata=meta)


def main():
    try:
        token    = open(KUKSA_TOKEN).read().strip()
        ca_certs = open(KUKSA_CA_CERT, "rb").read()
    except OSError as e:
        print(f"[KUKSA Bridge] Credenciais em falta: {e}", file=sys.stderr)
        # Drena stdin sem publicar
        for _ in sys.stdin:
            pass
        return

    creds   = grpc.ssl_channel_credentials(root_certificates=ca_certs)
    channel = grpc.secure_channel(f"{KUKSA_HOST}:{KUKSA_PORT}", creds)
    stub    = val_pb2_grpc.VALStub(channel)
    meta    = [("authorization", f"Bearer {token}")]

    print(f"[KUKSA Bridge] Ligado a {KUKSA_HOST}:{KUKSA_PORT}", file=sys.stderr, flush=True)

    # Publica IsEnabled=True no arranque
    try:
        _pub_bool(stub, meta, _P_OBJ_ENABLED, True)
    except Exception:
        pass

    for raw in sys.stdin:
        line = raw.strip()
        if not line:
            continue

        try:
            if line.startswith("L "):
                # L <deviation> <status>
                _, dev_s, status = line.split(" ", 2)
                _pub_float(stub, meta,  _P_DEVIATION, float(dev_s))
                _pub_string(stub, meta, _P_STATUS,    status)

            elif line.startswith("O "):
                # O <speed_limit> <traffic_light> <signs_json> <extras_json>
                _, sl_s, traffic, signs_s, extras_s = line.split(" ", 4)
                signs  = json.loads(signs_s)
                extras = json.loads(extras_s)
                _pub_float(stub, meta,        _P_SPEED_LIMIT, float(sl_s))
                _pub_string(stub, meta,       _P_TRAFFIC,     traffic)
                _pub_string_array(stub, meta, _P_SIGNS,       signs)
                _pub_string_array(stub, meta, _P_EXTRAS,      extras)

        except Exception as e:
            print(f"[KUKSA Bridge] Erro: {e} (linha: {line!r})", file=sys.stderr, flush=True)


if __name__ == "__main__":
    main()
