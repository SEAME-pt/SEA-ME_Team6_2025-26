---
id: ASSUMP-L0-16
header: 'Assumption: CI/CD'
level: '1.16'
normative: true
references:
- id: EXPECT-L0-16
  path: ../expectations/EXPECT-L0-16.md
  type: file
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
evidence:
  type: validate_software_dependencies
  configuration:
    packages:
      - "GitHub Actions"
      - "CI/CD"
      - "workflow"
text: 'GitHub Actions runner resources and execution quotas are available and sufficient
  for CI/CD pipeline execution before CI/CD verification begins.'
---
GitHub Actions runner resources and execution quotas are available and sufficient for CI/CD pipeline execution before CI/CD verification begins.

Acceptance criteria / notes:
- GitHub Actions is enabled for the repository.
- Execution minutes quota is sufficient for planned build/test workloads.
- Self-hosted runners (if used) are configured and operational.

Rationale: CI/CD verification for `EXPECT-L0-16` assumes that GitHub Actions infrastructure and quota allocation are provided externally to the project.
