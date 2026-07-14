---
id: ASSERT_L0_19
header: TSF Validation Automated
text: 'Automated TSF validation is implemented and running in the CI/CD pipeline.
  The validation script checks YAML frontmatter structure, required fields, level
  format, and cross-references between TSF items. Validation results are reported
  in GitHub Actions workflow runs.

  '
level: '1.19'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_19/EXPECTATIONS-EXPECT_L0_19.md
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_19/EVIDENCES-EVID_L0_19.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_hardware_availability
  configuration:
    components:
    - STM32
    - CAN
    - Raspberry Pi
---

