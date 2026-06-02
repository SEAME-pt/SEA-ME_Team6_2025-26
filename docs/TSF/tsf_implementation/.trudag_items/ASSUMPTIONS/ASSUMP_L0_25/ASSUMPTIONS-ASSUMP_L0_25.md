---
id: ASSUMP_L0_25
header: "Assumption: Qt Cluster Environment Ready"
text: |
  Assumption: The Raspberry Pi 4 development environment is properly configured with Qt framework for automotive HMI applications. The display interface and framebuffer are correctly configured for fast startup. The Qt application is designed to display an initial UI independently of backend data availability. Video recording equipment is available for external timing measurements.
level: '1.25'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_25/EXPECTATIONS-EXPECT_L0_25.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_software_dependencies
  configuration:
    components:
      - "Raspberry Pi 4"
      - "Qt framework"
      - "Display interface"
      - "Video recording equipment"
---
