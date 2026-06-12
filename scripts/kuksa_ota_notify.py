#!/usr/bin/env python3
"""
Publishes OTA update status to KUKSA databroker.
Called by ota-check.sh.

Usage:
  kuksa_ota_notify.py --available <version>    # new version detected
  kuksa_ota_notify.py --installed <version>    # update just installed
  kuksa_ota_notify.py --current <version>      # publish current installed version on boot
"""

import argparse
import sys

import grpc
from kuksa.val.v2 import val_pb2, val_pb2_grpc

KUKSA_HOST    = "10.21.220.191"
KUKSA_PORT    = 55555
KUKSA_CA_CERT = "/etc/kuksa/tls/ca.crt"
KUKSA_TOKEN   = "/etc/kuksa/jwt/publisher.jwt"

_P_UPDATE_AVAILABLE  = "Vehicle.OTA.UpdateAvailable"
_P_PENDING_VERSION   = "Vehicle.OTA.PendingVersion"
_P_INSTALLED_VERSION = "Vehicle.OTA.InstalledVersion"


def _pub_bool(stub, meta, path, value):
    req = val_pb2.PublishValueRequest()
    req.signal_id.path       = path
    req.data_point.value.bool = bool(value)
    stub.PublishValue(req, metadata=meta)


def _pub_string(stub, meta, path, value):
    req = val_pb2.PublishValueRequest()
    req.signal_id.path         = path
    req.data_point.value.string = str(value)
    stub.PublishValue(req, metadata=meta)


def main():
    parser = argparse.ArgumentParser()
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--available",  metavar="VERSION")
    group.add_argument("--installed",  metavar="VERSION")
    group.add_argument("--current",    metavar="VERSION")
    args = parser.parse_args()

    try:
        token    = open(KUKSA_TOKEN).read().strip()
        ca_certs = open(KUKSA_CA_CERT, "rb").read()
    except OSError as e:
        print(f"[OTA Notify] Credenciais em falta: {e}", file=sys.stderr)
        sys.exit(1)

    creds   = grpc.ssl_channel_credentials(root_certificates=ca_certs)
    channel = grpc.secure_channel(f"{KUKSA_HOST}:{KUKSA_PORT}", creds)
    stub    = val_pb2_grpc.VALStub(channel)
    meta    = [("authorization", f"Bearer {token}")]

    try:
        if args.available:
            _pub_bool(stub, meta,   _P_UPDATE_AVAILABLE,  True)
            _pub_string(stub, meta, _P_PENDING_VERSION,   args.available)
            print(f"[OTA Notify] Update disponível: {args.available}")

        elif args.installed:
            _pub_bool(stub, meta,   _P_UPDATE_AVAILABLE,  False)
            _pub_string(stub, meta, _P_PENDING_VERSION,   "")
            _pub_string(stub, meta, _P_INSTALLED_VERSION, args.installed)
            print(f"[OTA Notify] Instalado: {args.installed}")

        elif args.current:
            _pub_string(stub, meta, _P_INSTALLED_VERSION, args.current)
            print(f"[OTA Notify] Versão atual: {args.current}")

    except Exception as e:
        print(f"[OTA Notify] Erro KUKSA: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
