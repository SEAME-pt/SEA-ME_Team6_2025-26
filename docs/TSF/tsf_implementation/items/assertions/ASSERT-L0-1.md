---
id: ASSERT-L0-1
header: Architecture documentation exists
text: The repository contains system architecture documentation that describes the
  hardware components and software modules of the demonstrator car.
level: '1.1'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-1.md
- type: file
  path: ../evidences/EVID-L0-1.md
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
