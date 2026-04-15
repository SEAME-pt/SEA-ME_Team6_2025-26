---
id: ASSUMP-L0-32
header: 'Assumption: ADAS timing test conditions'
text: 'Assumption: The ADAS timing and robustness verification environment is representative
  and stable enough to measure real-time KPIs.


  The following conditions are assumed:

  - Camera input stream configuration (resolution, frame rate, codec) is fixed during
  each scenario run.

  - Test hardware and runtime load are controlled so latency and CPU measurements
  are not biased by unrelated background activity.

  - The instrumented pipeline captures consistent stage timestamps for decode, preprocess,
  infer, postprocess, and render.

  - Scenario tagging is correct and reproducible for normal, shadow, and curve routes.

  - CPU profiling for post-processing is sampled with sufficient granularity to compute
  reliable average and p95 values.

  - Perception model/configuration remains constant across compared runs unless explicitly
  declared in the test report.


  Under these assumptions, measured latency and CPU KPIs are considered valid evidence
  of scenario robustness for low-speed autonomous driving control.

  '
level: '1.32'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-32.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_software_dependencies
  configuration:
    dependencies:
    - TSF tooling
---
