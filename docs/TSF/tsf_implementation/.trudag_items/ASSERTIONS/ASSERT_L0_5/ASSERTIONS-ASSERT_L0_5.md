---
id: ASSERT_L0_5
header: Basic Qt application launches
text: A basic Qt QML application successfully launches and displays on the Raspberry
  Pi 5 using the documented Qt 6 installation.
level: '1.5'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_5/EXPECTATIONS-EXPECT_L0_5.md
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_5/EVIDENCES-EVID_L0_5.md
review_status: accepted
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
evidence:
  type: validate_hardware_availability
  configuration:
    components:
    - STM32
    - CAN
    - Raspberry Pi
---
