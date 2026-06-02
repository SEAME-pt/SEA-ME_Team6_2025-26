---
id: ASSUMP-L0-29
header: "Assumption: Driver Presence Detection Ready"
text: |
  Assumption: The vehicle platform is equipped with a functional driver presence detection system that can detect the symbolic driver (Diogo's duck). The control system is configured to gate driving and autonomous commands based on driver presence status. Video recording equipment is available for documenting test execution with and without the driver present. The symbolic driver (duck) provides a reliable and repeatable detection signal.
level: '1.29'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-29.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_software_dependencies
  configuration:
    components:
      - "Driver presence sensor"
      - "Vehicle control system"
      - "Diogo's duck (symbolic driver)"
      - "Video recording equipment"
---
