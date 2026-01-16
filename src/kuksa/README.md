# KUKSA Databroker Integration

## Index

1. [Overview](#overview)
2. [Architecture](#architecture)
3. [Why KUKSA?](#why-kuksa)
4. [Data Model (VSS)](#data-model-vss)
5. [Build Layout](#build-layout)
6. [Databroker Configuration](#databroker-configuration)
7. [Publisher (kuksa_publisher)](#publisher-kuksa_publisher)
8. [Reader (kuksa_reader)](#reader-kuksa_reader)
9. [Next Steps](#next-steps)


## Overview

This module integrates **KUKSA Databroker (v2)** into our project to expose vehicle data over a **standardized VSS (Vehicle Signal Specification)** interface.

The current implementation demonstrates a **full data flow**:

CAN data  →  C++ Publisher  →  KUKSA Databroker  →  C++ Reader (Qt app)

It is designed as a **foundation** for later integration with:
- real CAN frames
- dashboards (Qt)
- other vehicle services

---

## Architecture

### Components

- **KUKSA Databroker (0.5.0)**  
  Central data hub exposing vehicle signals over gRPC.

- **Publisher (`kuksa_publisher`)**  
  C++ application that publishes values to KUKSA using the VAL gRPC API.

- **Reader (`kuksa_reader`)**  
  Simple C++ client that reads values back from KUKSA (polling).

- **VSS Metadata (`vss.json`)**  
  Defines which vehicle signals exist and their data types.

---

## Why KUKSA?

- Provides a **standard vehicle data model (VSS)**
- Uses **gRPC** for fast, typed IPC
- Aligns with **AGL / SDV architecture**

---

## Data Model (VSS)

The databroker is started with a custom VSS file defining the available signals.

Example signals used in this demo:

| Path | Type | Description |
|----|----|----|
| `Vehicle.Speed` | double | Vehicle speed |
| `Vehicle.Cabin.AirTemperature` | double | Cabin temperature |
| `Vehicle.Test.Heartbeat` | int32 | Demo heartbeat counter |

These signals **must exist** in the VSS metadata, otherwise KUKSA will reject publishes with `Path not found`.

---

## Build Layout

```
kuksa/
├── src/
│ ├── kuksa_publisher.cpp
│ └── kuksa_reader.cpp
├── generated/
│ └── kuksa/val/v2/
│ ├── *.pb.cc / *.pb.h
│ └── *.grpc.pb.cc / *.grpc.pb.h
├── bin/
│ ├── kuksa_publisher
│ └── kuksa_reader
├── vss_min.json
├── Makefile
└── README.md
```


---

## Databroker Configuration

### VSS File Location

The VSS metadata is stored at:
`/etc/kuksa/vss.json`


### Systemd Service

The image provides a built-in service:
`kuksa-databroker.service` 

Can be configured in:
`/etc/default/kuksa-databroker`

Example configuration:

```sh
EXTRA_ARGS="--address 0.0.0.0 \
            --port 55555 \
            --vss /etc/kuksa/vss.json \
            --insecure \
            --disable-authorization"
```

### Publisher (kuksa_publisher)

#### Purpose

- Publishes vehicle data to KUKSA

- Acts as the bridge between CAN logic and VSS signals

#### Key concepts

- Uses gRPC VAL v2 API

- Publishes values using:

    - SignalID (VSS path)

    - Datapoint → Value (oneof typed_value)

#### Publish Logic

```
req.mutable_signal_id()->set_path("Vehicle.Speed");

req.mutable_data_point()->mutable_value()->set_double_(speed);
```

`req` is a `kuksa::val::v2::PublishValueRequest`. It is possible to use `mutable_signal_id()->set_path()`to set the VSS path.

In the same way, data_point is used to set the value in the VSS tree.

For now it is needed to run `./kuksa_publisher`, after it is going to be a system service.

### Reader (kuksa_reader)

#### Purpose

- Reads values back from KUKSA to verify correctness

#### How it Works

- Calls GetValue for a list of VSS paths

- Decodes the Value oneof and prints it

### Next Steps

- Replace demo values with real CAN decoding ☑️

- Move publisher into a daemon/service 

- Add subscriptions instead of polling ☑️

- Integrate with Qt dashboard / VISS

- Research on JWT / TLS - optional