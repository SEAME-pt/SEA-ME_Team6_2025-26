---
id: ASSUMP-L0-3
header: "Assumption: Software dependencies available"
text: "Assumption: All required software dependencies, libraries, and development tools are installed and accessible in the build and runtime environments."
level: '1.3'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-3.md
  id: EXPECT-L0-3
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
evidence:
  type: validate_software_dependencies
  configuration:
    components:
      - "Qt"
      - "AGL"
      - "Linux"
---

All required software dependencies, libraries, and development tools are installed and accessible in the build and runtime environments.

**Acceptance criteria / notes:**
- Required components are available and configured in the test environment.
- If a component is unavailable, a documented mitigation plan will be provided.
- This assumption is validated by the `validate_software_dependencies` validator.

**Rationale:** Verification activities for `EXPECT-L0-3` depend on these prerequisites being met.
