# RAUC Integration in AGL
### Yocto Build System & meta-seame Layer
*SEA:ME Team 6 · Module 02 – ADAS · March 2026*

---

## 1. Overview

This document covers the integration of RAUC (Robust Auto-Update Controller) into the AGL Yocto build system for the SEA:ME Team 6 platform. It focuses on what was added to the `meta-seame` custom layer to produce images and bundles with A/B partition update support.

> **Note:** Runtime behaviour — polling timer, `ota-update.sh`, rollback logs — is documented in `OTA_Implementation_Guide.md`. This document covers the Yocto build level only.

At build time, RAUC integration provides:

- The `rauc` daemon and CLI compiled into the AGL image.
- A `system.conf` describing the partition layout and the certificate for bundle verification.
- A bundle recipe (`rauc-bundle.bb`) that packages the rootfs into a signed `.raucb` file.
- A custom bootloader backend script for the RPi5 native firmware (no U-Boot).

---

## 2. SD Card Partition Layout

The A/B scheme requires two rootfs slots. The layout adopted for Team 6 is:

| Partition | Device | Mount | Size | Role |
|-----------|--------|-------|------|------|
| p1 | `/dev/mmcblk0p1` | `/boot` | 333 MB | Boot files, config.txt, overlays |
| p2 | `/dev/mmcblk0p2` | `/` | 5.2 GB | rootfs slot A — active |
| p3 | `/dev/mmcblk0p3` | *(none)* | 4.0 GB | rootfs slot B — standby |
| p4 | `/dev/mmcblk0p4` | `/data` | 512 MB | Persistent data — survives updates |

The `/data` partition (p4) is never touched by RAUC. All runtime configuration in `/data/seame-configs/` survives A/B slot switches.

> **Important:** RAUC only writes to the inactive slot. The running system is never modified during an install. The switch happens at the next reboot via bootloader flags.

---

## 3. Yocto Recipe Additions in meta-seame

### 3.1 File inventory

| File | Location in meta-seame | Purpose |
|------|------------------------|---------|
| `rauc-bundle.bb` | `recipes-core/rauc-bundle/` | Creates the `.raucb` update bundle |
| `rauc_%.bbappend` | `recipes-core/rauc/` | Installs `system.conf` and backend scripts into the image |
| `system.conf` | `recipes-core/rauc/files/` | Partition mapping and certificate path |
| `bootloader-custom-backend.sh` | `recipes-core/rauc/files/` | Slot switching for RPi5 native bootloader |
| `post-install.sh` | `recipes-core/rauc/files/` | Hook run after writing the new rootfs |
| `ca.cert.pem` | `recipes-core/rauc/files/` | CA certificate for bundle verification (deployed to device) |
| `agl-image-minimal.bbappend` | `recipes-core/images/` | Adds `rauc` and `rauc-mark-good` to the image |

### 3.2 agl-image-minimal.bbappend

```bitbake
# meta-seame/recipes-core/images/agl-image-minimal.bbappend
IMAGE_INSTALL:append = " \
    rauc \
    rauc-mark-good \
"
```

`rauc-mark-good` is a service from `meta-rauc` that marks the current slot as good after a successful boot, preventing automatic rollback on subsequent reboots.

### 3.3 rauc_%.bbappend

```bitbake
# meta-seame/recipes-core/rauc/rauc_%.bbappend
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " \
    file://system.conf \
    file://ca.cert.pem \
    file://bootloader-custom-backend.sh \
    file://post-install.sh \
"

do_install:append() {
    install -d ${D}${sysconfdir}/rauc
    install -m 0644 ${WORKDIR}/system.conf   ${D}${sysconfdir}/rauc/system.conf
    install -m 0644 ${WORKDIR}/ca.cert.pem   ${D}${sysconfdir}/rauc/ca.cert.pem
    install -d ${D}${libdir}/rauc
    install -m 0755 ${WORKDIR}/bootloader-custom-backend.sh \
        ${D}${libdir}/rauc/bootloader-custom-backend.sh
    install -m 0755 ${WORKDIR}/post-install.sh \
        ${D}${libdir}/rauc/post-install.sh
}
```

