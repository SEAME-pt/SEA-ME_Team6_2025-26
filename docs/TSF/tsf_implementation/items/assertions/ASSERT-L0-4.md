---
id: ASSERT-L0-4
header: "Assertion: The system can receive remote input to control motor direction and speed"
text: |
  The system can receive remote input to control motor direction and speed
  The car shall accept `forward`, `back`, `left`, `right`, and `stop` commands, and execute them reliably (controller event → wheel actuation) System architecture documentation maps components to implemented modules: The system architecture documentation (diagrams and descriptive documents) shall provide a clear mapping between the listed hardware and software components and their implementation (e.g
  Expansion Board, Raspberry Pi 5, Hailo AI Hat, DC motors, servo motor, display interface).
level: 1.2
normative: true
references:
  - type: "file"
    path: "../expectations/EXPECT-L0-4.md"
  - type: "file"
    path: "../evidences/EVID-L0-4.md"
reviewers:
  - name: "Joao Jesus Silva"
    email: "joao.silva@seame.pt"
---

