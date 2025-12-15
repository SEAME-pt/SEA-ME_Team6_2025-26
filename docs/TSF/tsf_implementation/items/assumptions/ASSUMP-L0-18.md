---
id: ASSUMP-L0-18
header: Assumption: RTOS environment configured
text: Assumption: The real-time operating system (ThreadX) is properly installed, configured, and operational on the target microcontroller.
level: '1.18'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-18.md
  id: EXPECT-L0-18
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
evidence:
  type: validate_software_dependencies
  configuration:
    components:
      - "ThreadX"
      - "STM32 toolchain"
      - "RTOS configuration"
---

The real-time operating system (ThreadX) is properly installed, configured, and operational on the target microcontroller.

**Acceptance criteria / notes:**
- Required components are available and configured in the test environment.
- If a component is unavailable, a documented mitigation plan will be provided.
- This assumption is validated by the `validate_software_dependencies` validator.

**Rationale:** Verification activities for `EXPECT-L0-18` depend on these prerequisites being met.
