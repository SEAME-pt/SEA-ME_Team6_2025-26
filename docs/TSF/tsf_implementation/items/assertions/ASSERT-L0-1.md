---
id: ASSERT-L0-1
header: "Assertion: A system architecture diagram or document is present in the repository"
text: |
  System architecture documentation maps components to implemented modules: The
  system architecture documentation (diagrams and descriptive documents) shall
  provide a clear mapping between the listed hardware and software components and
  their implementation (e.g. Expansion Board, Raspberry Pi 5, Hailo AI Hat, DC
  motors, servo motor, display interface). Verification activities include design
  review, inspection of the architecture document, and traceability checks against
  evidence artifacts (see `../evidences/EVID-L0-1.md`).

  Acceptance criteria:
  - A system architecture diagram or document is present in the repository.
  - The document maps each component to the repository artifacts or implementation
    (folders, scripts, build artifacts).

  Verification methods:
  - Documentation inspection and design review; cross-check with evidence files.
level: 1.2
normative: true
references:
  - type: "file"
    path: "../expectations/EXPECT-L0-1.md"
  - type: "file"
    path: "../evidences/EVID-L0-1.md"
reviewers:
  - name: "Joao Jesus Silva"
    email: "joao.silva@seame.pt"
---

