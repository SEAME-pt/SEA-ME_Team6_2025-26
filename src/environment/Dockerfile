FROM ubuntu:24.04 as build

#! Base Tools
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    bash \
    wget \
    unzip \
    python3 \
    python3-pip \
    libssl-dev \
    lcov \
    genhtml \
    ffmpeg \
    && rm -rf /var/lib/apt/lists/*

#! Base Tools
RUN apt-get update && apt-get install -y \
    qt6-base-dev \
    qt6-base-dev-tools \
    qt6-quickcontrols2-dev \
    qt6-dbus-dev \
    libqt6test6t64 \
    libqt6network6t64 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY ../tests/unit/python/requirements.txt .

RUN pip install -r requirements.txt

COPY . .

RUN cmake -S src/cluster -B src/cluster/build
RUN cmake --build src/cluster/build

RUN cmake -S src/tests -B src/tests/build
RUN cmake --build src/tests/build

RUN ctest --test-dir src/tests/build --output-on-failure
