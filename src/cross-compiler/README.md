# 🚀 Cross-Compiler

## 📚 Index
- [👋 Introduction](#sec-intro)
- [🧠 Core Ideas](#sec-core-ideas)
- [🤔 Why we use Cross-Compilation](#sec-why-cross)
- [🐋 Why we use Docker](#sec-why-docker)
- [🏗️ System Architecture](#sec-system-architecture)
- [⚙️ How it works (Step by Step)](#sec-how-it-works)
- [🧰 Building with Docker](#sec-build-docker)
- [📦 Deploying to Raspberry Pi](#sec-deploy-rasp)
- [🚨 Issues Log](#sec-issues)
- [🔗 Links](#sec-links)

---

<a id="sec-intro"></a>
## 👋 Introduction

A cross-compiler is a type of compiler capable of generating executable code for a plataform or operating system different from the one which it is running.  
In simpler terms, it allows developers to build programs for another achitecture or environment - for example, compiling code on a computer (x86_64) that will later run on a smaller embedded device such as a Raspberry Pi (ARM64).  
Cross-compilation is especially useful when the target system:
 - Has limited computational resources.
 - Runs on a different CPU architecture (eg. ARM vs x86).
 - Uses a different operating system or runtime environment.
 - Cannot easily host a full compiler toolchain due to storage or performance constraints.

Instead of compiling software directly on the target hardware - which is ofter slow and inefficient - we can use a cross-compiler to produce binaries remotely from a more capable development machine.  
This approach not only improves build speed and developer productivity, but also ensure consistency, since the build environment can be standardized using tools like Docker.  
In modern embedded development, cross-compilers are fundamental to building complex frameworks such as Qt, OpenCV or custom Linux kernels, which can be time-consuming or even impossible to compile natively on lightweight devices.  
By defining a clear separation between the host and the target, cross-compilation becomes an essential bridge between desktop development and embedded deployment.

---

<a id="sec-core-ideas"></a>
## 🧠 Core Ideas

- **Cross-compilation**: Process of building software on one system that runs on another, often with a different CPU architecture or OS.
- **Host System**: The development machine performing the build - typically an x86_64 Linux workstation with Docker and CMake installed. 
- **Target System**: The device the software is meant to run on - in this case, a Raspberry Pi running ARM64.
- **Toolchain**: A complete set of compilers, linkers and build tools configured to produce binaries compatible with the target platform.
- **Sysroot**: A structured snapshot of the target's filesystem (libraries, headers, binaries) used during compilation to ensure correct linking and compatibility.
- **CMake Toolchain File**: A configuration file defining compiler paths, sysroot locations and platform settings that guide the cross-compile process.
- **Docker Integration**: Containerized environments used to isolate, reproduce, and automate cross-compilation, avoiding host dependency issues.
- **Dual-Container Architecture**: One Docker image replicates the target system to generate the sysroot, another hosts the cross-compiler that buils the project and Qt for ARM.
- **Qt Cross-Build**: Qt itself is built inside the cross-compiler container using the ARM sysroot, enabling deployment of complext QtQuick/QML apps directly to Raspberry Pi.
- **Dependency Consistency**: Ensures that the same versions of libraries used during compilation exist on the target device, preventing runtime errors.
- **Environment Replication**: Recreates the Raspberry Pi's file system and libraries inside the host machine through Docker, eliminating the need for physical hardware during builds.
- **Automation**: The entire toolchain setup, build and packaging process is fully scripted through Dockerfiles and CMake commands for reproducibility.
- **Performance Optimization:** Cross-compiling on a high-performance host drastically reduces build times compared to compiling natively on ARM hardware.
- **Portability**: The Dockerized toolchain can be shared among team members, CI/CD systems or other workstations, guaranteeing identical builds.
- **Deployment Pipeline**: The compiled binaries and Qt libraries are extracted, transferred to the Raspberry Pi, and executed with the minimal manual configuration.
- **Scability**: The same cross-compile approach can be extended to other ARM-based devices or different embedded platforms.
- **Error Isolation**: Containarized builds prevent host contamination and allow isolated debugging of environment-specific issues.
- **Reproducibility**: Each build is fully deterministic - rebuilding the Docker image always produces the same binaries, ensuring stable software releases.
- **Integration with Project**: The cross-compiler forms the bridge between the development environment (Instrument Cluster source code) and the ARM-based runtime system (Raspberry Pi).

---

<a id="sec-why-cross"></a>
## 🤔 Why we use Cross-Compilation

During the early stages of development, we experimented with several methods to build and deploy our Qt application on the Raspberry Pi.  
Although each method seemed viable in theory, we quickly encountered techincal barriers that made local or direct builds impractical.  
The challenges ultimately led us to adopt a cross-compilatio  workflow, which proved to be faster, more reliable and scalable.

##### **Attempt 1 - Building Directly on the Raspberry Pi**

Our first approach was the most straightforward one - installing all necessary Qt libraries, dependencies, and build tools directly on the Raspberry Pi, then compiling the application natively.  
While simple, this approach exposed several limitations of the Raspberry Pi environment:  
- **Hardware Constraints:**
The Raspberry Pi, while capable for runtime execution, lacks the computational power required to build large projects efficiently. Compiling Qt 6 and the associated modules can take many hours, sometimes exceeding the device's thermal or memory limits.
- **Package Fragmentation and Version Mismatches:**
The official Raspberry Pi repositories provide older or incomplete Qt packages, and attempting to compile newer versions let to incompatible versions, missing modules and depdency conflicts.


Despite several attepemts, this method proved unreliable, time-consuming and unsuitable for a reproducible development workflow.

##### **Attempt 2 - Building on the Host and Transferring the Executable**

After facing repeated build failures on the Raspberry Pi, we tried a different strategy, compile the project on our development workstation (Ubuntu x86_64) using the standard Qt setup and then copy the generated binary to the Raspberry Pi via SSH.  
At first this approach seemed promising - compilation on a desktop was fast and stable, and deployment was straightforward.  
Howerver, once transferred the executable refused to run, returning the error:  
```bash
./HelloQt6Qml: cannot execute binary file: Exec format error
```
This occured because the binary was compiled for a different CPU architecture.  
Our host machine used and x86_64 processor, while the Raspberry Pi runs on an ARM64 (AArch64) architecture.  
Even though both run Linux, their instruction sets, memory alignment, and binary interfaces are fundamentally incompatible.  
In other words, the Raspberry Pi simple could not execute a binary compiled for x86_64.
This approach highlighted a critical reality:  
**To run an application on a target device with a different architecture, you must buil it for that architecture.**

After these experiments, it became clear that what we needed was a way to compile on our host machine, taking advantage of its speed and resources, but generate binaries that would run on the Raspberry Pi's ARM64 environment.  
That's where cross-compilation comes in.  
Cross-compilation provided the bridge between the host and target architectures by using a specialized compiled toolchain configured for the target's architecture.  
This means the host can build binaries as if it were "pretending" to be the target system - linking against the correct ARM libraries, headers and system paths - all without ever touching the Raspberry Pi.

---

<a id="sec-why-docker"></a>
## 🐋 Why we use Docker

Once we decided to use cross-compilation, the next challenge was how to set up and maintain that environment.  
Cross-compiling Qt manually for the Raspberry Pi can be extremely complex, it requires:
- Installing and configuring a cross-compiler toolchain for ARM64.
- Copying or generating a sysroot (a mirror of the target filesystem).
- Setting multiple CMake variables (`CMAKE_SYSROOT`, `CMAKE_PREFIX_PATH`, etc).
- Ensuring consitent library versions between host and target. 

Doing this manually often leads to configuration drift, version conflicts, and inconsistent build accros different developers or systems.

##### **Manual Setup**
Without Docker, you can still cross-compile by installing:
```bash
$ sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
```
Then manually setting up environment variables, downloading the Raspberry Pi sysroot, and configuring CMake toolchains.  
However, this approach is:  
- **Error-prone:** One missing path or mismatched library breaks the build.
- **Hard to reproduce:** Every developer must manually replicate the same setup.
- **Unstable over time:** System updates or different Ubuntu versions can silently break the configuration.

##### **Dockerized Cross-Compilation**
To eliminate these issues, we use Docker containers to encapsulate and automate the entire cross-compilation process.  
Our setup uses two Dockerfiles:
- **DockerFileRasp (Target Emulator):**
  - Simulates the Raspberry Pi's environment using `arm64v8/debian:bookworm`.
  - Installs all the required dependencies and exports a compressed sysroot `rasp.tar.gz`.
- **DockerFile (Cross-Compiler):**
  - Runs on the x86_64 host.
  - Uses the generated sysroot to build Qt and our application for the ARM64 target.

**Benefits of using Docker:**
- **Reproducibility:** Every build environment is defined by a Dockerfile, ensuring consistent results across machines. 
- **Automation:** Building the sysroot, toolchain, Qt and final binaries is scripted - no manual setup required. 
- **Isolation:** Dependencies and libraries inside the container don't interfere with the host OS. 
- **Performance:** The heavy compilation still runs on the host hardware, but inside a clean, isolated container. 
- **Portability:** The same setup can target other ARM boards with Dockerfile adjustments. 
- **Stability:** Containers ensure consistent library versions and system environments, eliminating "works on my machine" problems. 

---

<a id="sec-system-architecture"></a>
## 🏗️ System Architecture

? 

---

<a id="sec-how-it-works"></a>
## ⚙️ How it works (Step by Step)

? 

---

<a id="sec-build-docker"></a>
## 🧰 Building with Docker

? 

---

<a id="sec-deploy-rasp"></a>
## 📦 Deploying to Raspberry Pi

? 

---

<a id="sec-issues"></a>
## 🚨 Issues Log

<a id="issue-x"></a>

### Issue #x - 
**- Error Example:**

**- Cause:** 

**- Solution:**

<!-- <a id="issue-x"></a>

### Issue #x - 
**- Error Example:**

**- Cause:** 

**- Solution:** -->

---

<!-- <a id="sec-summary"></a>
## 🧩 Summary

1. **Write Source Code** → Program in a high-level language like C++.
2. Instead of a native compiler, we use a compiler configurated for a **different target platform**.
3. Source code is preprocessed: **macros** are expanded, **headers** are included and code is prepared for compilation.
4. Cross-compiler translates the **preprocessed code** into **target-specific** assembly code.
5. The object is linked with **target libraries** and **system APIs** from the sysroot to produce the final executable.
6. It creates a program that runs on the target system.
7. The compiled executable is copied or **deployed** to the **target** device.

<div align="center">
  <img src="https://media.geeksforgeeks.org/wp-content/uploads/20220915123006/Crosscompiler-660x330.png" />
</div>

--- -->

<!-- <a id="sec-how-it-works"></a>
## 📄 How our Cross-Compiler works

Our **host machines** are running `Ubuntu x86_64`, while the **target machine** uses `Bookworm aarch64`. To simplify working across two different operating systems, we decided to use a **cross-compiler** with Docker.  

Thanks to the work of Mr. [PhysicsX](https://github.com/PhysicsX), we were able to incorporate mostly of his work into our project by following the instructions and explanations provided in his [GitHub repository](https://github.com/PhysicsX/QTonRaspberryPi?tab=readme-ov-file) and [Youtube tutorial](https://youtu.be/5XvQ_fLuBX0?si=WcI7c6L5jJSLzCvF).  

The cross-compiler works by creating **two Docker containers** using different Dockerfiles: one simulates the OS of our target, (this allow us to build Qt without needing a Raspberry Pi) and the other serves as the **cross-compiler**, responsible for preparing the executable so it can be run on the target machine.  

This README will only cover the core components of how the Dockerfiles work. For more detailed information about how it was done and how to install it, we recommend consulting the work of Mr. [PhysicsX's](https://github.com/PhysicsX) work.  

`DockerFileRasp (Target):`  

```dockerfile
FROM arm64v8/debian:bookworm

ENV DEBIAN_FRONTEND=noninteractive

RUN touch /build.log
```

Definition of the base image **Debian Bookworn (arm64)** and environment setup by disabling **interactive prompts** during installation and creation of a **build.log file** to log the build output.  

```dockerfile
RUN { \
   echo "deb http://deb.debian.org/debian bookworm main contrib non-free" > /etc/apt/sources.list && \
    echo "deb-src http://deb.debian.org/debian bookworm main contrib non-free" >> /etc/apt/sources.list && \
    echo "deb http://security.debian.org/debian-security bookworm-security main contrib non-free" >> /etc/apt/sources.list && \
    echo "deb-src http://security.debian.org/debian-security bookworm-security main contrib non-free" >> /etc/apt/sources.list && \

  ...

  apt-get update && \
    apt-get full-upgrade -y && \
    apt-get install -y \
    libboost-all-dev libudev-dev libinput-dev libts-dev libmtdev-dev \
    libjpeg-dev libfontconfig1-dev libssl-dev libdbus-1-dev libglib2.0-dev \
    libxkbcommon-dev libegl1-mesa-dev libgbm-dev libgles2-mesa-dev \
  ...
}
```

Configuration of **APT Repositories** ensuring up-to-date and complete package access and install a set of **development tools and libraries** with different purposes: Build tools, Graphics & X11 libs, Audio, GStreamer & multimedia codecs, Common dev libraries and Math/science libs.  

```dockerfile
WORKDIR /build

RUN tar czf rasp.tar.gz -C / lib usr/include usr/lib etc/alternatives
```

Sets the working directory for any following commands and packages key parts of the system into a compressed archive named rasp.tar.gz.  

`DockerFile (Cross Compiler):`  

```dockerfile
FROM debian:bookworm

ENV DEBIAN_FRONTEND=noninteractive

ARG BUILD_OPENCV=FALSE
```

Definition of the base image **Debian Bookworm** and environment setup by disabling **interactive prompts** during installation and declaration of a **optional build argument** that can be passed at build time to control whether OpenCV should be built.  

```dockerfile
RUN { \
  set -e && \
  apt-get update && apt-get install -y \
  wget git build-essential make rsync sed \
  libclang-dev ninja-build gcc bison python3 gperf pkg-config \
  ...
}
```

**Updates** the packages lists and **installs** a large number of **development tools and libraries** that cover: Core build tools, Compilers and parsers, Version control and utilities, Math & system libraries, Graphics/X11 libraries, Security & encryption, Database and data formats, General development libs, Build automation tools

```dockerfile
RUN ( \
    git clone https://github.com/Kitware/CMake.git && \
    ./bootstrap && make -j$(nproc) && make install \
)

RUN mkdir sysroot sysroot/usr sysroot/opt
COPY rasp.tar.gz /build/rasp.tar.gz
RUN tar xvfz /build/rasp.tar.gz -C /build/sysroot

RUN wget https://raw.githubusercontent.com/riscv/riscv-poky/master/scripts/sysroot-relativelinks.py && \
    python3 sysroot-relativelinks.py /build/sysroot
```

Clones and builds **CMake** directly from source, creates a sysroot (replica of the Raspberry Pi filesystem and used for cross-compilation) extracting the prebuilt Raspberry Pi environment, **rasp.tar.gz** into a directory.  
Lastly downloads a python script to **fix absolute symlinks** inside the sysroot, converting them to relative links so the toolchain reference them correctly.  

```dockerfile
COPY toolchain.cmake /build/

RUN { \
    mkdir -p qt6/... && \
    wget ...qtbase...qtshadertools...qtdeclarative... && \
    cmake ... && cmake --build ... && cmake --install ... \
}

RUN mkdir /build/project
COPY project /build/project
RUN { \
    cd /build/project && \
    /build/qt6/pi/bin/qt-cmake . && \
    cmake --build .; \
}
```

Copies the **Qt-specific CMake toolchain file** used to cross-compile for ARM, followed by the **download and build of Qt 6.9.1**, and lastly copies the project source code into the container, builds it using the **Qt for Raspberry toolchain** using the cross-built Qt binaries for consistent target behavior.  

---
<a id="sec-how-to-use"></a>
## 📝 How to use

1.  **Setup Docker**  
The first step is to correctly setup the base of the cross compiler, [Docker](https://www.docker.com/)  
2. **Create the target Docker image**  
On the `/cross-compiler` folder run the command to create the target image
```bash
docker buildx build --platform linux/arm64 --load -f DockerFileRasp -t raspimage .
```  
3. **Extract the compressed sysroot**  
After the image is created, extract from the image the `rasp.tar.gz`, that serves as the sysroot, which the cross-compiler will use to link against correct ARM libraries
```bash
docker create --name temp-arm raspimage
docker cp temp-arm:/build/rasp.tar.gz ./rasp.tar.gz
```  
4. **Create Debian image and Qt compilation**  
On the `/src` run the build of the Dockerfile
```bash
docker build -t qtcrossbuild -f cross-compiler/Dockerfile .
```  
5. **Extract the executable and binaries**
After the image is created (will take a couple hours) copy the executable and `qt-pi-binaries.tar.qz`
```bash
docker create --name tmpbuild qtcrossbuild
docker cp tmpbuild:/build/project/HelloQt6Qml ./HelloQt6Qml
docker cp tmpbuild:/build/qt-pi-binaries.tar.qz ./qt-pi-binaries.tar.qz
```  
6. **Send to Raspberry**  
After having all the necessary files send them to the raspberry via ssh  
7. **Config and execute**  
With the files already on the Rasp  
```bash
sudo mkdir /usr/local/qt6
sudo tar -xvf qt-pi-binaries.tar.gz -C /usr/local/qt6

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/qt6/lib/

./HelloQt6Qml
``` -->


<a id="sec-links"></a>

## 🔗 Links

  - https://ruvi-d.medium.com/a-master-guide-to-linux-cross-compiling-b894bf909386  
  - https://www.geeksforgeeks.org/compiler-design/what-is-cross-compiler/  
  - https://wiki.qt.io/Cross-Compile_Qt_6_for_Raspberry_Pi  
  - https://medium.com/@janiethedev/cross-compile-qt-6-7-2-for-raspberry-pi-4-with-ubuntu-1e951af83bb5  
  - https://github.com/PhysicsX/QTonRaspberryPi  
  - https://youtu.be/5XvQ_fLuBX0?si=WcI7c6L5jJSLzCvF  
  - https://www.docker.com/ 
  - https://learn.microsoft.com/en-us/dotnet/core/deploying/native-aot/cross-compile
