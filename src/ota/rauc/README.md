# RAUC A/B Update System - Phase D

## Overview

RAUC (Robust Auto-Update Controller) enables atomic A/B partition updates for the AGL rootfs.

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     SD Card Layout                          │
├─────────────────────────────────────────────────────────────┤
│  mmcblk0p1  │  /boot     │  333 MB  │  Boot partition       │
│  mmcblk0p2  │  /         │  5.2 GB  │  rootfs-A (active)    │
│  mmcblk0p3  │  (unused)  │  4.0 GB  │  rootfs-B (standby)   │
│  mmcblk0p4  │  /data     │  512 MB  │  Persistent data      │
└─────────────────────────────────────────────────────────────┘
```

## How A/B Updates Work

1. **Current state**: System boots from rootfs-A (`/dev/mmcblk0p2`)
2. **Install update**: RAUC writes new image to rootfs-B (`/dev/mmcblk0p3`)
3. **Switch boot**: Post-install handler updates `/boot/cmdline.txt` to point to rootfs-B
4. **Reboot**: System boots from rootfs-B
5. **Rollback**: If boot fails, manually change `cmdline.txt` back to rootfs-A

## Files

| File | Description |
|------|-------------|
| `system.conf.rpi4` | RAUC config for Raspberry Pi 4 (32-bit) |
| `system.conf.rpi5` | RAUC config for Raspberry Pi 5 (64-bit) |
| `post-install.sh` | Handler to update boot configuration |
| `ca.cert.pem` | Certificate for verifying bundles |
| `dev-cert.pem` | Development signing certificate |
| `dev-key.pem` | Development signing key (KEEP SECRET!) |
| `setup-rauc.sh` | Setup script to run on devices |
| `create-bundle.sh` | Script to create RAUC bundles |

## Setup on Device

```bash
# Copy files to device
scp -r src/ota/rauc root@<IP>:/tmp/

# SSH to device and run setup
ssh root@<IP>
cd /tmp/rauc
chmod +x setup-rauc.sh
./setup-rauc.sh
```

## Creating a Bundle

```bash
# On your development machine (requires rauc installed)
cd src/ota/rauc
chmod +x create-bundle.sh
./create-bundle.sh rpi4 v1.10.0
```

## Installing a Bundle

```bash
# Copy bundle to device
scp bundles/update-rpi4-v1.10.0.raucb root@<IP>:/tmp/

# Install on device
ssh root@<IP> "rauc install /tmp/update-rpi4-v1.10.0.raucb"

# Reboot to activate
ssh root@<IP> "reboot"
```

## RAUC Commands

```bash
# Check RAUC status
rauc status

# Show slot information
rauc status --detailed

# Install a bundle
rauc install /path/to/bundle.raucb

# Show bundle info
rauc info /path/to/bundle.raucb

# Mark current slot as good (after successful boot)
rauc status mark-good
```

## Rollback

If the new rootfs fails to boot:

1. Connect via serial console or physically access the device
2. Mount boot partition: `mount /dev/mmcblk0p1 /boot`
3. Edit `/boot/cmdline.txt`
4. Change `root=/dev/mmcblk0p3` back to `root=/dev/mmcblk0p2`
5. Reboot

## Integration with Current OTA

RAUC A/B updates complement our existing OTA system:

| Feature | Current OTA (Phase C) | RAUC (Phase D) |
|---------|----------------------|----------------|
| **Scope** | Application binaries | Full rootfs |
| **Downtime** | ~6 seconds | Reboot required |
| **Rollback** | Version directory | A/B partition |
| **Risk** | Low (apps only) | Medium (full system) |
| **Use case** | Frequent updates | Major releases |

## Status

- [x] RAUC installed on both devices (v1.15.1)
- [x] Partitions prepared (rootfs-A and rootfs-B)
- [x] Configuration files created
- [x] Certificates generated
- [ ] Setup script tested on devices
- [ ] Bundle creation tested
- [ ] A/B switch tested
- [ ] Rollback tested

## Security Notes

⚠️ **Important**: The `dev-key.pem` file is a development key. For production:
1. Generate new keys on a secure machine
2. Never commit private keys to git
3. Use HSM or secure key storage
4. Rotate keys periodically

---

*Sprint 8 - Phase D - SEA:ME Team 6*
