# KUKSA reader implementation

## Overview

`kuksa_reader_subscriber` is a C++ gRPC client that runs on **RPi4** and subscribes
to vehicle signals from a **KUKSA Databroker running on RPi5 (AGL)**.

It uses **gRPC subscriptions** instead of polling and prints values **only when they change**.

---

## Goal

`RPi4 ── gRPC ──> RPi5 (AGL + KUKSA Databroker)`

### The big idea: what “subscribe” means in gRPC + KUKSA

With polling you do:

1. client asks “give me Vehicle.Speed”
2. server replies with the current value
3. client repeats every 1s

With subscribe you do:

1. client sends **one request** saying “I want updates for these paths”
2. server replies with a **stream** (a continuous sequence of messages)
3. client stays blocked reading from the stream and prints whenever a new update arrives
    
So the program becomes: **“open stream → read forever → print updates”**.

### The subscription flow

1. Create channel to `RPi5_IP:55555`
2. Create stub
3. Build SubscribeRequest with paths
4. Call `Subscribe()` → get stream
5. `while(Read)` print updates
6. If stream ends → check status

---

## Dependencies (RPi4)

The RPi4 must have the **same runtime dependencies as the RPi5**.

Mainly:

- **libgrpc++ 1.60.1**
- **protobuf runtime 25.0.1**

> ⚠️ `protoc` is **not required** on the RPi4  
> The generated protobuf / gRPC code is copied from the RPi5. generated folder

---

## How it works

1. The subscriber creates a gRPC connection to the KUKSA Databroker.
2. It sends a `SubscribeRequest` with a list of VSS signal paths.
3. The databroker opens a **server → client stream**.
4. When values are updated, the databroker pushes them to the subscriber.
5. The subscriber prints a value **only if it changed** from the previous one.

A better explanation of code is decribed in the README.md of the src folder -> there is a deep explanation on how to convert this for the Qt app.

---

## Project structure

```
.
├── Makefile
├── src/   
|   └── kuksa_reader_subscriber.cpp
└── generated/
    └── kuksa/val/v2/
    ├── val.pb.cc
    ├── val.grpc.pb.cc
    └── types.pb.cc
```
---

## How to run

1. Run make on the folder
2. Run the program - for now you have to specify the IP of RPi5
`
./kuksa_reader_subscriber 10.21.220.191:55555
`
3. If the connection is successful, the program blocks and prints updates when values change.

Expected output
```
[KUKSA] Vehicle.Speed = 32.4
[KUKSA] Vehicle.Cabin.AirTemperature = 21.8
----
[KUKSA] Vehicle.Speed = 33.1
----
````
---

## Common problems

### 1. RE2 not found

Error:
`Package 're2', required by 'grpc', not found`

This happens because gRPC depends on RE2, but the RE2 development package is missing.

Fix
```
sudo apt-get update
sudo apt-get install -y libre2-dev pkg-config
```

Then run make again.

### 2. Databroker configuration (RPi5)

To allow the RPi4 to connect, the KUKSA Databroker must listen on all interfaces.

Edit:
`/etc/default/kuksa-databroker`

Set:
`EXTRA_ARGS="--address 0.0.0.0 --port 55555 --vss /etc/kuksa/vss.json --insecure --disable-authorization"`


Restart the service after applying the change.