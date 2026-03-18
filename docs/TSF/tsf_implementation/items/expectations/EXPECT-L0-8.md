---
id: EXPECT-L0-8
header: CAN communication established
text: The Raspberry Pi 5 and STM32 microcontroller communicate bidirectionally using
  the CAN protocol.
level: '1.8'
normative: true
references:
- path: ../assertions/ASSERT-L0-8.md
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
