---
id: EXPECT-L0-26
header: "End-to-End System Startup Time"
text: |
  The complete system shall provide end-to-end availability of safety-relevant data and a usable instrument cluster UI within time bounds consistent with common automotive architectural practices.

  **Justification:** Automotive systems are architected to ensure fast availability of safety-relevant data and early driver feedback, while allowing complex systems to boot in parallel.

  **Acceptance Criteria:**
  - Safety-relevant speed data is available within ≤ 100 ms after power-on.
  - A usable instrument cluster UI is displayed within ≤ 2.0 s after power-on.
  - Backend services (AGL + KUKSA) become fully operational within ≤ 10 s, without impacting safety or UI availability.
level: '1.26'
normative: true
references:
- type: file
  path: ../assertions/ASSERT-L0-26.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
---
