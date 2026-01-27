---
id: ASSUMP_L0_15
header: 'Assumption: Agile'
level: '1.15'
normative: true
references:
- id: EXPECT_L0_15
  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_15/EXPECTATIONS-EXPECT_L0_15.md
  type: file
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
evidence:
  type: validate_software_dependencies
  configuration:
    packages:
      - "Agile"
      - "Scrum"
      - "sprint"
text: 'GitHub Projects and Agile/Scrum methodology documentation are accessible for
  self-training before agile process adoption begins.'
---
GitHub Projects and Agile/Scrum methodology documentation are accessible for self-training before agile process adoption begins.

Acceptance criteria / notes:
- GitHub Projects is enabled and accessible for the repository.
- Agile and Scrum training materials (online resources, books, documentation) are accessible to team members.
- Team members complete individual self-training on Agile/Scrum methodologies.
- Sprint schedules and ceremonies (standup, review, retrospective) are defined by the team.

Rationale: Agile process adoption for `EXPECT-L0-15` depends on GitHub Projects tooling availability and individual self-training on Agile/Scrum methodologies. This assumption clarifies that the project uses GitHub Projects exclusively and relies on self-directed learning.
