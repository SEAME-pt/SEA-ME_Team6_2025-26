---
id: ASSERT_L0_28
header: "Temperature Measurement Accuracy Verification"
text: |
  The system has been verified to measure the exterior temperature with accuracy suitable for vehicle status display. Testing confirms that the measured exterior temperature corresponds to the reference temperature with a maximum deviation of ±2 °C, meeting automotive ambient temperature sensing requirements.

  **Verification Method:** Integration test. Measure the actual exterior temperature using a calibrated thermometer. Compare the reference value with the system-reported temperature. Video or image evidence of the measurement setup.
level: '1.28'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_28/EXPECTATIONS-EXPECT_L0_28.md
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_28/EVIDENCES-EVID_L0_28.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "Temperature Sensor"
      - "STM32"
      - "Calibrated Thermometer"
---
