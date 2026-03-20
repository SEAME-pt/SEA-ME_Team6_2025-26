# SEAME — Lane Detection Visual Progress

This document shows the visual evolution of the lane detection system, from the initial UFLD implementation to the final SegFormer pipeline.

---

## Phase 1 — UFLD (Ultra Fast Lane Detection)

UFLD was the first model implemented. It predicts lane positions as column indices at fixed horizontal row anchors. The model was trained on synthetic CARLA data.

### UFLD — Initial Results

The first results showed significant zigzag in the lane detections. The root cause was incorrect training labels — the dataset generation script was producing wrong x coordinates for the lane positions, causing the model to learn incorrect patterns.

![UFLD_3](UFLD_3.png)

**Issues identified:**
- Incorrect training labels from flawed clustering algorithm
- Model learned wrong lane positions
- Low FPS (~18) — CARLA was running simultaneously consuming GPU memory

**Fixes applied:**
- Rewrote label generation: left lane = rightmost pixel left of center, right lane = leftmost pixel right of center
- Simplified from 4 lanes to 2 lanes

---

## Phase 2 — SegFormer (First Version)

After improving UFLD, a second approach using SegFormer was explored. SegFormer is a semantic segmentation model that classifies every pixel — naturally better at following curves. The first version used masks containing all visible lane markings.

### SegFormer — All lanes detected

![segformer_v1](segformer_v1.png)

**Issue:** Masks included all ID-24 pixels — model could not distinguish which lanes belonged to the car's current lane.

---

## Phase 3 — SegFormer — Improved Mask Generation

The mask generation was improved to only include the 2 lanes immediately adjacent to the car, using the CARLA Waypoint API for guidance. Additionally, ROI filtering and outlier filtering were added to the post-processing.

### SegFormer — Better lane selection, but still unstable

![segforme_v2](segforme_v2.png)

---

## Phase 4 — SegFormer — ROI + Outlier Filtering

Further improvements to the post-processing pipeline: ROI applied to ignore the top 40% of the image, outlier filtering added before polynomial fitting, and the car center used as an anchor for lane selection.

### SegFormer — Significant improvement on straight roads

![segformer_v3_1](segformer_v3_1.png)

![segformer_v3_2](segformer_v3_2.png)

![segformer_v3_3](segformer_v3_3.png)

---

## Phase 5 — SegFormer 

Major improvements in this version:
- **Outlier detections, ignores spurious pixels**
- **Confidence-based point filtering** — only draws points where mask evidence exists, no extrapolation
- **Dataset quality filters** — intersection detection and traffic light filtering added

### SegFormer Results

![segformer_v6](segformer_v6.png)

![segformer_v6_1](segformer_v6_1.png)

![segformer_v6_2](segformer_v6_2.png)


**Key achievements:**
- Dashed lines detected and tracked but not following correctly ✅
- Curves followed ✅
- Lines more stable and not influenced by outliers detections ✅

---

## Summary

**Conclusions:**
- After exploring semantic segmentation-based approaches for lane detection, we concluded that models natively designed for this task.
- Change the model version and verify differences



| Version | Model | Key Issue | train_loss | val_loss |
|---|---|---|---|---|
| UFLD initial | UFLD | Severe zigzag from incorrect labels |  | |
| SegFormer | SegFormer | Too many lanes detected | | |
| SegFormer | SegFormer | Unstable on curves | 0.0158 | 0.0205 |
| SegFormer | SegFormer | Good on straights, fails at intersections | 0.0154 | 0.0208 |
| SegFormer| SegFormer | Stable, dashed lines detected, curves followed | 0.0148 | 0.0196 |
