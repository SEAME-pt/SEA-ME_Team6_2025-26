---
id: ASSERT-L0-12
header: Display streams wirelessly
text: The Qt cluster display content from Raspberry Pi 5 successfully streams to Raspberry
  Pi 4 wirelessly using VNC protocol with acceptable latency.
level: '1.12'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-12.md
- type: file
  path: ../evidences/EVID-L0-12.md
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
