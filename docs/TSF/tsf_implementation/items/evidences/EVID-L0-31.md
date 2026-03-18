---
id: EVID-L0-31
header: OTA Update System Implementation - Evidence (SOTA, COTA, FOTA, RAUC)
text: "Evidence demonstrating that the comprehensive OTA update system is implemented\
  \ and functional, covering SOTA, COTA, FOTA, and RAUC OS updates.\n\n**Verification\
  \ Method:** \n- Integration test: SOTA application updates\n- Integration test:\
  \ COTA configuration changes\n- Integration test: FOTA firmware updates\n- Integration\
  \ test: RAUC bundle validation, installation, slot switching\n- Health check execution\
  \ verification\n- Rollback verification on simulated failure\n- Logs from OTA scripts,\
  \ RAUC, bootloader, and health check scripts\n\n**Expected Artifacts:**\n\n**SOTA\
  \ (Software OTA):**\n- Application update scripts\n- Package management configuration\n\
  - Update verification logs\n\n**COTA (Configuration OTA):**\n- Configuration management\
  \ scripts\n- Parameter files and templates\n- Configuration persistence validation\n\
  \n**FOTA (Firmware OTA):**\n- STM32/ECU firmware update scripts\n- CAN-based firmware\
  \ flash utilities\n- Firmware version verification logs\n\n**RAUC OS Updates:**\n\
  - RAUC configuration files (system.conf)\n- RAUC bundle creation scripts and signed\
  \ bundles (.raucb)\n- Installation script (install-bundle.sh)\n- Post-reboot health\
  \ check script (post-reboot-verify.sh)\n- Test logs showing successful bundle installation\n\
  - Test logs showing automatic slot switching\n- Test logs showing rollback on failure\
  \ scenario\n- Documentation of A/B partition layout\n- Evidence of data persistence\
  \ across updates\n"
level: '1.31'
normative: true
references:
- type: file
  path: src/ota/rauc/install-bundle.sh
- type: file
  path: src/ota/rauc/install-bundle.sh
- type: file
  path: src/ota/rauc/post-reboot-verify.sh
- type: file
  path: docs/guides/OTA/OTA_Implementation_Guide.md
- type: file
  path: docs/demos/OTA-Action.png
- type: file
  path: docs/demos/OTA-Tags.png
- type: file
  path: docs/demos/OTA-Release.png
- type: file
  path: docs/demos/OTA-r4-script.jpeg
- type: file
  path: docs/demos/OTA-r5-script.jpeg
- type: file
  path: docs/demos/assembled-car2.jpeg
- type: file
  path: docs/demos/3d-car.jpg
- type: file
  path: docs/demos/car_architecture_new_2 - sprint6.jpg
- type: file
  path: docs/demos/car_architecture_old - sprint6.jpeg
- type: file
  path: docs/demos/CAN Scheme - New update.png
- type: file
  path: docs/demos/crosscompiling_scheme-sprint6.jpeg
- type: file
  path: docs/demos/car_architecture_new_1 - sprint6.jpg
- type: file
  path: docs/demos/Validation-of-energy-layout-sprint0.jpeg
- type: file
  path: docs/demos/3d-car2.jpg
- type: file
  path: docs/demos/system-architecture-cross.png
- type: file
  path: docs/demos/Validation-of-energy-layout-sprint3.jpeg
- type: file
  path: docs/demos/3d-car3.jpg
- type: file
  path: docs/demos/CAN-layout-sprint3.jpeg
- type: file
  path: docs/demos/assembled-car1.jpeg
- type: file
  path: docs/demos/eletric-scheme.png
- type: file
  path: docs/demos/Validation-of-energy-layout-sprint2.jpeg
- type: file
  path: docs/demos/CANbus Scheme Updated.png
- type: file
  path: docs/demos/Validation-of-energy-layout-sprint1.jpeg
- type: file
  path: docs/guides/AGL_and_ThreadX_benefits_Guide.md
- type: file
  path: docs/guides/AGL_Installation_Guide.md
- type: file
  path: docs/guides/AGL_ Joystick_installation_and_test.md
