---
id: EVID-L0-24
header: "Combined ThreadX AGL Startup Evidence"
text: |
  Evidence demonstrating that the combined STM32 (ThreadX) and Raspberry Pi 5 (AGL) system provides integrated startup behavior where safety-relevant data is available independently of AGL boot completion.

  **Verification Method:** Integration test. CAN traffic analysis during AGL boot. Correlation of STM32 CAN timestamps with AGL service startup logs.

  **Expected Artifacts:**
  - STM32 CAN traffic logs with timestamps (showing data available within 100 ms)
  - AGL boot logs with service startup times
  - CAN bus captures during AGL boot sequence
  - Video recording of combined startup behavior
  - Correlation analysis of STM32 and AGL timing
level: '1.24'
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
This evidence item collects integration test results, CAN traffic logs, and timing correlation data that demonstrate the combined startup requirement is met.

**Evidence Status:** Pending - integration test to be performed during system integration phase.
