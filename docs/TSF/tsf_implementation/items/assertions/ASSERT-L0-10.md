---
id: ASSERT-L0-10
header: Dual Raspberry Pi architecture operational
text: The system operates with Raspberry Pi 5 running computational tasks and Raspberry
  Pi 4 running the display cluster, with wireless communication between them.
level: '1.10'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-10.md
- type: file
  path: ../evidences/EVID-L0-10.md
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
