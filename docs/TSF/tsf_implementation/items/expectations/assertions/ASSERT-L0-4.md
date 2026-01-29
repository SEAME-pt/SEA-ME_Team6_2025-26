---
id: ASSERT-L0-4
header: Remote control operational
text: The demonstrator car responds to remote control commands for steering and throttle
  through the implemented joystick interface.
level: '1.4'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-4.md
- type: file
  path: ../evidences/EVID-L0-4.md
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
