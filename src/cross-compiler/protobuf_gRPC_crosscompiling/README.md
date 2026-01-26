# Protobuf & gRPC Stub Generation (Cross-Compilation Guide)

This document explains **how and why** we generated the `*.pb.*` and `*.grpc.pb.*` files used by the KUKSA integration, and why this step is done **outside** the AGL target using a **cross-compilation / host environment**.

---

## Index

1. [Why Stub Generation Is Needed](#why-stub-generation-is-needed)
2. [Why This Is Done Outside the AGL Target](#why-this-is-done-outside-the-agl-target)
3. [Version Compatibility (Important)](#version-compatibility-important)
4. [Host Environment Setup (Docker)](#host-environment-setup-docker)
   - [Docker Image Requirements](#docker-image-requirements)
   - [How to do it](#how-to-do-it)

---

## Why Stub Generation Is Needed

KUKSA Databroker exposes its API using **gRPC** and **Protocol Buffers**.

To interact with it from C++, we must generate C++ source files from the `.proto` definitions:

- `val.proto` – defines the VAL gRPC service (PublishValue, GetValue, etc.)
- `types.proto` – defines shared data structures (SignalID, Datapoint, Value, …)
- These files are in the `kuksa/proto`: `https://github.com/eclipse-kuksa/kuksa-databroker/tree/main/proto/kuksa/val/v2`

These files are transformed into:
- `*.pb.h / *.pb.cc` → Protobuf message types
- `*.grpc.pb.h / *.grpc.pb.cc` → gRPC service stubs

These generated files are then **compiled normally** on the AGL target as part of our application.

---

## What Are gRPC Stubs (Important Concept)

KUKSA Databroker exposes its functionality as a **remote API** defined in `.proto` files.

From these `.proto` files, **gRPC stubs** are generated and used by client applications (like our publisher and reader).

### What is a Stub

A **stub** is an **auto-generated, strongly-typed client API** that allows a C++ application to call a **remote service** as if it were a local function.

In practical terms:
- The stub represents the **KUKSA Databroker API**
- Each stub method corresponds to a **remote API call**
- Calling a stub method sends a request to the databroker over **gRPC**
- All networking, serialization, and transport details are handled automatically

Example:
```cpp
stub->PublishValue(&ctx, request, &response);
```

Although this looks like a normal C++ function call, internally it:

- Serializes the request using Protocol Buffers
- Sends it over HTTP/2 via gRPC
- Waits for the remote response
- Deserializes the response
- Returns a gRPC status

---

## Why This Is Done Outside the AGL Target

The AGL target **does not ship `protoc`** (the Protocol Buffer compiler), even though it provides:
- `libprotobuf`
- `libgrpc`
- `libgrpc++`

Therefore:
- **Stub generation must happen on the host**
- The generated `.cc/.h` files are architecture-independent
- Only the compilation step must match the target architecture

---

## Version Compatibility (Important)
**This was the main reason we need Docker**

To avoid ABI / API mismatches:

- `protoc` version must be **compatible with target libprotobuf**
- `grpc_cpp_plugin` version must be **compatible with target libgrpc++**

In our case:
- AGL Databroker uses gRPC **1.60.1**
- Protobuf runtime version is **25.x**

So the host environment was aligned to these versions before generating stubs.

---

## Host Environment Setup (Docker)

### Docker Image Requirements

- `protoc` (matching protobuf version)
- `grpc_cpp_plugin` (matching gRPC version)
- `protobuf-dev`
- `grpc-dev`

This container is used **only** to generate source files.

---

## How to do it?

We use a Docker container **only** to generate stubs.

The Dockerfile used is in `/src/cross-compiler/proto-stubs-kuksa`.

### Run Docker build

#### 1. Build the image (from the folder that contains the Dockerfile)
`docker build -t kuksa-proto-gen .`

#### 2. Run an interactive shell with your current folder mounted into /work
```
docker run --rm -it \
  -v "$(pwd)":/work \
  kuksa-proto-gen \
  bash
```
This:
- Starts a container from that image
- Mounts your project folder into the container at /work

#### 3. Inside the container generate the files

Correct the path of `val.proto` and `types.proto` if needed.
```
cd /work

mkdir -p generated

protoc -I proto \
  --cpp_out=generated \
  proto/types.proto \
  proto/val.proto

protoc -I proto \
  --grpc_out=generated \
  --plugin=protoc-gen-grpc="$(which grpc_cpp_plugin)" \
  proto/types.proto \
  proto/val.proto

```
  This generates:

  ```
generated/
└── kuksa/val/v2/
    ├── types.pb.h
    ├── types.pb.cc
    ├── types.grpc.pb.h
    ├── types.grpc.pb.cc
    ├── val.pb.h
    ├── val.pb.cc
    ├── val.grpc.pb.h
    └── val.grpc.pb.cc
````

Now these files can be tranfered to AGL.

When compiling the KUKSA publisher the files are compiled together with the application.

