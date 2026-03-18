---
id: EXPECT-L0-7
header: Cross-compilation capability exists
text: The Qt application can be cross-compiled for the target hardware architecture.
level: '1.7'
normative: true
references:
- path: ../assertions/ASSERT-L0-7.md
  type: file
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
