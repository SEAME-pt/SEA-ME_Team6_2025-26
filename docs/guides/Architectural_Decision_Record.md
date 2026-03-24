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

  - 
