---
id: EVID-L0-23
header: "Raspberry Pi 5 AGL Boot Time Evidence"
text: |
  Evidence demonstrating that the Raspberry Pi 5–based system running Automotive Grade Linux (AGL) completes operating system initialization and reaches middleware operational state within ≤ 10 s after power-on.

  **Verification Method:** Timestamp logging from bootloader, kernel, systemd, and KUKSA service startup. AGL boot logs analysis. External time measurement (video recording from power-on to service availability).

  **Expected Artifacts:**
  - AGL boot logs with timestamps
  - Systemd service startup timing analysis
  - KUKSA service availability timestamps
  - Video recording from power-on to middleware availability
level: '1.23'
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
This evidence item collects boot logs, timing measurements, and video recordings that demonstrate the Raspberry Pi 5 AGL boot time requirement is met.

**Evidence Status:** Pending - timing measurements to be collected during hardware integration testing.
