---
id: ASSERT-L0-6
header: Static GUI elements displayed
text: The Qt cluster application displays static visual elements including speedometer,
  RPM gauge, and status indicators as defined in the QML implementation.
level: '1.6'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-6.md
- type: file
  path: ../evidences/EVID-L0-6.md
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
