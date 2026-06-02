---
id: ASSUMP_L0_30
header: "Assumption: Car ready to next module: The vehicle shall be finished to module 2"
text: |
  Assumption: All required components (AGL, ThreadX, CAN, QT, Camera, Sensors, STM32, Raspberry Pi) are available and functional. The development environment, testing infrastructure, and integration tools are properly configured to support finalization of the vehicle for module 2, enabling ADAS, Machine Learning, and OTA capabilities.
level: '1.30'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_30/EXPECTATIONS-EXPECT_L0_30.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_software_dependencies
  configuration:
    components:
      - "AGL"
      - "ThreadX"
      - "CAN"
      - "QT"
      - "Camera"
      - "Sensors"
      - "STM32"
      - "Raspberry Pi"
      - "Development environment"
      - "Test infrastructure"
      - "Integration tools"
---
