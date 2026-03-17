---
id: EVID_L0_31
header: "OTA Update System Implementation - Evidence (SOTA, COTA, FOTA, RAUC)"
text: |
  Evidence demonstrating that the comprehensive OTA update system is implemented and functional, covering SOTA, COTA, FOTA, and RAUC OS updates.

  **Verification Method:** 
  - Integration test: SOTA application updates
  - Integration test: COTA configuration changes
  - Integration test: FOTA firmware updates
  - Integration test: RAUC bundle validation, installation, slot switching
  - Health check execution verification
  - Rollback verification on simulated failure
  - Logs from OTA scripts, RAUC, bootloader, and health check scripts

  **Expected Artifacts:**
  
  **SOTA (Software OTA):**
  - Application update scripts
  - Package management configuration
  - Update verification logs
  
  **COTA (Configuration OTA):**
  - Configuration management scripts
  - Parameter files and templates
  - Configuration persistence validation
  
  **FOTA (Firmware OTA):**
  - STM32/ECU firmware update scripts
  - CAN-based firmware flash utilities
  - Firmware version verification logs
  
  **RAUC OS Updates:**
  - RAUC configuration files (system.conf)
  - RAUC bundle creation scripts and signed bundles (.raucb)
  - Installation script (install-bundle.sh)
  - Post-reboot health check script (post-reboot-verify.sh)
  - Test logs showing successful bundle installation
  - Test logs showing automatic slot switching
  - Test logs showing rollback on failure scenario
  - Documentation of A/B partition layout
  - Evidence of data persistence across updates
level: '1.31'
normative: true
references:
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/src/ota/rauc
  description: RAUC implementation scripts
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/ota/rauc/install-bundle.sh
  description: Bundle installation script
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/ota/rauc/post-reboot-verify.sh
  description: Post-reboot health check script
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/OTA/RAUC-implementation.md
  description: RAUC implementation documentation
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/OTA-Action.png
  description: OTA GitHub Action Workflow screenshot
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/OTA-Tags.png
  description: OTA Release Tags screenshot
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/OTA-Release.png
  description: OTA Release Page screenshot
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/OTA-r4-script.jpeg
  description: OTA Script Running on Raspberry Pi 4
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/OTA-r5-script.jpeg
  description: OTA Script Running on Raspberry Pi 5
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
