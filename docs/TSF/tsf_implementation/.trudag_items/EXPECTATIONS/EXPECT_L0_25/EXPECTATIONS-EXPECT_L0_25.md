---
id: EXPECT_L0_25
header: "Instrument Cluster Qt UI Availability"
text: |
  The instrument cluster system running a Qt-based application on Raspberry Pi 4 shall display an initial usable UI within a bounded time after power-on.

  **Justification:** OEM instrument cluster guidelines commonly target 1–2 s for first visual feedback after ignition on. AGL and Tier-1 cluster architectures emphasize early visual availability, even with partial data.

  **Acceptance Criteria:**
  - The initial instrument cluster image is displayed within ≤ 2.0 s after power-on.
  - The Qt application operates independently of backend data availability.
  - Vehicle data values are displayed once published by KUKSA.
level: '1.25'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_25/ASSERTIONS-ASSERT_L0_25.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
---
