# ADR - YOLOv8s for object detection.

  - We chose YOLOv8s for our Pi racer project as part of the perception system. YOLOv8s will be tasked with detecting objects such as cars, traffic signs, and traffic lights.

# Why did we choose YOLO?

  - After a long study session of searching AI models, YOLO came to us as the best choice for several reasons, such as:

      - Real-time performance, yolo is one of the fastest models there is, and the reason for that is that YOLO sees an entire image in one go ( unlike other models yolo just needs to pass one time through the neural networks, making it super fast)
   
      - Yolo, it's lightweight enough to run on our Hailo-8.
   
      - Another very important aspect is that Yolo can process around 50-60 fps, which is very important for us as we are dealing with autonomous driving.
   
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

