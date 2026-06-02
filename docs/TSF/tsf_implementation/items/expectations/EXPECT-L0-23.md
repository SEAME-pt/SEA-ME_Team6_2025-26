---
id: EXPECT-L0-23
header: "Raspberry Pi 5 AGL Boot Time"
text: |
  The Raspberry Pi 5–based system running Automotive Grade Linux (AGL) shall complete its operating system initialization and reach a state where middleware services (e.g. KUKSA) are operational within a bounded time after power-on.

  **Justification:** Linux-based automotive platforms (AGL, QNX, Adaptive AUTOSAR) typically boot in 5–15 s. AGL documentation describes fast boot paths but does not mandate strict limits. This timing does not affect safety, as safety-relevant data is handled by the MCU.

  **Acceptance Criteria:** The RASP5/AGL system reaches middleware operational state within ≤ 10 s after power-on.
level: '1.23'
normative: true
references:
- type: file
  path: ../assertions/ASSERT-L0-23.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
---
