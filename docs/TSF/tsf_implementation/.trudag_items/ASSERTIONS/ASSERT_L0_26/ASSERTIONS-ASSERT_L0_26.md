---
id: ASSERT_L0_26
header: "End-to-End Startup Time Verification"
text: |
  The complete system has been verified to provide end-to-end availability of safety-relevant data and a usable instrument cluster UI within the specified time bounds. Testing confirms that safety-relevant speed data is available within ≤ 100 ms, instrument cluster UI is displayed within ≤ 2.0 s, and backend services (AGL + KUKSA) become fully operational within ≤ 10 s without impacting safety or UI availability.

  **Verification Method:** End-to-end integration test. External time measurement (video recording from power-on to UI display). System logs from STM32, AGL, and Qt. CAN traffic correlation with UI updates.
level: '1.26'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_26/EXPECTATIONS-EXPECT_L0_26.md
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_26/EVIDENCES-EVID_L0_26.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "STM32"
      - "ThreadX"
      - "Raspberry Pi 5"
      - "AGL"
      - "Raspberry Pi 4"
      - "Qt"
---