### 3.4 system.conf

Describes the hardware layout to the RAUC daemon. The `compatible` string must match exactly the value in the bundle recipe.

```ini
# meta-seame/recipes-core/rauc/files/system.conf
[system]
compatible=seame-team6-rpi5
bootloader=custom
custom-backend=/usr/lib/rauc/bootloader-custom-backend.sh

[keyring]
path=/etc/rauc/ca.cert.pem

[slot.rootfs.0]
device=/dev/mmcblk0p2
type=ext4
bootname=A

[slot.rootfs.1]
device=/dev/mmcblk0p3
type=ext4
bootname=B
```

> **Important:** The `compatible` string (`seame-team6-rpi5`) must match the `RAUC_BUNDLE_COMPATIBLE` field in `rauc-bundle.bb` byte for byte. A mismatch causes RAUC to reject the bundle before installation.

### 3.5 bootloader-custom-backend.sh

The RPi5 uses its native firmware bootloader (not U-Boot), so RAUC's standard backends do not apply. A custom backend script is required. It implements three commands called by the RAUC daemon:

| Command | Action |
|---------|--------|
| `get-primary` | Returns the bootname (A or B) of the currently active slot |
| `set-primary <n>` | Writes the desired boot slot to `/data/rauc-boot-state` so it is selected on next reboot |
| `mark-bad <n>` | Records the failed slot; called by RAUC during automatic rollback |

```bash
#!/bin/bash
# /usr/lib/rauc/bootloader-custom-backend.sh
STATE_FILE="/data/rauc-boot-state"

case "$1" in
  get-primary)  cat "${STATE_FILE}" 2>/dev/null || echo "A" ;;
  set-primary)  echo "$2" > "${STATE_FILE}" ;;
  mark-bad)     echo "BAD:$2" >> "${STATE_FILE}.log" ;;
esac
```

> **Note:** The state file lives in `/data/` (the persistent p4 partition), so it is accessible from both slot A and slot B and survives rootfs updates.

### 3.6 post-install.sh

Called by RAUC after writing the new rootfs to the inactive slot, before rebooting. Used to restore persistent configuration into the new slot:

```bash
#!/bin/bash
# RAUC sets RAUC_SLOT_MOUNT_POINT to the newly written slot mount point
if [ -f /data/seame-configs/restore-seame-config.sh ]; then
    /data/seame-configs/restore-seame-config.sh "${RAUC_SLOT_MOUNT_POINT}"
fi
exit 0
```

---

## 4. Bundle Recipe (rauc-bundle.bb)

Instructs BitBake to produce a signed `.raucb` file from the compiled rootfs image.

> **Note:** In Yocto Scarthgap the bundle class lives in `classes-recipe/`, not `classes/`. Use `inherit bundle` without a path and ensure `meta-rauc` is on the `scarthgap` branch.

```bitbake
# meta-seame/recipes-core/rauc-bundle/rauc-bundle.bb
SUMMARY = "RAUC update bundle for SEA:ME Team 6 RPi5"
LICENSE = "MIT"

inherit bundle

RAUC_BUNDLE_COMPATIBLE = "seame-team6-rpi5"
RAUC_BUNDLE_VERSION    = "${PV}"

RAUC_BUNDLE_SLOTS        = "rootfs"
RAUC_SLOT_rootfs         = "agl-image-minimal"
RAUC_SLOT_rootfs[fstype] = "ext4"
RAUC_SLOT_rootfs[type]   = "image"

RAUC_KEY_FILE  = "${THISDIR}/files/dev-key.pem"
RAUC_CERT_FILE = "${THISDIR}/files/dev-cert.pem"
```

The output bundle is placed in:

```
tmp/deploy/images/raspberrypi5/seame-team6-rpi5-<version>.raucb
```

### 4.1 Certificate setup

Certificates are generated once and stored in the repo (public certs only). The CA private key must never be committed.

