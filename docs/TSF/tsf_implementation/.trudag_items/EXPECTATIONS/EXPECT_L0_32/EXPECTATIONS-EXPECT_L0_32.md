---
id: EXPECT_L0_32
header: "End-to-End AI Inference Performance (Dual Model Execution)"
text: |
  The system shall execute two concurrent AI/ML models on the Hailo AI accelerator
  and process results in the instrument cluster with a minimum combined throughput
  of 10 frames-per-second (FPS) during normal operation. End-to-end latency from
  camera capture to result display shall be ≤ 100 ms per frame. The concurrent
  models may include combinations such as lane detection and object detection.
  Verification shall measure throughput and latency on the target hardware using
  production inference pipelines and the Qt instrument cluster renderer.
level: '1.32'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_32/ASSERTIONS-ASSERT_L0_32.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
---
