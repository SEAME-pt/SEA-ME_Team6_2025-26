---
id: EVID-L0-29
header: "Driver Presence Condition Evidence"
text: |
  Evidence demonstrating that the vehicle only accepts driving or autonomous commands when a driver (Diogo's duck) is detected as present inside the vehicle.

  **Verification Method:** Integration test. Execute command attempts with and without the driver present. Video recording showing system behavior in both cases.

  **Expected Artifacts:**
  - Video recording of test with driver (duck) absent - showing command rejection
  - Video recording of test with driver (duck) present - showing command acceptance
  - Driver presence sensor logs
  - Command rejection/acceptance logs
  - Photos of test setup with symbolic driver (duck)
level: '1.29'
normative: true
references:
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md
score: 0.0
reviewers:
  - name: Joao Jesus Silva
    email: joao.silva@seame.pt
review_status: pending
---
This evidence item collects video recordings and logs that demonstrate the driver presence condition requirement is met.

**Evidence Status:** Pending - integration test to be performed during driver presence validation testing.