```bash
# Generate self-signed CA
openssl req -x509 -newkey rsa:4096 -keyout ca-key.pem \
    -out ca.cert.pem -days 3650 -nodes -subj "/CN=SEA-ME-Team6-CA"

# Generate dev signing key + CSR
openssl req -newkey rsa:4096 -keyout dev-key.pem \
    -out dev-csr.pem -nodes -subj "/CN=SEA-ME-Team6-Dev"

# Sign dev cert with CA
openssl x509 -req -in dev-csr.pem -CA ca.cert.pem \
    -CAkey ca-key.pem -CAcreateserial -out dev-cert.pem -days 1825
```

| File | Committed to repo? | Deployed to device? | Purpose |
|------|--------------------|---------------------|---------|
| `ca.cert.pem` | Yes | Yes — `/etc/rauc/` | RAUC daemon uses this to verify bundles |
| `dev-cert.pem` | Yes | No — build host only | Bundle recipe uses this to sign `.raucb` files |
| `dev-key.pem` | **No** — `.gitignore` | No | Private key for signing; never leave build host |
| `ca-key.pem` | **No** — `.gitignore` | No | CA private key; never commit |

---

## 5. Installing a Bundle on the Device

Once the device is running an AGL image with RAUC installed, push and apply a bundle over SSH:

```bash
# Copy bundle from build machine to device
scp tmp/deploy/images/raspberrypi5/seame-team6-rpi5-*.raucb \
    root@10.21.220.192:/tmp/update.raucb

# Install the bundle (writes to inactive slot)
ssh root@10.21.220.192 "rauc install /tmp/update.raucb"

# Reboot to activate the new slot
ssh root@10.21.220.192 "reboot"

# After reboot — verify slot state
ssh root@10.21.220.192 "rauc status"
```

Expected `rauc status` output after a successful install and reboot (slot B now active):

```
=== System Info ===
Compatible:  seame-team6-rpi5
Booted from: rootfs.1 (/dev/mmcblk0p3)

=== Slot States ===
x [rootfs.1] (/dev/mmcblk0p3, ext4, booted)
      bootname: B  |  boot status: good

o [rootfs.0] (/dev/mmcblk0p2, ext4, inactive)
      bootname: A  |  boot status: good
```

---

## 6. Known Issues and Lessons Learned

| Issue | Root cause | Fix |
|-------|------------|-----|
| `bundle.bbclass` not found during bitbake parse | In Scarthgap the class moved from `classes/` to `classes-recipe/` | Use `inherit bundle` without path. Ensure `meta-rauc` is on the `scarthgap` branch. |
| RAUC rejects bundle — compatible mismatch | `RAUC_BUNDLE_COMPATIBLE` in `rauc-bundle.bb` does not match `compatible=` in `system.conf` | Both strings must be identical. Check for trailing whitespace. |
| Slot B shows `bad` after first boot | `rauc-mark-good` service did not run (missing `WantedBy` or wrong `After=` chain) | Verify `rauc-mark-good.service` is enabled and its ordering is correct. |
| `post-install.sh` not executed — permission denied | `install -m 0644` used instead of `0755` in `do_install` | Use `install -m 0755` for all shell scripts. |

---

## 7. RAUC vs tar.gz OTA Scripts

| Dimension | tar.gz OTA scripts | RAUC (.raucb) |
|-----------|--------------------|---------------|
| Scope | Application binaries only | Complete rootfs image |
| Build involvement | None — built by GitHub Actions CI | Full Yocto build required |
| Package size | 260 KB – 4.6 MB | 1 – 5 GB |
| Downtime | ~6 s (hot service restart) | 30 – 60 s (full reboot) |
| Rollback | Atomic symlink to previous version | A/B partition switch at boot level |
| Cryptographic check | SHA256 of tar.gz | X.509 signed bundle |
| Typical frequency | Multiple times per week | Major releases only |

Both mechanisms coexist without conflict. RAUC manages rootfs partitions; the tar.gz scripts manage files within the running rootfs under `/opt/ota/` and `/opt/cluster/`.

---

*Document version 1.1 · SEA:ME Team 6 · March 2026*
