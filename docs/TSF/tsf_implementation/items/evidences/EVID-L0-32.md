---
id: EVID-L0-32
header: ADAS pipeline KPI evidence placeholders
text: 'Evidence package for ADAS real-time pipeline performance under normal, shadow,
  and curve scenarios.


  Required evidence set:

  - Integration test report with per-stage timestamps (decode, preprocess, infer,
  postprocess, render)

  - Scenario-tagged latency summary (median and p95 for normal/shadow/curve)

  - Post-processing CPU profiling report (average and p95 of one CPU core)

  - Raw logs or exported metrics used to compute KPI statistics


  Pass criteria for this evidence item:

  - Normal scenario: median <= 70 ms and p95 <= 100 ms

  - Shadow and curve scenarios: p95 <= 120 ms

  - Post-processing CPU: average <= 40% and p95 <= 60% of one CPU core


  The current references keep the placeholder marker to indicate that concrete artifacts
  are pending collection.

  '
level: '1.32'
normative: true
references:
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md
  description: TSF_PLACEHOLDER_EVIDENCE
score: 0.0
---
Evidence item for requirement L0-32.

No measured artifacts are attached yet. Keep `TSF_PLACEHOLDER_EVIDENCE` until real benchmark reports, logs, or dashboard exports are available.
