---
id: ASSERT_L0_25
header: "Instrument Cluster Qt UI Verification"
text: |
  The instrument cluster system running a Qt-based application on Raspberry Pi 4 has been verified to display an initial usable UI within the specified time bound. Testing confirms that the initial instrument cluster image is displayed within ≤ 2.0 s after power-on, the Qt application operates independently of backend data availability, and vehicle data values are displayed once published by KUKSA.

  **Verification Method:** Integration test. External time measurement using video recording. Qt application startup logs. Framebuffer / display initialization logs.
level: '1.25'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_25/EXPECTATIONS-EXPECT_L0_25.md
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_25/EVIDENCES-EVID_L0_25.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "Raspberry Pi 4"
      - "Qt"
      - "Instrument Cluster"
---
