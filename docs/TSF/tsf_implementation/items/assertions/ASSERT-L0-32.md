---
id: ASSERT-L0-32
header: 'Assertion: ADAS pipeline KPIs verified'
text: "Instrumented integration testing confirms that the ADAS perception pipeline\
  \ meets real-time latency and robustness KPIs for low-speed autonomous driving.\n\
  \nVerified processing chain:\ndecode -> preprocess -> infer -> postprocess -> render\n\
  \nThe assertion is satisfied only when all of the following are true:\n1. Normal\
  \ scenario E2E latency metrics satisfy:\n   - median <= 70 ms\n   - p95 <= 100 ms\n\
  2. Scenario robustness metrics satisfy:\n   - shadow scenario p95 <= 120 ms\n  \
  \ - curve scenario p95 <= 120 ms\n3. Post-processing CPU usage satisfies:\n   -\
  \ average <= 40% of one CPU core\n   - p95 <= 60% of one CPU core\n\nMetrics are\
  \ computed from scenario-tagged runs using per-stage timestamps and CPU profiling\
  \ samples. Compliance requires that all thresholds are met in the same verification\
  \ campaign.\n"
level: '1.32'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-32.md
- type: file
  path: ../evidences/EVID-L0-32.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
---
