# AGL (Automotive Grade Linux) - Configuration Guide 

---

## Table of Contents

1. [Introduction](#introduction)
2. [Custom Meta-Layer (meta-seame)](#custom-meta-layer-meta-seame)
3. [local.conf Configuration](#localconf-configuration)
4. [Installed Components](#installed-components)
5. [Network Configuration](#network-configuration)
6. [U-Boot Bypass](#u-boot-bypass)
7. [Automatic WiFi Support](#automatic-wifi-support)
8. [USB/Joystick Support](#usbjoystick-support)
9. [Build and Deploy](#build-and-deploy)
10. [Troubleshooting](#troubleshooting)

---

## Introduction

This guide documents all custom configurations applied to AGL Terrific Trout 20.0.2 for the SEA:ME project on the Raspberry Pi 5. 
The base system used is **agl-image-minimal** with extensions to support:

- Weston/Wayland Compositor
- Qt6 for dashboard development
- Network connectivity (Ethernet + WiFi)
- USB device support (Joystick-PiRacer)
- CAN communication tools
- SSH for remote access
- Direct boot without U-Boot (solution for RPi5 boot issues)

---

## Custom Meta-Layer (meta-seame)

### Layer Structure

The custom layer `meta-seame` is created in the AGL workspace to add project-specific configurations:

```
$AGL_TOP/
├── meta-agl/
├── meta-agl-demo/
├── meta-agl-devel/
├── meta-seame/                    ← Custom layer
│   ├── conf/
│   │   └── layer.conf
│   ├── recipes-connectivity/
│   │   ├── network-config/
│   │   │   ├── files/
│   │   │   │   └── 20-wired.network
│   │   │   └── network-config_1.0.bb
│   │   └── wifi-startup/
│   │       ├── files/
│   │       │   ├── wifi-startup.sh
│   │       │   └── wifi-startup.service
│   │       └── wifi-startup_1.0.bb
│   ├── recipes-support/
│   │   └── joystick-support/
│   │       ├── files/
│   │       │   └── joystick-setup.sh
│   │       └── joystick-support.bb
│   └── recipes-bsp/
│       └── bootfiles/
│           └── rpi-config_git.bbappend
└── build-rpi5/
    └── conf/
        ├── local.conf
        └── bblayers.conf
```

### Creating the Layer

```bash
cd $AGL_TOP

# Create layer structure
mkdir -p meta-seame/conf
mkdir -p meta-seame/recipes-connectivity/network-config/files
mkdir -p meta-seame/recipes-connectivity/wifi-startup/files
mkdir -p meta-seame/recipes-support/joystick-support/files
```

### layer.conf

**File:** `meta-seame/conf/layer.conf`

```bitbake
# Layer configuration for meta-seame
BBPATH .= ":${LAYERDIR}"

BBFILES += "${LAYERDIR}/recipes*/*/*.bb \
            ${LAYERDIR}/recipes*/*/*.bbappend"

BBFILE_COLLECTIONS += "seame"
BBFILE_PATTERN_seame = "^${LAYERDIR}/"
BBFILE_PRIORITY_seame = "10"

LAYERDEPENDS_seame = "core"
LAYERSERIES_COMPAT_seame = "scarthgap"
```

**Note:** `LAYERSERIES_COMPAT` must be "scarthgap" for AGL 20.0.x (Yocto 5.0.x).

### Adding to the Build

```bash
cd $AGL_TOP/build-rpi5
bitbake-layers add-layer ../meta-seame

# Verify if it was added
bitbake-layers show-layers | grep seame
```

---

## local.conf Configuration

### Location

**File:** `$AGL_TOP/build-rpi5/conf/local.conf`

### Section 1: Machine and Distro

```bitbake
MACHINE = "raspberrypi5"
DISTRO = "poky-agl"

# Parallel build configuration
PARALLEL_MAKE = "-j 8"
BB_NUMBER_THREADS = "8"
```

### Section 2: Direct Boot without U-Boot (CRITICAL)

This configuration resolves the issue where the RPi5 hangs at U-Boot:

```bitbake
##############################################################################
# RASPBERRY PI 5 - DIRECT BOOT WITHOUT U-BOOT
##############################################################################

# CRITICAL: Disable U-Boot completely
RPI_USE_U_BOOT = "0"

# Enable UART for debug
ENABLE_UART = "1"
SERIAL_CONSOLES = "115200;ttyAMA0"

# Direct Kernel
KERNEL_IMAGETYPE = "Image"

# Force newer kernel with better RPi5 support
PREFERRED_VERSION_linux-raspberrypi = "6.6%"

# GPU Memory
GPU_MEM = "256"

# Device tree for RPi5
KERNEL_DEVICETREE:append = " broadcom/bcm2712-rpi-5-b.dtb"

# Extra configurations for config.txt on RPi5
RPI_EXTRA_CONFIG = " \n\
# Direct boot without U-Boot \n\
arm_64bit=1 \n\
kernel=Image \n\
disable_overscan=1 \n\
gpu_mem=256 \n\
enable_uart=1 \n\
uart_2ndstage=1 \n\
\n\
# Optimizations for RPi5 \n\
arm_boost=1 \n\
over_voltage_delta=50000 \n\
\n\
# Display \n\
dtoverlay=vc4-kms-v3d \n\
max_framebuffers=2 \n\
"

# Boot delay to allow system initialization time
RPI_EXTRA_CONFIG:append = " \n\
boot_delay=1 \n\
"
```

### Section 3: Weston/Wayland Compositor

```bitbake
##############################################################################
# WESTON/WAYLAND COMPOSITOR
##############################################################################

DISTRO_FEATURES:append = " wayland"
DISTRO_FEATURES:remove = " x11"

# Weston compositor packages
IMAGE_INSTALL:append = " \
    weston \
    weston-init \
    weston-examples \
    wayland \
    wayland-protocols \
    wayland-utils \
"
```

### Section 4: Qt6 Development

```bitbake
##############################################################################
# QT6 DEVELOPMENT ENVIRONMENT
##############################################################################

# Qt6 core packages
IMAGE_INSTALL:append = " \
    qtbase \
    qtdeclarative \
    qtwayland \
    qtquickcontrols2 \
    qtsvg \
    qtserialport \
"

# Qt6 development tools (optional - increases image size)
IMAGE_INSTALL:append = " \
    qtbase-dev \
    qtdeclarative-dev \
    cmake \
"

# GPU acceleration for RPi5
PACKAGECONFIG:append:pn-qtbase = " gles2 eglfs kms gbm"
```

### Section 5: Networking

```bitbake
##############################################################################
# NETWORK CONFIGURATION
##############################################################################

# Network tools
IMAGE_INSTALL:append = " \
    openssh \
    openssh-sshd \
    openssh-sftp-server \
    iproute2 \
    net-tools \
    iputils \
    wireless-regdb-static \
    wpa-supplicant \
"

# WiFi support
IMAGE_INSTALL:append = " \
    linux-firmware-bcm43455 \
    linux-firmware-rpidistro-bcm43455 \
"

# Enable systemd-networkd
PACKAGECONFIG:append:pn-systemd = " networkd resolved"
VIRTUAL-RUNTIME_net_manager = "systemd"
```

### Section 6: CAN Bus Support

```bitbake
##############################################################################
# CAN BUS SUPPORT
##############################################################################

# CAN utilities
IMAGE_INSTALL:append = " \
    can-utils \
    libsocketcan \
"

# Kernel modules CAN
KERNEL_MODULE_AUTOLOAD += "can can-raw can-dev mcp251x"
```

### Section 7: USB/Input Devices

```bitbake
##############################################################################
# USB AND INPUT DEVICES SUPPORT
##############################################################################

# USB HID support
DISTRO_FEATURES:append = " usbhost usbgadget"

# Joystick/Gamepad support
IMAGE_INSTALL:append = " \
    joystick \
    linuxconsoletools \
    evtest \
    libevdev \
"

# Kernel modules
KERNEL_MODULE_AUTOLOAD += "joydev usbhid hid_generic"
KERNEL_FEATURES += "cfg/hid.scc"
```

### Section 8: Development Tools

```bitbake
##############################################################################
# DEVELOPMENT TOOLS
##############################################################################

IMAGE_INSTALL:append = " \
    python3 \
    python3-pip \
    git \
    nano \
    vim \
    htop \
    iotop \
    lsof \
    strace \
"
```

### Section 9: System Monitoring

```bitbake
##############################################################################
# SYSTEM MONITORING
##############################################################################

IMAGE_INSTALL:append = " \
    lm-sensors \
    i2c-tools \
    spi-tools \
"
```

### Section 10: Custom Recipes

```bitbake
##############################################################################
# CUSTOM RECIPES FROM META-SEAME
##############################################################################

IMAGE_INSTALL:append = " \
    network-config \
    wifi-startup \
    joystick-support \
"
```

### Section 11: Storage and Filesystem

```bitbake
##############################################################################
# STORAGE AND FILESYSTEM
##############################################################################

# Extra space in rootfs (2GB)
IMAGE_ROOTFS_EXTRA_SPACE = "2097152"

# Support for additional filesystems
IMAGE_INSTALL:append = " \
    e2fsprogs \
    e2fsprogs-resize2fs \
    dosfstools \
    util-linux \
"
```

### Section 12: Final Optimizations

```bitbake
##############################################################################
# OPTIMIZATIONS
##############################################################################

# Remove unnecessary AGL demos
PACKAGE_EXCLUDE = "agl-demo-*"

# Enable useful systemd features
DISTRO_FEATURES:append = " systemd"
VIRTUAL-RUNTIME_init_manager = "systemd"
```

---

## Installed Components

### Base AGL Components

| Component | Description | Notes |
|------------|-----------|-------|
| **agl-image-minimal** | Base AGL Image | Core operating system |
| **Weston 13** | Wayland Compositor | DRM Backend + RDP |
| **systemd** | System and service manager | Init system |
| **ConnMan** | Connection Manager | Network manager (disabled in favor of systemd-networkd) |

### Added Components

#### Display & Graphics
- `weston` - Wayland Compositor
- `weston-init` - Startup scripts
- `weston-examples` - Wayland examples
- `wayland-protocols` - Wayland protocols
- `mesa-demos` - OpenGL demos (optional)

#### Qt6 Framework
- `qtbase` - Core Qt6
- `qtdeclarative` - QML engine
- `qtwayland` - Wayland Backend for Qt
- `qtquickcontrols2` - UI Controls
- `qtsvg` - SVG Support
- `qtserialport` - Serial communication

#### Networking
- `openssh` + `openssh-sshd` - SSH Server
- `iproute2` - Modern network tools
- `net-tools` - ifconfig, route, etc
- `wpa-supplicant` - WiFi Client
- `linux-firmware-bcm43455` - RPi5 WiFi Firmware

#### CAN Bus
- `can-utils` - CAN Tools (cansend, candump, etc)
- `libsocketcan` - SocketCAN library

#### USB/Input
- `joystick` - Joystick support
- `linuxconsoletools` - Calibration tools
- `evtest` - Input event tester

#### Development
- `python3` + `python3-pip`
- `git`
- `cmake`
- `nano`, `vim`
- `htop`, `iotop`

---

## Network Configuration

### Static Ethernet Configuration

#### Recipe: network-config

**File:** `meta-seame/recipes-connectivity/network-config/network-config_1.0.bb`

```bitbake
SUMMARY = "Static network configuration for SEA:ME"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://20-wired.network"

S = "${WORKDIR}"

do_install() {
    install -d ${D}${sysconfdir}/systemd/network
    install -m 0644 ${WORKDIR}/20-wired.network ${D}${sysconfdir}/systemd/network/
}

FILES:${PN} += "${sysconfdir}/systemd/network/20-wired.network"

RDEPENDS:${PN} = "systemd"
```

**File:** `meta-seame/recipes-connectivity/network-config/files/20-wired.network`

```ini
[Match]
Name=eth0

[Network]
Address=192.168.1.100/24
Gateway=192.168.1.1
DNS=8.8.8.8
DNS=8.8.4.4

[Link]
RequiredForOnline=yes
```

**Customization:** Adjust the IP (192.168.1.100) and gateway (192.168.1.1) according to your network.

### Automatic WiFi Configuration

#### Recipe: wifi-startup

**File:** `meta-seame/recipes-connectivity/wifi-startup/wifi-startup_1.0.bb`

```bitbake
SUMMARY = "WiFi Startup Script for SEA:ME AGL"
DESCRIPTION = "Automatically connects to WiFi on boot"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
    file://wifi-startup.sh \
    file://wifi-startup.service \
"

S = "${WORKDIR}"

inherit systemd

SYSTEMD_SERVICE:${PN} = "wifi-startup.service"
SYSTEMD_AUTO_ENABLE = "enable"

RDEPENDS:${PN} = "wpa-supplicant"

do_install() {
    # Install script
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/wifi-startup.sh ${D}${bindir}/

    # Install systemd service
    install -d ${D}${systemd_unitdir}/system
    install -m 0644 ${WORKDIR}/wifi-startup.service ${D}${systemd_unitdir}/system/
}

FILES:${PN} += "${systemd_unitdir}/system/wifi-startup.service"
```

**File:** `meta-seame/recipes-connectivity/wifi-startup/files/wifi-startup.sh`

```bash
#!/bin/sh

echo "[WiFi] Starting WiFi configuration..."

# Wait for interface to be available
sleep 5

# Bring up interface
ip link set wlan0 up
sleep 2

# Check if wpa_supplicant is already running
if pgrep -x "wpa_supplicant" > /dev/null; then
    echo "[WiFi] wpa_supplicant already running"
    killall wpa_supplicant
    sleep 1
fi

# Start wpa_supplicant
echo "[WiFi] Starting wpa_supplicant..."
wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf -D nl80211

# Wait for connection
sleep 5

# Request DHCP
echo "[WiFi] Requesting DHCP..."
udhcpc -i wlan0

# Verify connection
if ip addr show wlan0 | grep -q "inet "; then
    IP=$(ip addr show wlan0 | grep "inet " | awk '{print $2}')
    echo "[WiFi] Connected successfully! IP: $IP"
    exit 0
else
    echo "[WiFi] ERROR: Failed to obtain IP"
    exit 1
fi
```

**File:** `meta-seame/recipes-connectivity/wifi-startup/files/wifi-startup.service`

```ini
[Unit]
Description=WiFi Auto-Connect at Boot
After=network.target
Before=sshd.service ssh.service multi-user.target
Wants=network.target

[Service]
Type=oneshot
ExecStart=/usr/bin/wifi-startup.sh
RemainAfterExit=yes
StandardOutput=journal
StandardError=journal
Restart=on-failure
RestartSec=10

[Install]
WantedBy=multi-user.target
```

#### Configuring wpa_supplicant.conf

NOTE: This file must be manually created on the system after the initial boot:

```bash
# On Raspberry Pi, after boot:
wpa_passphrase "NetworkName" "Password" > /etc/wpa_supplicant.conf
```

Or created manually:

```bash
cat > /etc/wpa_supplicant.conf << 'EOF'
ctrl_interface=/var/run/wpa_supplicant
ctrl_interface_group=0
update_config=1
country=PT

network={
    ssid="NetworkName"
    psk="YourPasswordHere"
    key_mgmt=WPA-PSK
    priority=1
}
EOF

chmod 600 /etc/wpa_supplicant.conf
```

---

## U-Boot Bypass

### The Issue

The Raspberry Pi 5 has known issues with U-Boot on AGL Terrific Trout 20.0.2, resulting in a boot hang with the message "U-BOOT" on the screen.

### Implemented Solution

The solution is to perform a **direct boot of the Linux kernel**, bypassing U-Boot, by configuring the Raspberry Pi firmware to load the kernel directly.

### Configuration in local.conf

```bitbake
# CRITICAL: Disable U-Boot completely
RPI_USE_U_BOOT = "0"

# Direct Kernel
KERNEL_IMAGETYPE = "Image"

# Configurations for config.txt
RPI_EXTRA_CONFIG = " \n\
arm_64bit=1 \n\
kernel=Image \n\
..."
```

### Post-Boot Verification

```bash
# Check if booted without U-Boot
journalctl -b | grep -i "u-boot"  # Nothing should appear

# Confirm kernel booted directly
dmesg | head -20
```

### Manual Troubleshooting

If the problem persists, you can manually edit `config.txt` on the boot partition:

```bash
# Mount boot partition
sudo mount /dev/mmcblk0p1 /mnt

# Edit config.txt
sudo nano /mnt/config.txt

# Add/Verify:
arm_64bit=1
kernel=Image
disable_overscan=1
enable_uart=1
dtoverlay=vc4-kms-v3d

# Unmount
sudo umount /mnt
```

---

## Automatic WiFi Support

### Initialization Flow

1. **systemd** starts → `network.target`
2. `wifi-startup.service` is activated (Before=sshd.service)
3. Script `wifi-startup.sh` executes:
   - Activates interface `wlan0`
   - Starts `wpa_supplicant`
   - Requests IP via DHCP (udhcpc)
4. `sshd.service` starts (after WiFi is configured)

### Useful Commands

```bash
# Check WiFi service status
systemctl status wifi-startup.service

# Check logs
journalctl -u wifi-startup.service -f

# Restart WiFi
systemctl restart wifi-startup.service

# Check IP
ip addr show wlan0

# Test connectivity
ping -c 4 8.8.8.8
```

### Manual Configuration (Alternative)

If you prefer to configure manually without using the service:

```bash
# Activate interface
ip link set wlan0 up

# Connect with wpa_supplicant
wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf -D nl80211

# Obtain IP
udhcpc -i wlan0
```

---

## USB/Joystick Support

### Recipe: joystick-support

**File:** `meta-seame/recipes-support/joystick-support/joystick-support.bb`

```bitbake
SUMMARY = "Joystick support for PiRacer"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://joystick-setup.sh"

S = "${WORKDIR}"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/joystick-setup.sh ${D}${bindir}/
}

RDEPENDS:${PN} = "linuxconsoletools joystick"
```

**File:** `meta-seame/recipes-support/joystick-support/files/joystick-setup.sh`

```bash
#!/bin/sh

# Load necessary kernel modules
modprobe joydev
modprobe usbhid
modprobe hid_generic

# Set proper permissions
chmod 666 /dev/input/js*
chmod 666 /dev/input/event*

echo "Joystick setup complete"
ls -la /dev/input/js* 2>/dev/null || echo "No joystick devices found"
```

### Verification

```bash
# See USB devices
lsusb

# See input devices
ls -la /dev/input/

# Test joystick
evtest /dev/input/js0

# See events in real-time
cat /dev/input/js0 | hexdump -C
```

### Kernel Modules

The following modules are automatically loaded via `KERNEL_MODULE_AUTOLOAD`:

- `joydev` - Joystick support
- `usbhid` - USB HID devices
- `hid_generic` - Generic HID

---

## Build and Deploy

### Environment Preparation

```bash
cd $AGL_TOP
source meta-agl/scripts/aglsetup.sh -m raspberrypi5 -b build-rpi5 agl-demo agl-devel

# Add meta-seame
cd build-rpi5
bitbake-layers add-layer ../meta-seame

# Verify layers
bitbake-layers show-layers
```

### Initial Build (Clean Build)

```bash
cd $AGL_TOP/build-rpi5

# Complete build (takes 2-4 hours the first time)
bitbake agl-image-minimal
```

### Incremental Build

```bash
# After changes in local.conf or recipes
bitbake agl-image-minimal

# Clean cache of a specific recipe
bitbake -c cleansstate 
bitbake 
```

### Image Location

```bash
cd $AGL_TOP/build-rpi5/tmp/deploy/images/raspberrypi5/

ls -lh *.wic.xz
```

Relevant files:
- `agl-image-minimal-raspberrypi5.rootfs.wic.xz` - Compressed full image
- `agl-image-minimal-raspberrypi5.rootfs.manifest` - List of installed packages

### Deploy to MicroSD

#### Method 1: Using dd (Linux)

```bash
# Unmount existing partitions
sudo umount /dev/mmcblk0p1 /dev/mmcblk0p2

# Write image
cd $AGL_TOP/build-rpi5/tmp/deploy/images/raspberrypi5/
xzcat agl-image-minimal-raspberrypi5.rootfs.wic.xz | sudo dd of=/dev/mmcblk0 bs=4M status=progress

# Sync
sudo sync

# Eject
sudo eject /dev/mmcblk0
```

#### Method 2: Using bmaptool (Faster)

```bash
# Install bmaptool
sudo apt install bmap-tools

# Write with bmaptool
sudo bmaptool copy --bmap agl-image-minimal-raspberrypi5.rootfs.wic.bmap \
                   agl-image-minimal-raspberrypi5.rootfs.wic.xz \
                   /dev/mmcblk0
```

### First Boot

1. Insert MicroSD into Raspberry Pi 5
2. Connect HDMI monitor
3. Connect power (recommended 5V/5A with Official Raspberry Pi Charger)
4. Wait for boot (2-3 minutes on first boot)

#### Default Login

- **User:** `root`
- **Password:** (empty - press Enter)

---

## Troubleshooting

### Boot Stuck at U-Boot

**Symptom:** Black screen with text "U-BOOT" in the top corner.

**Solution:**
1. Verify `RPI_USE_U_BOOT = "0"` in `local.conf`
2. Verify presence of `RPI_EXTRA_CONFIG` with `kernel=Image`
3. Clean rebuild:
```bash
bitbake -c cleansstate linux-raspberrypi rpi-config
bitbake agl-image-minimal
```

### WiFi Does Not Connect

**Diagnosis:**
```bash
# Check service status
systemctl status wifi-startup.service

# Check logs
journalctl -u wifi-startup.service -n 50

# Check interface
ip link show wlan0

# Check firmware
ls /lib/firmware/brcm/

# Check wpa_supplicant.conf
cat /etc/wpa_supplicant.conf
```

**Solutions:**
1. Verify WiFi credentials in `/etc/wpa_supplicant.conf`

2. Test manual connection:
```bash
systemctl stop wifi-startup
wpa_supplicant -i wlan0 -c /etc/wpa_supplicant.conf -D nl80211
# (view logs in foreground)
```

3. Check status and stop and disable ConnMan
```sh
systemctl status connman
systemctl stop connman
systemctl disable connman
```

4. Connection Test
```sh
ping -c 4 8.8.8.8
```
If there is a response, the network is functional.

5. Create an Auto-Startup Script

To make Wi-Fi start automatically after login.
Create the script:
```sh
mkdir -p /opt/scripts
nano /opt/scripts/wifi-autostart.sh
```

Write this inside:
```sh
#!/bin/sh

sleep 5
ip link set wlan0 up
sleep 2
killall wpa_supplicant 2>/dev/null
sleep 1
wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf -D nl80211
sleep 5
udhcpc -i wlan0

```

Make it executable:
```sh
chmod +x /opt/scripts/wifi-autostart.sh
```

And create the service in systemd
```
nano /etc/systemd/system/wifi-autostart.service
```

and write this inside:
```
[Unit]
Description=WiFi Auto-Connect at Boot
After=network.target
Before=sshd.service ssh.service multi-user.target
Wants=network.target

[Service]
Type=oneshot
ExecStart=/opt/scripts/wifi-autostart.sh
RemainAfterExit=yes
StandardOutput=journal
StandardError=journal

# Restart if anything fails
Restart=on-failure
RestartSec=10

[Install]
WantedBy=multi-user.target
```

And finally, activate this service

```
# Reload services
systemctl daemon-reload

# Enable service
systemctl enable wifi-autostart.service

# Start service
systemctl start wifi-autostart.service

# Check status
systemctl status wifi-autostart.service

```
And that's it!

### SSH Not Working

**Diagnosis:**
```bash
# Check SSH status
systemctl status sshd

# Check if port 22 is listening
netstat -tlnp | grep :22

# Check logs
journalctl -u sshd -n 50
```

**Solutions:**
1. Start SSH manually:
```bash
systemctl start sshd
systemctl enable sshd
```

2. Check firewall (if applicable):
```bash
iptables -L -n
```

### Weston Does Not Start

**Diagnosis:**
```bash
# Check Weston status
systemctl status weston

# Check logs
journalctl -u weston -n 100

# Test manually
weston --backend=drm-backend.so
```

**Solutions:**
1. Check DRM permissions:
```bash
ls -la /dev/dri/
# Should have card0 and renderD128
```

2. Check video group:
```bash
groups root
# Should include 'video'
```

### Qt Application Does Not Run

**Diagnosis:**
```bash
# Check installed Qt packages
opkg list-installed | grep qt

# Test simple app
QT_QPA_PLATFORM=wayland /path/to/app

# Check environment variables
echo $QT_QPA_PLATFORM
echo $WAYLAND_DISPLAY
```

**Solutions:**
1. Export necessary variables:
```bash
export QT_QPA_PLATFORM=wayland
export WAYLAND_DISPLAY=wayland-0
```

2. Check compositor:
```bash
ps aux | grep weston
```

### Joystick Not Detected (Please check the AGL_Joystick Installation and test Guide for more info)

**Diagnosis:**
```bash
# See USB devices
lsusb

# See input devices
ls -la /dev/input/

# Check modules
lsmod | grep -E "joydev|usbhid"

# See USB logs
dmesg | grep -i usb
```

**Solutions:**
1. Load modules manually:
```bash
modprobe joydev
modprobe usbhid
```

2. Check udev rules:
```bash
udevadm info -a /dev/input/js0
```

### Performance Issues

**Diagnosis:**
```bash
# Monitor CPU/RAM
htop

# Check temperature
vcgencmd measure_temp

# Check clock
vcgencmd measure_clock arm

# Check throttling
vcgencmd get_throttled
```

**Solutions:**
1. Check adequate power supply (5V/5A for RPi5)
2. Add heatsink/fan
3. Reduce `GPU_MEM` if not using heavy graphics
4. Check `arm_boost=1` in `config.txt`

### Build Errors

**Error:** "Layer seame is not compatible with the core layer"

**Solution:** Check `LAYERSERIES_COMPAT_seame = "scarthgap"` in `layer.conf`

---

**Error:** "Nothing PROVIDES 'package-name'"

**Solution:** 1. Check correct package name:
```bash
bitbake -s | grep package-name
```

2. Add layer that provides the package:
```bash
bitbake-layers show-recipes package-name
```

---

**Error:** Task failed (do_compile, do_install, etc)

**Solution:**
1. See detailed log:
```bash
bitbake -c compile package-name -f
# View log in tmp/work/...
```

2. Clean and rebuild:
```bash
bitbake -c cleansstate package-name
bitbake package-name
```

### Network Issues

**Ethernet does not obtain IP:**

```bash
# Check link
ip link show eth0

# Check systemd-networkd configuration
cat /etc/systemd/network/20-wired.network

# Restart networkd
systemctl restart systemd-networkd

# Manual DHCP request (if using DHCP)
dhclient eth0
```

---

## References

### AGL Documentation
- [AGL Documentation](https://docs.automotivelinux.org/)
- [AGL Release Notes - Terrific Trout](https://docs.automotivelinux.org/en/trout/#00_Getting_Started/)
- [Yocto Project Quick Build](https://docs.yoctoproject.org/brief-yoctoprojectqs/index.html)

### Hardware
- [Raspberry Pi 5 Documentation](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html)
- [PiRacer Assembly Guide](https://www.waveshare.com/wiki/PiRacer_AI_Kit)

---
