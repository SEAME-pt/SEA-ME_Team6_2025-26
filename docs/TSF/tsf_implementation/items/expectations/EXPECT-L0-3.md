---
id: EXPECT-L0-3
header: Software environment operational
text: The Raspberry Pi 5 runs a Linux environment capable of executing Qt applications.
level: '1.3'
normative: true
references:
- path: ../assertions/ASSERT-L0-3.md
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
