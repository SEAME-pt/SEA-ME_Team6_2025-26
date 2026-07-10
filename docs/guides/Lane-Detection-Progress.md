# SEAME — Lane Detection Visual Progress

This document shows the visual evolution of the lane detection system, from the initial UFLD implementation to the final SegFormer pipeline.

---

## Phase 1 — UFLD (Ultra Fast Lane Detection)

UFLD was the first model implemented. It predicts lane positions as column indices at fixed horizontal row anchors. The model was trained on synthetic CARLA data.

### UFLD — Initial Results

The first results showed significant zigzag in the lane detections. The root cause was incorrect training labels — the dataset generation script was producing wrong x coordinates for the lane positions, causing the model to learn incorrect patterns.

<img width="803" height="323" alt="UFLD_3" src="https://github.com/user-attachments/assets/e75ef5b2-7348-4716-a57d-e7e7babc4d3a" />

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

<img width="805" height="327" alt="segformer_v1" src="https://github.com/user-attachments/assets/17052aca-fc98-438a-9a3d-26e4aee28136" />


**Issue:** Masks included all ID-24 pixels — model could not distinguish which lanes belonged to the car's current lane.

---

## Phase 3 — SegFormer — Improved Mask Generation

The mask generation was improved to only include the 2 lanes immediately adjacent to the car, using the CARLA Waypoint API for guidance. Additionally, ROI filtering and outlier filtering were added to the post-processing.

### SegFormer — Better lane selection, but still unstable

<img width="805" height="327" alt="segforme_v2" src="https://github.com/user-attachments/assets/73b78883-1648-404e-bdc3-03522f4f35d6" />


---

## Phase 4 — SegFormer — ROI + Outlier Filtering

Further improvements to the post-processing pipeline: ROI applied to ignore the top 40% of the image, outlier filtering added before polynomial fitting, and the car center used as an anchor for lane selection.

### SegFormer — Significant improvement on straight roads

<img width="803" height="323" alt="segformer_v3_1" src="https://github.com/user-attachments/assets/e905a530-771e-4e3a-949a-4e2d095d33aa" />

<img width="803" height="323" alt="segformer_v3_2" src="https://github.com/user-attachments/assets/67c74598-5f65-479d-8240-0ac2d27c9496" />

<img width="803" height="323" alt="segformer_v3_3" src="https://github.com/user-attachments/assets/369e56b9-fb5d-41bf-b341-6f3b1d41be43" />


---

## Phase 5 — SegFormer 

Major improvements in this version:
- **Outlier detections, ignores spurious pixels**
- **Confidence-based point filtering** — only draws points where mask evidence exists, no extrapolation
- **Dataset quality filters** — intersection detection and traffic light filtering added

### SegFormer Results

<img width="803" height="329" alt="segformer_v6" src="https://github.com/user-attachments/assets/ad6a81f9-ac89-4a0f-a193-42a3ab01c0ab" />

<img width="803" height="329" alt="segformer_v6_1" src="https://github.com/user-attachments/assets/fa967bbf-5a7e-4e32-8c5b-cba21d1fa13b" />

<img width="803" height="329" alt="segformer_v6_2" src="https://github.com/user-attachments/assets/e15aba2f-7a66-48b6-8e52-3adf09fe09f8" />


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
