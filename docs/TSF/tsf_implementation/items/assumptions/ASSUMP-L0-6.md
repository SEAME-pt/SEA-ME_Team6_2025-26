---
id: ASSUMP-L0-6
header: 'Assumption: The Qt GUI shall display static graphical elements such as...'
level: '1.6'
normative: true
references:
- id: EXPECT-L0-6
  path: ../expectations/EXPECT-L0-6.md
  type: file
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
evidence:
  type: validate_software_dependencies
  configuration:
    packages:
      - "font"
      - "icon"
      - "TrueType"
text: 'Third-party font libraries and icon sets required by the Qt GUI are available
  under compatible licenses before UI implementation begins.'
---
Third-party font libraries and icon sets required by the Qt GUI are available under compatible licenses before UI implementation begins.

Acceptance criteria / notes:
- External font files (e.g., TrueType, OpenType) are downloaded or accessible.
- Icon sets (e.g., Font Awesome, Material Icons) are available with compatible licenses.
- License terms allow redistribution and use in the project context.

Rationale: Qt GUI implementation for `EXPECT-L0-6` depends on external font and icon libraries being legally available. This assumption clarifies that third-party asset procurement and licensing are prerequisites.
