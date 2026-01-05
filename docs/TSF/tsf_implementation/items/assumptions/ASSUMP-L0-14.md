---
id: ASSUMP-L0-14
header: 'Assumption: TSF & Documentation'
level: '1.14'
normative: true
references:
- id: EXPECT-L0-14
  path: ../expectations/EXPECT-L0-14.md
  type: file
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
evidence:
  type: validate_software_dependencies
  configuration:
    packages:
      - "TSF"
      - "TruDAG"
      - "Trustable"
text: 'The Trustable Software Framework (TSF) methodology documentation and reference
  implementations are publicly accessible online before TSF implementation begins.'
---
The Trustable Software Framework (TSF) methodology documentation and reference implementations are publicly accessible online before TSF implementation begins.

Acceptance criteria / notes:
- TruDAG documentation: https://codethinklabs.gitlab.io/trustable/trustable/
- TSF methodology: https://codethinklabs.gitlab.io/trustable/trustable/methodology.html
- Reference implementation: https://gitlab.com/CodethinkLabs/safety-monitor/safety-monitor/-/tree/main/trustable
- Installation guide: https://codethinklabs.gitlab.io/trustable/trustable/trudag/install.html
- Eclipse TSF project: https://projects.eclipse.org/projects/technology.tsf
- Codethink TSF GitLab: https://gitlab.com/CodethinkLabs/trustable/trustable
- TruDAG source docs: https://gitlab.com/CodethinkLabs/trustable/trustable/-/tree/main/docs/trudag
- SCORE-JSON schema: https://score-json.github.io/json/main/generated/trustable_graph.html

Rationale: TSF implementation for `EXPECT-L0-14` depends on external documentation and examples being available for consultation and self-training. This assumption provides explicit URLs for rastreability and transparency.
