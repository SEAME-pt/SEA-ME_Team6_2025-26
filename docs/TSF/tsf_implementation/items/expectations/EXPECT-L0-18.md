---
id: EXPECT-L0-18
header: Communicate via can bus also using threadx
text: The real-time system shall communicate via can bus also using threadx with proper thread management and scheduling.
level: '1.18'
normative: true
references:
- type: file
  path: ../assertions/ASSERT-L0-18.md
  id: ASSERT-L0-18
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
---

This expectation is derived from requirement **L0-18** in LO_requirements.md.

**Acceptance Criteria:**
Threads should create and send messages using CAN Bus to sent/receive messages successfully
