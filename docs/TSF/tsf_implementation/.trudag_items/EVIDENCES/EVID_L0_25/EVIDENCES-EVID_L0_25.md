---
id: EVID_L0_25
header: "Instrument Cluster Qt UI Availability Evidence"
text: |
  Evidence demonstrating that the instrument cluster system running a Qt-based application on Raspberry Pi 4 displays an initial usable UI within ≤ 2.0 s after power-on.

  **Verification Method:** Integration test. External time measurement using video recording. Qt application startup logs. Framebuffer / display initialization logs.

  **Expected Artifacts:**
  - Video recording from power-on to UI display
  - Qt application startup logs with timestamps
  - Framebuffer initialization timing logs
  - Display driver startup measurements
  - Screenshots of initial UI state
level: '1.25'
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
This evidence item collects video recordings, startup logs, and timing measurements that demonstrate the instrument cluster UI availability requirement is met.

**Evidence Status:** Pending - timing measurements to be collected during hardware integration testing.
