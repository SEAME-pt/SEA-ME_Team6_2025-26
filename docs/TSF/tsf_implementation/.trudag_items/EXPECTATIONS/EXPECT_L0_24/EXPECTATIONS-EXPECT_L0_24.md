---
id: EXPECT_L0_24
header: "Combined Startup ThreadX and AGL"
text: |
  The combined STM32 (ThreadX) and Raspberry Pi 5 (AGL) system shall provide an integrated startup behavior where safety-relevant data is available before or independently of the completion of the AGL boot process.

  **Justification:** Common automotive architectural practice separates safety-critical MCUs from Linux-based systems. The slower boot domain (AGL) must not block faster safety-relevant functions.

  **Acceptance Criteria:**
  - Speed data publication from the STM32 is available within ≤ 100 ms after power-on.
  - AGL system becomes operational within ≤ 10 s, without blocking or delaying data publication.
  - No startup conflict occurs between ThreadX and AGL domains.
level: '1.24'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_24/ASSERTIONS-ASSERT_L0_24.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
---
