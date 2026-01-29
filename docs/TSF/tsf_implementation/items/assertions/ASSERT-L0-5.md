---
id: ASSERT-L0-5
header: Basic Qt application launches
text: A basic Qt QML application successfully launches and displays on the Raspberry
  Pi 5 using the documented Qt 6 installation.
level: '1.5'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-5.md
- type: file
  path: ../evidences/EVID-L0-5.md
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
