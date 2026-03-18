---
id: ASSERT_L0_3
header: Software environment configured
text: The software development environment is set up on the Raspberry Pi 5 with all
  required dependencies and tools installed according to the documented procedures.
level: '1.3'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_3/EXPECTATIONS-EXPECT_L0_3.md
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_3/EVIDENCES-EVID_L0_3.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_hardware_availability
  configuration:
    components:
    - STM32
    - CAN
    - Raspberry Pi
---
