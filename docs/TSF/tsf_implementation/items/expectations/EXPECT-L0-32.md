---
id: EXPECT-L0-32
header: ADAS pipeline latency and robustness
text: "The ADAS real-time perception pipeline shall process camera frames and produce\
  \ stable, scenario-robust outputs suitable for low-speed autonomous driving control.\n\
  \nScope of the measured end-to-end (E2E) pipeline:\ndecode -> preprocess -> infer\
  \ -> postprocess -> render\n\nAcceptance criteria:\n1. Normal scenario E2E latency:\n\
  \   - Median <= 70 ms\n   - p95 <= 100 ms\n2. Scenario robustness for difficult\
  \ lighting/geometry:\n   - Shadow scenario p95 <= 120 ms\n   - Curve scenario p95\
  \ <= 120 ms\n3. Post-processing CPU load budget:\n   - Average <= 40% of one CPU\
  \ core\n   - p95 <= 60% of one CPU core\n\nThe perception outputs shall remain stable\
  \ across consecutive frames in normal, shadow, and curve conditions so trajectory\
  \ control receives timely and consistent scene interpretation.\n\nVerification method:\n\
  Instrumented integration test with per-stage timestamps, CPU profiling focused on\
  \ post-processing, and scenario-tagged runs (normal/shadow/curve) to compute median\
  \ and p95 metrics.\n"
level: '1.32'
normative: true
references:
- type: file
  path: ../assertions/ASSERT-L0-32.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
---
