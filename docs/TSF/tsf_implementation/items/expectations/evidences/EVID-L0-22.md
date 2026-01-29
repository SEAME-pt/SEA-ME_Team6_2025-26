---
id: EVID-L0-22
header: "STM32 ThreadX Startup Timing Evidence"
text: |
  Evidence demonstrating that the STM32-based control system running ThreadX achieves deterministic startup within the required ≤ 100 ms time bound. This includes startup timing measurements, CAN transmission logs, and oscilloscope captures showing the time from power-on to first speed data publication over CAN.

  **Verification Method:** Timing measurements using hardware timestamps and logic analyzer captures of CAN bus activity after power-on reset.

  **Expected Artifacts:**
  - Startup timing logs from STM32 debug output
  - CAN bus captures showing first speed message timestamp
  - Oscilloscope/logic analyzer screenshots
level: '1.22'
normative: true
references:
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md
score: 0.0
reviewers:
  - name: Joao Jesus Silva
    email: joao.silva@seame.pt
review_status: accepted
---
This evidence item collects repository artifacts, sprint reports and timing measurements that demonstrate the STM32 ThreadX startup requirement is met.

**Evidence Status:** Pending - timing measurements to be collected during hardware integration testing.
