---
id: EXPECT_L0_31
header: 'OTA Update System Implementation: The vehicle shall support Over-The-Air
  (OTA) software updates using OTA Scripts for SOTA, COTA, and FOTA, plus RAUC with
  A/B partition scheme for OS updates'
text: "The vehicle shall support comprehensive Over-The-Air (OTA) software updates\
  \ covering:\n\n- **SOTA (Software Over-The-Air):** Application and middleware updates\
  \ via OTA scripts\n- **COTA (Configuration Over-The-Air):** Remote configuration\
  \ changes and parameter updates\n- **FOTA (Firmware Over-The-Air):** Low-level firmware\
  \ updates for ECUs and microcontrollers\n- **OS Updates:** Full system image updates\
  \ using RAUC with A/B partition scheme for safe, atomic u\n\n\n\n\n**Justification:**\
  \ Modern automotive systems require reliable OTA update mechanisms to deploy security\
  \ patches, bug fixes, and feature updates without physical access. A layered OTA\
  \ approach (SOTA/COTA/FOTA) allows granular updates while RAUC provides atomic OS\
  \ updates with rollback capability, essential for safety-critical automotive applications.\n\
  \n**References:** \n- RAUC documentation (https://rauc.readthedocs.io/)\n- Automotive\
  \ OTA best practices (AUTOSAR, ISO 24089)\n- AGL OTA update guidelines\n- UNECE\
  \ WP.29 Regulation on Software Updates\n\n**Acceptance Criteria:**\n1. **SOTA:**\
  \ The system shall update application software without requiring full OS reinstallation\n\
  2. **COTA:** The system shall apply configuration changes remotely and persist them\
  \ across reboots\n3. **FOTA:** The system shall update STM32/ECU firmware via CAN\
  \ or other interfaces\n4. **RAUC OS Updates:**\n   - Receive and validate RAUC bundles\
  \ (.raucb) over network\n   - Install updates atomically to inactive rootfs partition\n\
  \   - Automatically switch boot slot after successful installation\n   - Perform\
  \ post-reboot health checks (network, disk, services)\n   - Rollback to previous\
  \ version if health checks fail within 3 boot attempts\n5. Preserve user data in\
  \ /data partition across all update types\n6. Provide update status and progress\
  \ feedback\n\n**Verification Method:** \n- Integration test: SOTA script execution\
  \ and application update verification\n- Integration test: COTA configuration push\
  \ and persistence validation\n- Integration test: FOTA firmware flash and ECU verification\n\
  - Integration test: RAUC bundle validation, installation, slot switching\n- Health\
  \ check execution verification\n- Rollback verification on simulated failure\n-\
  \ Logs from OTA scripts, RAUC, bootloader, and health check scripts\n"
level: '1.31'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_31/ASSERTIONS-ASSERT_L0_31.md
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_31/EVIDENCES-EVID_L0_31.md
- type: file
  path: docs/guides/OTA/OTA_Implementation_Guide.md
- type: file
  path: src/ota/README.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: pending
---
