---
id: ASSUMP-L0-26
header: "Assumption: Complete System Integration Ready"
text: |
  Assumption: The complete system (STM32/ThreadX, Raspberry Pi 5/AGL, and Raspberry Pi 4/Qt) is fully integrated with all communication interfaces (CAN, KUKSA) properly configured. All subsystems can be powered on simultaneously for end-to-end timing tests. Video recording and log collection capabilities are available across all platforms for comprehensive timing analysis.
level: '1.26'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-26.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_software_dependencies
  configuration:
    components:
      - "STM32 with ThreadX"
      - "Raspberry Pi 5 with AGL"
      - "Raspberry Pi 4 with Qt"
      - "CAN bus"
      - "KUKSA middleware"
---
