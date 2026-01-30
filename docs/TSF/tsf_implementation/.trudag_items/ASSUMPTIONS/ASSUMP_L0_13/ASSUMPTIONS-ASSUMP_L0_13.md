---
id: ASSUMP_L0_13
header: 'Assumption: Project & GitHub setup'
level: '1.13'
normative: true
references:
- path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_13/EXPECTATIONS-EXPECT_L0_13.md
  type: file
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
evidence:
  type: validate_software_dependencies
  configuration:
    packages:
    - GitHub
    - repository
    - permissions
text: GitHub repository access and permissions are granted to all project team members
  before project setup verification begins.
review_status: accepted
---
GitHub repository access and permissions are granted to all project team members before project setup verification begins.

Acceptance criteria / notes:
- All team members have GitHub accounts.
- Repository permissions (read, write, admin) are configured according to roles.
- Branch protection rules are documented.

Rationale: Project setup verification for `EXPECT-L0-13` assumes that access control and team onboarding are handled externally to the technical verification process.
