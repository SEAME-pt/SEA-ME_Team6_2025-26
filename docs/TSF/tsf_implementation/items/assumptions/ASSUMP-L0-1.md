---
id: ASSUMP-L0-1
header: "Assumption: Availability of hardware components"
text: "Assumption: The project team will provide the required hardware components and they will be operational in the integration environment during verification activities."
level: '1.1'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-1.md
  id: EXPECT-L0-1
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "Raspberry Pi"
      - "Hailo"
      - "motor"
      - "servo"
      - "display"
      - "board"
---

The project team will provide the required hardware components and they will be operational in the integration environment during verification activities.

**Acceptance criteria / notes:**
- Required components are available and configured in the test environment.
- If a component is unavailable, a documented mitigation plan will be provided.
- This assumption is validated by the `validate_hardware_availability` validator.

**Rationale:** Verification activities for `EXPECT-L0-1` depend on these prerequisites being met.
