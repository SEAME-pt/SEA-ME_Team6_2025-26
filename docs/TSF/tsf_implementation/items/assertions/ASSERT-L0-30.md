---
id: ASSERT-L0-30
header: "Car ready to next module: The vehicle shall be finished to module 2"
text: |
  The vehicle has been verified to be completely finalized for module 2, with architecture, software, and hardware ready for AGL, ThreadX, CAN, QT, Camera, Sensors, etc., enabling ADAS, Machine Learning, and OTA capabilities.

  **Verification Method:** Visual, Unity, Functional, Integration tests. Conduct comprehensive testing to ensure all components are integrated and operational.
level: '1.30'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-30.md
- type: file
  path: ../evidences/EVID-L0-30.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "AGL"
      - "ThreadX"
      - "CAN"
      - "QT"
      - "Camera"
      - "Sensors"
      - "STM32"
      - "Raspberry Pi"
---
