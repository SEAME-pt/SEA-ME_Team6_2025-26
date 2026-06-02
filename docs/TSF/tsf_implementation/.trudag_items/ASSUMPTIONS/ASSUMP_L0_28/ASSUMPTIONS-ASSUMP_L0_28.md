---
id: ASSUMP_L0_28
header: "Assumption: Temperature Sensor Test Environment Ready"
text: |
  Assumption: The system is equipped with a functional temperature sensor (NTC or digital) properly connected to the STM32 or data acquisition system. A calibrated reference thermometer is available for comparison measurements. The test environment allows for stable temperature conditions during measurement. Both sensors can be positioned in the same ambient conditions for accurate comparison.
level: '1.28'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_28/EXPECTATIONS-EXPECT_L0_28.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_software_dependencies
  configuration:
    components:
      - "Temperature sensor"
      - "Calibrated thermometer"
      - "STM32 data acquisition"
      - "Stable test environment"
---