- type: file
  path: docs/guides/AGL_Configuration_Guide.md
- type: file
  path: docs/guides/OTA/OTA_Presentation-sprint8.md
- type: file
  path: docs/guides/OTA/OTA_Comparison_Tests.md
- type: file
  path: docs/guides/OTA/OTA_Implementation_Guide.md
- type: file
  path: docs/guides/OTA/OTA_multiversion-arch-sprint8.md
- type: file
  path: docs/guides/Car_Architecture/Circuit_Connection_Diagram.md
- type: file
  path: docs/images/CanFeederArchitecture.jpg
- type: file
  path: src/setup/toolchains/agl-toolchain.cmake
- type: file
  path: src/setup/toolchains/r5-agl-toolchain.cmake
- type: file
  path: src/ota/rauc/create-bundle.sh
- type: file
  path: src/ota/rauc/bootloader-custom-backend.sh
- type: file
  path: src/ota/rauc/setup-rauc.sh
- type: file
  path: src/ota/scripts/ota-update.sh
- type: file
  path: src/ota/scripts/ota-check.sh
- type: file
  path: src/ota/scripts/setup-ota-device.sh
- type: file
  path: src/ota/scripts/benchmark-ota.sh
- type: file
  path: src/hmi/assets/icons/car-inactive.svg
- type: file
  path: src/hmi/assets/icons/car-active.svg
- type: file
  path: src/shared/backend/providers/vehicleprovider.hpp
- type: file
  path: src/shared/backend/providers/vehicleprovider.cpp
- type: file
  path: src/stm/Middlewares/ST/threadx/common/src/txe_byte_release.c
- type: file
  path: src/stm/Middlewares/ST/threadx/common/src/tx_byte_release.c
- type: file
  path: src/stm/Middlewares/ST/threadx/common/src/txe_block_release.c
- type: file
  path: src/stm/Middlewares/ST/threadx/common/src/tx_block_release.c
- type: file
  path: src/stm/Core/Inc/vehicle_state.h
- type: file
  path: src/stm/Drivers/CMSIS/Include/cmsis_iccarm.h
score: 1.0
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: pending
---
This evidence item collects test reports, logs, scripts, and documentation demonstrating the comprehensive OTA update system implementation.

## Evidence Collection Status

### SOTA (Software Over-The-Air) 🔄
- [ ] Application update scripts created
- [ ] Package management configured
- [ ] SOTA update verification tested

### COTA (Configuration Over-The-Air) 🔄
- [ ] Configuration push scripts created
- [ ] Parameter templates defined
- [ ] Configuration persistence validated

### FOTA (Firmware Over-The-Air) 🔄
- [ ] STM32 firmware update scripts created
- [ ] CAN-based flash utilities implemented
- [ ] Firmware version verification tested

### RAUC OS Updates

#### Phase A: RAUC Configuration ✅
- [x] RAUC installed on AGL (v1.15.1)
- [x] System configuration defined
- [x] A/B partition scheme configured

#### Phase B: Bundle Creation 🔄
- [x] Bundle creation process documented
- [ ] Signed bundle (.raucb) generated
- [ ] Bundle manifest validated

#### Phase C: Installation Workflow ✅
- [x] install-bundle.sh script created
- [x] Pre-install validation implemented
- [x] Config backup mechanism implemented
- [x] Logging to /var/log/rauc-install.log

#### Phase D: Post-Reboot Verification ✅
- [x] post-reboot-verify.sh script created
- [x] 7 health checks implemented:
  1. Network connectivity
  2. Disk space availability
  3. RAUC service status
  4. Boot slot verification
  5. Critical services check
  6. System log analysis
  7. Time synchronization
- [x] Mark-good / rollback logic implemented

#### Phase E: Integration Testing 🔄
- [ ] End-to-end RAUC update test
- [ ] Rollback test on simulated failure
- [ ] Data persistence verification
- [ ] Multi-update cycle test

**Evidence Status:** In Progress - RAUC scripts implemented, SOTA/COTA/FOTA scripts pending, awaiting bundle creation and integration testing.
