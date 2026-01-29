---
id: ASSERT-L0-29
header: "Driver Presence Condition Verification"
text: |
  The vehicle has been verified to only accept driving or autonomous commands when a driver is detected as present inside the vehicle. Testing confirms that the vehicle rejects all driving or autonomous commands when no driver is detected, and accepts commands when the driver (Diogo's duck) is detected, meeting functional safety principles for prevention of unintended actuation.

  **Verification Method:** Integration test. Execute command attempts with and without the driver present. Video recording showing system behavior in both cases.
level: '1.29'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-29.md
- type: file
  path: ../evidences/EVID-L0-29.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "Driver Presence Sensor"
      - "Vehicle Control System"
      - "Diogo's Duck"
---
