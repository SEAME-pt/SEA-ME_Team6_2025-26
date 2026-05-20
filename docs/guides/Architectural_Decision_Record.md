# ADR - YOLOv8s for object detection.

  - We chose YOLOv8s for our Pi racer project as part of the perception system. YOLOv8s will be tasked with detecting objects such as cars, traffic signs, and traffic lights.

# Why did we choose YOLO?

  - After a long study session of searching AI models, YOLO came to us as the best choice for several reasons, such as:

      - Real-time performance, yolo is one of the fastest models there is, and the reason for that is that YOLO sees an entire image in one go ( unlike other models yolo just needs to pass one time through the neural networks, making it super fast)
   
      - Yolo, it's lightweight enough to run on our Hailo-8.
   
      - Another very important aspect is that Yolo can process around 70 fps, which is very important for us as we are dealing with autonomous driving.
   
      - As yolo processes the whole image at once, it makes fewer mistakes when it comes to mistakenly background patches as objects, Yolo solves that by looking at the whole image at once.
   
      - When it comes to exportability, YOLO can be exported to almost any format, in our case, ONNX.
   
      - Large community.
   
# Why did we choose YOLOv8s?

  - We choose YOLOv8s because it gives us the best results when benchmarking on Hailo-8 and on the Carla simulator.

  - Also, it's the best trade-off between the nano and medium variants.

  - highest frames per second between the variants tested.

  - Lowest latency output between variants.

  - Highest mAP accuracy between variants.

**Yolov8s**

| Run ID | FPS   | Avg Latency (ms) | P50 (ms) | P95 (ms) | P99 (ms) | Min (ms) | Max (ms) | StdDev | CPU Avg (%) | CPU Max (%) |
| ------ | ----- | ---------------- | -------- | -------- | -------- | -------- | -------- | ------ | ----------- | ----------- |
| 0      | 75.76 | 13.20            | 13.17    | 13.46    | 13.78    | 12.34    | 13.99    | 0.17   | 15.3        | 18.5        |
| 1      | 75.65 | 13.22            | 13.19    | 13.47    | 13.67    | 12.22    | 14.06    | 0.17   | 15.1        | 15.6        |
| 2      | 75.36 | 13.27            | 13.24    | 13.55    | 13.85    | 12.30    | 14.08    | 0.18   | 15.0        | 16.7        |

**Yolov11s**

| Run ID | FPS   | Avg Latency (ms) | P50 (ms) | P95 (ms) | P99 (ms) | Min (ms) | Max (ms) | StdDev | CPU Avg (%) | CPU Max (%) |
| ------ | ----- | ---------------- | -------- | -------- | -------- | -------- | -------- | ------ | ----------- | ----------- |
| 0      | 42.93 | 23.29            | 23.39    | 23.59    | 24.08    | 20.85    | 24.26    | 0.32   | 9.9         | 11.3        |
| 1      | 43.03 | 23.24            | 23.39    | 23.62    | 24.19    | 20.91    | 24.47    | 0.40   | 10.0        | 11.8        |
| 2      | 43.02 | 23.24            | 23.40    | 23.58    | 24.02    | 20.93    | 24.37    | 0.37   | 10.1        | 11.3        |


| Run ID | FPS   | Avg Latency (ms) | P50 (ms) | P95 (ms) | P99 (ms) | Min (ms) | Max (ms) | StdDev | CPU Avg (%) | CPU Max (%) |
| ------ | ----- | -----------------| -------- |--------- | -------- | -------- | -------- | ------ | ----------- | ------------| 
| 0      | 36.66 | 27.27            | 27.24    | 27.74    | 28.37    | 25.72    | 29.99    | 0.32   | 8.6         | 11.4        |
| 1      | 36.61 | 27.31            | 27.27    | 27.63    | 28.66    | 25.76    | 30.81    | 0.36   | 8.2         | 11.2        |
| 2      | 36.66 | 27.28            | 27.27    | 27.56    | 28.04    | 25.78    | 28.47    | 0.27   | 8.4         | 11.6        |


# Why did we choose UFLDv2?

After evaluating several lane detection approaches, UFLDv2 stood out as the best option for our project for multiple reasons:

**Existing Hailo ecosystem support**

  - One of the biggest factors behind our decision was the amount of existing work available for running UFLDv2 on Hailo hardware.
    There were already reference implementations, compiled HEF examples, and DFC compilation guides available, which greatly reduced the integration complexity and overall development risk.
    Given our limited development time, this was an extremely important advantage.

  - Backbone and pooling layers run on the Hailo-8 NPU

  - Fully-connected layers and post-processing run on the RPi5 CPU

**Lightweight and suitable for real-time inference**

  - UFLDv2 is lightweight enough to run efficiently on the Hailo-8 accelerator while still maintaining good lane detection accuracy.
    This allowed us to split the pipeline efficiently between the Hailo-8 and the Raspberry Pi 5 CPU:
    
  - Good real-time performance
    
  - After optimising the post-processing stage — including migrating critical computations to a C++ shared library using OpenBLAS — the complete pipeline reaches around 12.5 FPS from frame          capture to final lane coordinate output. For our ADAS use case, this performance is sufficient for real-time operation. Flexible and easy to fine-tune

  - Another important advantage is that UFLDv2 can be fine-tuned easily on custom datasets. This was essential because public lane detection datasets mainly contain outdoor highway environments, while our competition track is:

  - Indoors
    
  - Artificially lit

  - Using simplified lane markings

  - Fixed and repetitive in layout

Fine-tuning on our own recorded dataset significantly improved robustness and detection consistency on the real circuit.

**Why did we switch from the CULane model to the TuSimple model?**

  - Initially, we used the CULane pre-trained weights, but the model struggled with curved lane detection. After analysing the issue, we discovered that the main limitation came from the number of row anchors used by the model:

  - CULane configuration uses 18 row anchors

  - TuSimple configuration uses 56 row anchors

The higher anchor density in the TuSimple configuration provides much finer vertical sampling of lane points, which is especially important for accurately following curves. After switching to the TuSimple pre-trained weights, we observed a clear improvement in curve tracking accuracy and overall lane stability.
