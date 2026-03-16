---
id: EXPECT_L0_31
header: "OTA Update System Implementation: The vehicle shall support Over-The-Air (OTA) software updates using OTA Scripts for SOTA, COTA, and FOTA, plus RAUC with A/B partition scheme for OS updates"
text: |
  The vehicle shall support comprehensive Over-The-Air (OTA) software updates covering:
  
  - **SOTA (Software Over-The-Air):** Application and middleware updates via OTA scripts
  - **COTA (Configuration Over-The-Air):** Remote configuration changes and parameter updates
  - **FOTA (Firmware Over-The-Air):** Low-level firmware updates for ECUs and microcontrollers
  - **OS Updates:** Full system image updates using RAUC with A/B partition scheme for safe, atomic updates

  **Justification:** Modern automotive systems require reliable OTA update mechanisms to deploy security patches, bug fixes, and feature updates without physical access. A layered OTA approach (SOTA/COTA/FOTA) allows granular updates while RAUC provides atomic OS updates with rollback capability, essential for safety-critical automotive applications.

  **References:** 
  - RAUC documentation (https://rauc.readthedocs.io/)
  - Automotive OTA best practices (AUTOSAR, ISO 24089)
  - AGL OTA update guidelines
  - UNECE WP.29 Regulation on Software Updates

  **Acceptance Criteria:**
  1. **SOTA:** The system shall update application software without requiring full OS reinstallation
  2. **COTA:** The system shall apply configuration changes remotely and persist them across reboots
  3. **FOTA:** The system shall update STM32/ECU firmware via CAN or other interfaces
  4. **RAUC OS Updates:**
     - Receive and validate RAUC bundles (.raucb) over network
     - Install updates atomically to inactive rootfs partition
     - Automatically switch boot slot after successful installation
     - Perform post-reboot health checks (network, disk, services)
     - Rollback to previous version if health checks fail within 3 boot attempts
  5. Preserve user data in /data partition across all update types
  6. Provide update status and progress feedback

  **Verification Method:** 
  - Integration test: SOTA script execution and application update verification
  - Integration test: COTA configuration push and persistence validation
  - Integration test: FOTA firmware flash and ECU verification
  - Integration test: RAUC bundle validation, installation, slot switching
  - Health check execution verification
  - Rollback verification on simulated failure
  - Logs from OTA scripts, RAUC, bootloader, and health check scripts
level: '1.31'
normative: true
references:
- type: file
  path: ../assertions/ASSERT-L0-31.md
- type: file
  path: ../evidences/EVID-L0-31.md
- type: url
  url: https://rauc.readthedocs.io/
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/src/ota/rauc
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: pending
---
