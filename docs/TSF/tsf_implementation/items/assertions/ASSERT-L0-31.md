---
id: ASSERT-L0-31
header: 'OTA Update System Implementation: The vehicle shall support SOTA, COTA, FOTA,
  and RAUC OS updates'
text: "The vehicle's comprehensive OTA update system has been verified to support:\n\
  - SOTA (Software OTA): Application and middleware updates\n- COTA (Configuration\
  \ OTA): Remote configuration changes\n- FOTA (Firmware OTA): ECU/STM32 firmware\
  \ updates\n- RAUC OS Updates: Safe, atomic system updates with A/B partition scheme\n\
  \n**Verification Method:** \n- Integration test: SOTA application update deployment\n\
  - Integration test: COTA configuration push and persistence\n- Integration test:\
  \ FOTA firmware flash to STM32/ECU\n- Integration test: RAUC bundle validation,\
  \ installation, slot switching\n- Health check execution verification\n- Rollback\
  \ verification on simulated failure\n- Analysis of logs from OTA scripts, RAUC,\
  \ bootloader, and health check scripts\n"
level: '1.31'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-31.md
- type: file
  path: ../evidences/EVID-L0-31.md
- type: url
  url: https://rauc.readthedocs.io/
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/src/ota
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: pending
evidence:
  type: validate_hardware_availability
  configuration:
    components:
    - SOTA Scripts
    - COTA Configuration
    - FOTA Firmware
    - RAUC
    - A/B Partitions
    - Bundle Validation
    - Slot Switching
    - Health Checks
    - Rollback Mechanism
    - Data Persistence
    tests:
    - sota_application_update
    - cota_config_push
    - fota_firmware_flash
    - rauc_bundle_creation
    - rauc_bundle_installation
    - slot_switching
    - health_check_pass
    - health_check_fail_rollback
    - data_persistence
---
