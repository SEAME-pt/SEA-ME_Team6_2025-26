---
id: ASSERT-L0-002
header: "L0-002 Assertion: UI auto-start and time update"
text: |
  Evidence indicates the UI service is enabled and time values update in the running application.
level: 1.2
normative: true
references:
  - type: "file"
    path: "../evidences/EVID-L0-002.md"
reviewers:
  - name: "David"
    email: "david@example.com"
    review_status: "pending"
---

Verification: `systemctl status driver-display.service` and screenshots.
