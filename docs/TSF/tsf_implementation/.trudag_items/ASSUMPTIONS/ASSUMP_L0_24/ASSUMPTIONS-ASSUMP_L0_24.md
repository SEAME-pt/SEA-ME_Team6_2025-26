---
id: ASSUMP_L0_24
header: "Assumption: Integrated System Environment Ready"
text: |
  Assumption: Both the STM32 (ThreadX) and Raspberry Pi 5 (AGL) systems are properly configured and connected via CAN bus. The CAN interface is functional and allows traffic analysis during boot. Both systems have independent power domains or can be powered simultaneously. Timing measurement capabilities are available on both platforms for correlation analysis.
level: '1.24'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_24/EXPECTATIONS-EXPECT_L0_24.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_software_dependencies
  configuration:
    components:
      - "STM32 with ThreadX"
      - "Raspberry Pi 5 with AGL"
      - "CAN bus interface"
      - "Timing measurement tools"
---
