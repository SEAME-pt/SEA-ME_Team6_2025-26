---
id: EXPECT_L0_31
header: "Unit tests for core functions"
text: |
  All project functions shall have automated unit tests that verify their
  correct behavior across normal, boundary and error cases. The tests must
  be runnable locally and in the CI pipeline (if configured) using the
  project's standard test runner. Test cases should exercise expected inputs,
  edge conditions and invalid inputs where applicable, and include clear
  assertions for functional outcomes.

  Scope & expectations:
  - Cover all exported functions in the codebase modules relevant to this
    requirement.
  - Provide examples and fixtures needed for deterministic execution.
  - Include clear setup/teardown steps when external resources are required.
  - Add brief instructions on how to run the tests locally and in the CI.

  Acceptance criteria:
  - All unit tests pass when executed.
  - Tests are deterministic and can be executed without manual steps.
  - Test failures clearly indicate the failing behavior to speed debugging.
level: '1.31'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_31/ASSERTIONS-ASSERT_L0_31.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
---
