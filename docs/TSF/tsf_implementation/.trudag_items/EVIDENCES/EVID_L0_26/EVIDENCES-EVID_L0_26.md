---
id: EVID_L0_26
header: "End-to-End Startup Time Evidence"
text: |
  Evidence demonstrating that the complete system provides end-to-end availability of safety-relevant data and a usable instrument cluster UI within the specified time bounds.

  **Verification Method:** End-to-end integration test. External time measurement (video recording from power-on to UI display). System logs from STM32, AGL, and Qt. CAN traffic correlation with UI updates.

  **Expected Artifacts:**
  - Video recording from power-on to full system availability
  - STM32 startup and CAN transmission logs (showing data within 100 ms)
  - Qt UI startup timing logs (showing UI within 2.0 s)
  - AGL/KUKSA service startup logs (showing operational within 10 s)
  - CAN traffic captures correlated with UI updates
  - End-to-end timing correlation analysis
level: '1.26'
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
This evidence item collects end-to-end integration test results, video recordings, and system logs that demonstrate all startup time requirements are met.

**Evidence Status:** Pending - end-to-end integration test to be performed during final system integration phase.
