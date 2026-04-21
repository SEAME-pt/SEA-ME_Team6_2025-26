---
id: ASSERT_L0_31
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
  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_31/EXPECTATIONS-EXPECT_L0_31.md
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_31/EVIDENCES-EVID_L0_31.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: pending
---
