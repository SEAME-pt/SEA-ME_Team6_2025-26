# AGL (Automotive Grade Linux) - Installation Guide

## Raspberry Pi 5 Build Instructions

This guide provides comprehensive instructions for building and deploying Automotive Grade Linux (AGL) on Raspberry Pi 5 using the Yocto Project build system.

---

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Git Configuration](#git-configuration)
3. [Build Environment Setup](#build-environment-setup)
4. [Source Code Download](#source-code-download)
5. [Raspberry Pi 5 Configuration](#raspberry-pi-5-configuration)
6. [Build Optimization](#build-optimization)
7. [Available Build Targets](#available-build-targets)
8. [Building the Image](#building-the-image)
9. [Flashing to microSD Card](#flashing-to-microsd-card)
10. [Hardware Setup and Boot](#hardware-setup-and-boot)

---

## Prerequisites

Install the required dependencies on your Ubuntu/Debian-based system:

```bash
sudo apt-get update
sudo apt-get install -y repo gawk wget git diffstat unzip texinfo \
    gcc-multilib build-essential chrpath socat cpio python3 python3-pip \
    python3-pexpect xz-utils debianutils iputils-ping python3-git \
    python3-jinja2 libegl1-mesa libsdl1.2-dev pylint3 xterm \
    python3-subunit mesa-common-dev zstd liblz4-tool curl
```





# 🎮 Waveshare Pyracer Controller Setup (via SSH)

### This guide provides a solution for installing the Waveshare Pyracer controller (or similar joysticks) via SSH.

### NOTE: This method focuses on loading the necessary kernel modules at runtime using modprobe, which avoids the need to install new packages or perform a system rebuild.

## 1. Load Required Kernel Modules

First, we must manually load the kernel modules required for joystick and USB Human Interface Device (HID) support.
Bash
```
sudo modprobe joydev
sudo modprobe usbhid
sudo modprobe hid_generic
```
## 2. Verify Modules are Loaded

After running the commands, check that the modules were loaded correctly into the kernel.
Bash
``` 
lsmod | grep -E "joydev|usbhid"
```
You should see output indicating that joydev and usbhid are active.

## 3. Connect the USB Dongle

At this point, connect the controller's USB dongle to the device.

    Important Note: If the dongle was already plugged in before you loaded the modules, disconnect it and plug it back in now. This forces the kernel to recognize the device using the newly loaded drivers.

## 4. Verify USB Device Detection

You can now confirm that the system has detected the dongle.

Use dmesg to see the most recent kernel messages, which should show the new USB device being registered:
```
dmesg | tail -30
```

Alternatively, you can list all connected USB devices:
```
lsusb
```

## 5. Test the Controller Input

Once the drivers are loaded and the dongle is detected, the system should create a device file (usually /dev/input/js0).

You can read the raw event data from the controller to confirm it is working. Run the following command and then press buttons or move the sticks on your controller:
```
sudo hexdump -C /dev/input/js0
```

You should see hexadecimal output appearing on your screen in real-time as you use the controller. This confirms the device is sending data.

    Press Ctrl+C to exit the hexdump command.



And...
that's it!
