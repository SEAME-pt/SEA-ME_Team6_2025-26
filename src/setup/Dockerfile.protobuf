FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive
ARG PROTOBUF_TAG=v25.8
ARG GRPC_TAG=v1.60.1

# ---- Install dependencies ----
RUN apt-get update && apt-get install -y \
    build-essential cmake git pkg-config \
    autoconf libtool \
    zlib1g-dev libc-ares-dev \
    libssl-dev wget unzip python3 \
    && rm -rf /var/lib/apt/lists/*

# ---- Build & install Protobuf v25.8 ----
RUN git clone --depth 1 --branch ${PROTOBUF_TAG} https://github.com/protocolbuffers/protobuf.git /tmp/protobuf \
 && git clone --depth 1 --branch 20230802.0 https://github.com/abseil/abseil-cpp.git /tmp/protobuf/third_party/abseil-cpp \
 && cmake -S /tmp/protobuf -B /tmp/protobuf/build \
      -Dprotobuf_BUILD_TESTS=OFF \
 && cmake --build /tmp/protobuf/build -j1 \
 && cmake --install /tmp/protobuf/build \
 && ldconfig \
 && rm -rf /tmp/protobuf

# ---- Build ONLY grpc_cpp_plugin from gRPC 1.60.1 ----
RUN git clone --depth 1 --branch ${GRPC_TAG} https://github.com/grpc/grpc.git /tmp/grpc \
 && git -C /tmp/grpc submodule update --init --recursive \
 && cmake -S /tmp/grpc -B /tmp/grpc/build \
      -DgRPC_BUILD_TESTS=OFF \
      -DgRPC_INSTALL=OFF \
      -DgRPC_BUILD_GRPC_CPP_PLUGIN=ON \
      -DgRPC_BUILD_GRPC_CSHARP_PLUGIN=OFF \
      -DgRPC_BUILD_GRPC_NODE_PLUGIN=OFF \
      -DgRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN=OFF \
      -DgRPC_BUILD_GRPC_PHP_PLUGIN=OFF \
      -DgRPC_BUILD_GRPC_PYTHON_PLUGIN=OFF \
      -DgRPC_BUILD_GRPC_RUBY_PLUGIN=OFF \
      -DgRPC_PROTOBUF_PROVIDER=package \
 && cmake --build /tmp/grpc/build -j1 \
 && cp /tmp/grpc/build/grpc_cpp_plugin /usr/local/bin/ \
 && rm -rf /tmp/grpc

# ---- Set working directory ----
WORKDIR /work

