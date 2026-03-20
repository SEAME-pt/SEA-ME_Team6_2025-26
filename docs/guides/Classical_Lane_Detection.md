# Classical Lane Detection Pipeline

This document describes the classical computer vision pipeline implemented for lane detection, as an alternative to deep learning approaches.

In our case, we also used it for improvement of masks for training a semantic segmentation model.

---

## Pipeline Overview

```
Binary Mask → ROI → Bird's Eye View → Histogram → Sliding Window → Polynomial Fit → Result
```

---

## Steps

**1. Original Mask**
The input is a binary mask where white pixels represent road markings (lane lines). This is extracted from the CARLA semantic segmentation camera (class ID 24 = RoadLine).

**2. ROI (Region of Interest)**
A trapezoidal mask is applied to focus only on the road area ahead of the car, eliminating irrelevant areas such as the sky, trees, and buildings.

**3. Bird's Eye View**
A perspective transform warps the ROI into a top-down view. In this view, parallel lane lines appear parallel (no perspective distortion), which makes polynomial fitting significantly more accurate.

**4. Histogram**
A column-wise sum of white pixels in the bottom half of the bird's eye image produces a histogram. The two peaks identify the starting x positions of the left and right lanes.

**5. Sliding Window**
Starting from the histogram peaks, windows slide upward collecting white pixels within a defined margin. If enough pixels are found, the window recenters on their mean x position. A drift mechanism carries the direction from the previous window when no pixels are found — useful for dashed lines.

**6. Polynomial Fitting**
A 2nd degree polynomial is fitted to the collected pixels for each lane. If one lane has too few pixels (e.g. dashed line with few visible segments), it borrows the curvature coefficient from the other lane — geometrically correct since both lanes share the same curve.

**7. Final Result**
The fitted polynomials are drawn onto a blank canvas in bird's eye space, then warped back to the original camera perspective and overlaid on the RGB image.

---

## Results

The two examples show the pipeline working correctly on different road scenarios:

- ![lane](lane-detection-algorithm_2.jpeg)
- ![lane1](lane-detection-algorithm_1.jpeg)
- **Image 1** — curved road with a dashed center line and a solid outer line. The sliding window correctly tracks both lanes through the curve.
- **Image 2** — straight road with two solid white lines. Clean histogram peaks and precise polynomial fit.

---

## Role in the Project

This pipeline can be applied to real camera images from the track to generate labels for fine-tuning the model on real-world data.
