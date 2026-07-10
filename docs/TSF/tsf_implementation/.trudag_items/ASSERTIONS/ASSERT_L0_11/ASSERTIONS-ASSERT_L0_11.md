---
id: ASSERT_L0_11
header: Qt launches automatically
text: The Qt cluster application automatically launches on both Raspberry Pi 5 and
  Raspberry Pi 4 during system boot without manual intervention.
level: '1.11'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_11/EXPECTATIONS-EXPECT_L0_11.md
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_11/EVIDENCES-EVID_L0_11.md
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
