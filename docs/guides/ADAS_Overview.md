# ADAS (Advanced Driver Assistance Systems) 
this document as the objective to help us understand what is ADAS and how it works

# ADAS fundamentals

**Whats is ADAS (Advanced Drives Assistance Systems) ?**
  - comprise a group of automotive systems that use multiple sensors, cameras and radars to monitor everything around the car. After processing the data in control units, output units can react promptly when needed.

So what are ADAS main goals ?
  - ADAS aims to assist drivers, helping drivers perform taks than we humans arent capable of, making driving safer for us and for the other drivers, by doing so ADAS aims improve:
    - Safety improvment.
    - Driver assistance.
    - Automation.

**Levels of driving automation**

ADAS is closely linked to the automation of vehicles. Depending on the number and types of systems installed, the vehicle falls within one of the following levels:
    
**level 0**
  - The driver has complete control over the car. There are only systems that assist in specific situations, such as the LDWS and parking sensors. Even AEB can be included at this level, acting in specific critical situations.

**level 1**
  - The vehicle has a system that can drive or accelerate/lock, such as the Lane Maintenance Assistant or the ACC.

**level 2**
  - Systems can drive, accelerate and brake simultaneously. Nevertheless, the driver must be attentive and ready to take control at any time.

**level 3**
  - Allows the driver to pause in active driving, since the systems take full control. However, the driver must be prepared to react after receiving a warning.

**level 4**
  - The driver is no longer obliged to react and can even sleep while the vehicle is being driven.

**level 5**
  - Description of a fully autonomous vehicle, which operates in all conditions without human intervention.

**Core Functional Layers of an ADAS system**

Modern Advanced Driver-Assistance Systems (ADAS) follow a modular architecture composed of four main functional layers:

  - Perception

  - Prediction

  - Planning

  - Control

**Perception layer**

the perception layer is responsable of understandig the vehicle surroundings using sensor data, such data can be colected via:

**Inputs**

  - Cameras

  - LiDAR

  - Radar

  - Ultrasonic sensors

**Tasks**

  - Object detection

  - Lane detection

  - Traffic sign recognition

  - Free-space detection

  - Semantic segmentation

**Technologies Used**

  - Convolutional Neural Networks (CNNs)

  - Sensor fusion algorithms

  - Classical computer vision (when applicable)

**Output**

A structured representation of the environment:

  - Object positions

  - Object classes

  - Distances

  - Lane boundaries

This layer basiclly will be charged with being able to "see" whats arround the car.


**Prediction layer**

The prediction layer estimates how detected objects will move in the near future.

**Tasks**

  - Vehicle trajectory prediction

  - Pedestrian motion prediction

  - Estimating time-to-collision

  - Behavior forecasting

**Technologies Used**

  - Recurrent Neural Networks (RNNs)

  - LSTMs

  - Probabilistic models

**Output**

  - Future trajectories and motion probabilities.

This layer will be in charge of knowing what will happen next.

**Planning layer**

The planning layer decides how the vehicle should respond to the environment.

**Tasks**

  - Path planning

  - Collision avoidance

  - Speed adjustment

  - Behavior selection

**Technologies Used**

  - Classical algorithms

  - Optimization methods

  - Reinforcement Learning

**Output**

A safe and feasible driving plan:

  - Desired path

  - Target speed

  - Maneuver decision

This layer is in charge of what the vehicle should do next.

**Control Layer**

The control layer translates the plan into physical vehicle commands.

**Tasks**

  - Steering control

  - Throttle control

  - Brake control

  - Stability adjustment

**Technologies Used**

  - PID controllers
 t
  - Model Predictive Control (MPC)

  - Vehicle dynamics models

**Output**

  - Low-level commands sent to the vehicle actuators.

This layer decides how the decisions should be executed.

# ADAS features

**Lane Keeping Assist (LKA)**

  - Lane Keeping Assist is an active safety system that uses cameras to monitor lane markings and automatically intervenes via steering or braking to prevent a vehicle unintentionally drifting out of his lane.

![what-is-lane-keep-assist-system-1](https://github.com/user-attachments/assets/866cc20a-8c6c-4e78-a08a-37c8dad8d5ab)


**How it works ?**

  - If the car approaches a line, the system uses electric power steering to apply a gentle torque, correcting the path, or uses Electronic Stability Program (ESP) to apply brakes to individual wheels to guide the car back. 

**Sensors:**

  - **Forward facing camera** (main sensor)
    1. Camera captures front road image
    2. image processing/ CNN detect lane lines
    3. System estimates lane center
    4. steering correction is calculated

**Limitations:**

  - Heavy rain
  - Snow
  - Fog
  - Faded lane markings
  - Strong sunlight glare

**AI Models**

  - U-Net
  - ENet
  - Deeplab

**Adaptive Cruise Control (ACC)**

  - Adaptive Cruise Control uses radar, lasers, or cameras to automatically adjust a vehicle's speed to maintain a safe, driver-defined distance from the car in front.

![acc](https://github.com/user-attachments/assets/aac9c2ad-1003-4f7c-bc84-7dd889df9ea6)

**How it works**

  - **Scanning:** Sensors constantly ping the area in front of your car to detect the distance and speed of other vehicles.
  - **Calculating:** An onboard computer analyzes that data against your set speed and gap distance. It determines if you are catching up to a slower car too quickly.
  - **Adjusting:** If the road is clear, it holds your speed. If someone slows down, it automatically reduces throttle or applies the brakes to match their pace and maintain your chosen gap.

**Sensors:**

  - **Forward Facing Radar** (Main Sensor)
    1. Emits radio waves
    2. receives reflected signals from the object ahead
    3. calculates distance, relative speed and direction

**Limitations**

  - Poor object classification
  - Low spatial resolution
  - Radar interference
  - Limited Lateral awareness

**AI Models**

  - YOLO
  - Faster R-CNN
  - SSD
  - Kalman filter

**Automatic Emergency Break (AEB)**

  - an advanced vehicle safety feature that uses sensors to monitor for potential front-end collisions with vehicles, pedestrians, or obstacles.

![autonomous-emergency-braking-2](https://github.com/user-attachments/assets/5ca35a90-2736-4c1a-b11b-4b9f2896494c)

**How it works**

  - **Detection:** The system uses sensors such as radar, cameras, or LiDAR to identify obstacles like vehicles, pedestrians, or cyclists.
  - **Warning:** Before intervening, most systems provide a Forward Collision Warning (FCW) through audible beeps, visual dashboard alerts, or haptic feedback.
  - **Intervention:** If the driver does not brake or applies insufficient pressure, the AEB engages to avoid the crash or reduce the impact speed.

**sensors**

  - **Radar** (main sensor)
    1. It emits radio waves that bounce off objects to measure their distance and relative speed.
    
  - **Cameras**
    1. High-resolution cameras capture visual data, which specialized software analyzes to classify objects.
   
  - **LiDAR**
    1. It uses pulsed laser beams to create a highly accurate 3D map of the surroundings.

  - **Ultrasonic Sensors**
    1. They use high-frequency sound waves to detect objects very close to the car.

**Limitations**

  - Speed Constraints
  - Object Recognition Gaps
  - Weather Factors
  - Technical Failures(False positives)

**AI Models**

  - Yolo
  - SSD
  - Transformers (Vision Transformers - ViT)

**Parking Assist Systems**

  - advanced driver-assistance technologies that automate, or assist with, maneuvering vehicles into parallel or perpendicular spots using sensors and cameras to detect obstacles and spaces.

![1690960808641975](https://github.com/user-attachments/assets/3a0d2378-915f-4847-94a2-6595f9596cf4)

**How it works**

  - **Activation:** The driver activates the automatic parking assist system by pressing a button or through the vehicle's information system.
  - **Search for parking locations:** The system uses various sensors to scan the surrounding environment and identify potential parking spaces.
  - **Detecting obstacles:** Sensors continuously monitor the area around the car, detecting obstacles such as walls, pedestrians and other vehicles.
  - **Calculate parking path:** When a suitable parking space is detected, the system calculates the trajectory and necessary steering input to guide the vehicle into the parking space.
  - **Steering assist:** The automatic parking assist system takes control of the steering wheel and starts driving the vehicle.
  - **Visual and auditory prompts:** During the driver's parking process, the system will provide visual and auditory prompts to guide the driver.
  - **Monitoring and intervention:** During the parking process, the system will always monitor the surrounding environment. If any unexpected obstacles appear, the system may pause or stop the parking process.
  - **Complete parking:** When the vehicle is successfully parked in the parking space, the automatic parking assist system will send out a parking completion signal.

**Sensors**

  - **Ultrasonic Sensors**
    1. hey use echolocation emitting high-frequency sound waves that bounce off obstacles.

  - **Electromagnetic Sensors**
    1. They create an electromagnetic field around the bumper.

  - **Cameras**
    1. Cameras provide the "eyes" for the driver or the automated system.

  - **Radar**
    1. Used in high-end vehicles or fully autonomous parking systems, mmWave radar sensors are more resistant to rain, fog, and dirt than ultrasonic sensors and can detect objects from much further away.

  - **LIDAR**
    1. Uses laser pulses to create a high-resolution 3D map of the environment, allowing the car to identify specific types of obstacles.
   
  - **Side Sensors**
    1. Scan for a gap large enough for the car to fit in as you drive past potential spaces.
   
**Limitations**

  - Blind Spots
  - Weather Conditions
  - Sound Absorption
  - Speed Restrictions
  - Low Light

**AI Models**

  - Yolo
  - ResNet
  - Dijkstra(calcula a trajetoria ideal)


# Sensors Technologies

**Camera based perception systems**






# ADAS architecture overview

**Sensor Layer**

  - this layer is tasked with collecting raw environmental and vehicle data.

<img width="850" height="395" alt="Typical-types-of-sensors-for-ADAS" src="https://github.com/user-attachments/assets/9d491a2a-6dbe-4f91-89e0-852b218d1dd0" />

**Common sensors**

  -   Cameras
  -   Radar
  -   LIDAR
  -   Ultrasonic sensors
  -   IMU
  -   GPS
  -   Wheel speed sensors


**Output**

  - Raw sensor data streams.


**Sensor interface and Preprocessing Layer**

  - This layer is tasked with preparing the raw data collected for the perception algorithms.

**Key functions**

  - Signal conditioning
  - Timestamp synchronization
  - Calibration
  - Noise filtering
  - Image enhancement
  - Radar/LiDAR clustering
  - Coordinate transformation

**AI usage**

  - Low

**Output**

  - Clean, synchronized, sructured sensor data.

**Sensor Fusion Layer**

  - This layer is tasked with combining data from multiple sensors for robust environmental understanding.

<img width="960" height="440" alt="Diagram_Recreation_-_Sensors_Chapter_4_Fig1-_960_x_440" src="https://github.com/user-attachments/assets/6bdd22f5-678a-4b28-9c4a-17cbd965b11f" />


**Fusion types**

  - Low level (raw data fusion)
  - Mid level (feature fusion)
  - High level (object level fusion)

**Techniques**

  - Deep learning based fusion
  - Particle filters
  - Bayesian Networks

**AI usage**

  - High

**Output**

  - Unified environmental model


**Perception Layer**

  - this layer is tasked with detecting and classifying objects in the environment.


**Functions**

  - Object detection
  - Lane detection
  - Traffic sign recognition
  - Traffic light recognition
  - Drivable area detection
  - Free-space estimation

**Common AI models**

  - CNNs
  - YOLO variants
  - Transformer-based vision models

**Output**

  - annoted scene with tracked objects and semantics.

**Localization & Mapping Layer**

  - this layer is tasked with determining the precise vehicle position.

**inputs**

  - GPS
  - IMU
  - HD MAPS
  - Sensor fusion outputs

**Functions**

  - Slam
  - Map matching
  - Ego vehicle outputs

**Output**

  - Vehicle position, orientation and velocity.

**Prediction layer**

  - This tasked with predicting the movement of detected objects.

**Functions**

  - Trajectory prediction
  - Behavior prediction
  - Risk assessment

**Output**

  - Future trajectories of surrounding objects.

**Planning layer**

  - This layer is tasked with determining the safest and the most optimal driving behavior.

**Types of planning**

  -  Behavioral planning
  -  Path planning
  -  Motion planning
  -  Speed planning

**AI usage**

  - Medium

**Output**

  - Desired trajectory + velocity profile

**Control Layer**

  - This layer is tasked with executing the planned maneuvers.

**Function**

  - steering control
  - Brake control
  - Throttle control

**Techniques**

  - PID control
  - Model Predictive Control

**AI usage**

  - Low

**Output**

  - Actuator commands

**Vehicle Actuation layer**

  - This layer is taked with the physical execution.

**Components**

  - Eletronic Power Steering
  - Brake System
  - Engine/Powertrain control

**AI usage**

  - None

**Result**

  - vehicle module adjustment

**Full Data Flow**

<img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/8547cd76-2a2d-4332-a22a-c8f8f99847d3" />


**Full ADAS system Architecture**

<img width="850" height="430" alt="Hierarchical-architecture-of-the-general-ADAS-systems-vs-end-to-end-approaches" src="https://github.com/user-attachments/assets/aa33336f-18d2-4483-80c3-dbc935d64e33" />


**Computational Requirements and Constraints**

  - Advanced Driver Assistance Systems (ADAS) require high-performance, safety-critical computing platforms capable of processing large volumes of sensor data under strict real-time constraints. This section outlines the computational architecture considerations, including edge vs. cloud processing, GPU requirements, latency constraints, and real-time guarantees.


**Edge Computing vs. Cloud Computing**

**Edge (Mandatory)**

- Low latency requirements
- safety-critical decisions
- No dependency on connectivity
- Deterministic real-time execution
- Compliance with functional safety standards

**Typical Edge Hardware**

  - Automotive SoCs (NVIDIA DRIVE, Qualcomm Snapdragon Ride, Mobileye EyeQ)
  - Multi-core ARM CPUs
  - Dedicated AI accelerators (NPUs)
  - Automotive-grade GPUs
  - Hardware safety monitors (ASIL-compliant MCUs)

**Cloud Computing (Supportive Role)**

Cloud systems are typically used for:

  - HD map updates
  - Fleet learning
  - Model training
  - Data logging and analytics
  - OTA software updates

Cloud processing is not used for real-time control decisions due to latency, connectivity, and safety limitations.


<img width="1280" height="720" alt="1719691178617" src="https://github.com/user-attachments/assets/344d0c7a-f5f7-4e52-9e0d-f54d4b1dc17b" />


# AI, MACHINE LEARNING AND DEEP LEARNING IN ADAS

**Artificial Intelligence in ADAS**

  - In the context of ADAS, Artificial Intelligence refers to computational methods that enable systems to learn from data, recognize patterns, make predictions, and improve performance without being explicitly programmed for every scenario.

So how does Ai helps us in our project ?

  - AI helps our project by enabling the system to learn from large amounts of real-world data instead of following fixed rules. It can adapt to complex inputs such as traffic patterns, pedestrians, and road conditions and how we do this you may ask? . We achieve this using machine learning techniques, including deep learning, convolutional neural networks (CNNs) for image recognition, and recurrent neural networks (RNNs) for sequential data like sensor readings. Finally, the AI predicts and chooses the safest and most effective actions for the vehicle in real time.


**Object Detection and Classification**

AI models identify and classify:

  - Vehicles

  - Pedestrians

  - Cyclists

  - Traffic signs

  - Traffic lights

  - Road boundaries

This is done by Deep neural Networks that process camera images to produce semantic labels and confidence scores.

**Semantic Segmentation**

AI is used to classify each pixel of an image into categories such as:

  - Road
 
  - Sidewalk

  - Lane markings

  - Obstacles

  - Drivable area

This enables accurate lane keeping and path planning.

**Sensor Fusion Enhancement**

AI can improve fusion by:

  - Learning cross-sensor feature relationships

  - Enhancing object tracking robustness

  - Compensating for sensor noise and occlusion

**Prediction of Dynamic Objects**

AI models estimate:

  - Future trajectories of surrounding vehicles

  - Pedestrian movement intention

  - Risk probability of collision

This predictive capability is critical for proactive safety systems such as Automatic Emergency Braking.


**Decision Automation**

Decision automation in ADAS refers to the ability to select appropriate driving behavior, prioritaze actions and menage risk under uncertainty.

**Behavior Selection**

  - Lane change decisions

  - Overtaking logic

  - Adaptive cruise control adjustments

All of this AI based decisions are combined with rule based safety layers to ensure determinism and compliance.

**Risk Assessment**

   - Estimate collision probability
   - Evaluate scene complexity
   - Assess driver attention

So, as we can see, AI will help us a lot in this project by enabling intelligent perception and decision support, transforming raw data into actionable environmental understanding. By doing this, it will improve perception and prediction.



**Machine learning vs Deep learning**

**Machine learning**

  - Machine learning is a part of artificial intelligence that enables computer systems to learn patterns from data and make predictions or decisions without being programmed by humans.

<img width="943" height="204" alt="image" src="https://github.com/user-attachments/assets/1cf666c9-a2a8-4146-b9f8-96a05f7d5fc9" />

**Types of Machine learning algorithms**

  - **Supervised learning:** the model learns from labelled datasets, where the input and output are clearly defined.
  - **Unsupervised Learning:** The model identifies patterns in data without any predefined labels.
  - **Reinforcement Learning:** The system learns by interacting with an environment and receiving rewards or penalties for its actions.



**Deep Learning**

  - Deep learning uses artificial neural networks with multiple hidden layers that can automatically learn complex patterns from raw data like images, sounds, and text. When is it used? well deep learning is used in applications such as image recognition, natural language processing, and speech recognition.

<img width="929" height="236" alt="image" src="https://github.com/user-attachments/assets/68e79445-ec51-47a6-a25f-8321a4326f0d" />

**Types of Deep learning**

  - **Convolutional Neural Networks:** It is used for image processing tasks. CNNs are designed to adaptively learn spatial hierarchies of features through convolutional layers.
  - **Recurrent Neural Networks:** Ideal for sequential data. RNNs have loops that allow information to persist, making them effective for tasks like speech recognition and language modeling.
  - **Long Short-term Memory Networks:** A type of RNN that addresses the vanishing gradient problem. They are used for complex sequences, including text and speech.
  - **Generative Adversarial Networks:** GANs consist of two neural networks, that are a generator and a discriminator, that compete against each other, creating synthetic data such as images.
  - **Transformers:** Handles long-range dependencies in data. They are the backbone of models like GPT, used in natural language processing.



| **Basis**              | **Machine Learning (ML)**                                    | **Deep Learning (DL)**                                                       |
| ---------------------- | ------------------------------------------------------------ | ---------------------------------------------------------------------------- |
| **Definition**         | Algorithms that learn from data and improve with experience. | Subset of ML using multi-layered neural networks.                            |
| **Data Requirement**   | Works well with small to medium datasets.                    | Requires large datasets for effective learning.                              |
| **Feature Extraction** | Manual – features must be selected by experts.               | Automatic – learns features directly from data.                              |
| **Training Time**      | Faster and less resource-intensive.                          | Slower and needs more computational power.                                   |
| **Accuracy**           | Depends on feature quality and algorithm.                    | Usually higher with enough data.                                             |
| **Hardware Needs**     | Can run on CPUs.                                             | Often requires GPUs or TPUs.                                                 |
| **Interpretability**   | Easier to interpret.                                         | Difficult to interpret (“black box”).                                        |
| **Examples**           | Spam detection, stock prediction, recommendation systems.    | Image classification, speech recognition, natural language processing (NLP). |


**Machine Learning** when data is structured and limited, and interpretability is important.
**Deep Learning** when working with large unstructured datasets or complex pattern recognition tasks.

**Ruled based vs Learning based Systems**

  - There are two main approaches in Artificial intelligence they are rule-based systems and machine-learning systems. Rule-based systems follow explicit rules created by human experts. They're like a set of instructions given to a computer to follow to make decisions. These systems are good for problems with clear rules and paths. On the other hand, machine learning systems learn from data instead of following explicit rules. They use patterns found in large sets of information to make decisions. These systems can adapt and improve over time as they see more data.

**What is a Rule-Based System?**

  - A rule-based system is a computational framework that relies on a predefined set of explicit rules to make decisions or draw conclusions within a specific domain. In technical terms, these rules are typically formulated as "if-then" statements, where specific conditions trigger corresponding actions. The strength of rule-based systems lies in their transparency and ease of interpretation. However, their drawback is the need for explicit rules, making them less adaptable to complex scenarios or situations where patterns are not easily expressible in rule form.

**Advantages of Rule-based system**

- It provides a clear and understandable way to express logical relationships, enhancing transparency in decision-making.
- The explicit nature of rules enables users to trace the decision-making process, creating transparency in system actions.
- Rule-based systems facilitate easy maintenance and debugging in the process.
- They are scalable and adaptable to changing requirements.


**Limitation of Rule-based system**

- Rule-based systems cannot learn from experience, restricting their capacity to adapt and improve over time.
- Rule-based systems may struggle with uncertain or ambiguous information, leading to potential inaccuracies in decision-making.
- Managing a large number of rules can become complex, posing challenges in organization.

**What is a Machine Learning System?**

  - A machine learning system is a computational framework that leverages algorithms and statistical models to enable computers to learn and make predictions or decisions without being explicitly programmed for each task. In technical terms, these systems analyze and generalize patterns from large datasets, allowing them to adapt and improve their performance over time. The strength of machine learning systems lies in their ability to handle complex, data-driven problems where explicit rule formulation may be impractical.

**Advantage of Machine learning system**

- Machine learning systems can adapt to changing data patterns, automatically improving their performance as they learn from new information.
- It excels at automating complex tasks, reducing the need for explicit programming and enabling the handling of intricate problems.
- Machine learning models can continuously learn and optimize their performance over time, enhancing their ability to make accurate predictions or classifications.

**Limitations of Machine learning system**

- Machine learning models, particularly complex ones, operate as black boxes, making it challenging to interpret their decision-making processes.
- The effectiveness of machine learning heavily relies on the quality and quantity of training data, and inadequate or biased data can lead to inaccurate predictions.
- ML models may struggle to generalize well to new, unseen scenarios if the training data does not sufficiently represent the diversity of potential situations, leading to poor performance in real-world applications.


**Advantages and Limitations of Deep Learning in ADAS**

**Advantages of Deep Learning in ADAS**

  - Superior Perception Performance
  - High-Dimensional Data Handling
  - Generalization to Complex Environments
  - Reduced Manual Feature Engineering
  - Continuous Improvement Through Data

**Limitations of Deep Learning in ADAS**

  - Limited Interpretability
  - Data Dependency
  - Non-Deterministic Behavior
  - High Computational Requirements
  - Safety Certification Challenges
  - Vulnerability to Edge Cases


# Perception Systems

**Object detection**

<img width="720" height="462" alt="image" src="https://github.com/user-attachments/assets/274d68ce-91f8-430d-84ea-8ad11b653dfe" />

**Types of object detection**

1. Single object detection
   - identifying and localizing a single object within an image or a frame.

2. Multi object detection
   - Detecting and locating multiple objects of different classes within an image or frame

3. Real time object detection
   - object detection systems optimized for real time processing, this is often used in applications like self driving cars and drones.

4. Instance segmentation
   - In addition to detect objects this type also involves pixel level segmentation of each object instance within an image

5. Scene understanding
   - It goes beyond object detection by analyzing the relationships between objects and understanding the context of the scene.
  
**Modern object detection techniques**

1. YOLO(v1-v8)
   - treats object detection as a single regression problem , predicting bounding boxes and class probabilities directly from the full image in one forward pass (known for its real time detection capability)

2. RetinaNet
   - Introduces Focal loss to adress the class inbalance problem during training, combines the benefits of single stand and two stage detectors.

3. Mask R-CNN
   - Extends faster R-CNN by adding a branch for predicting segmentation masks, enabling instance segmentation along with object detection.

4. EfficientDet
   - Uses compound scaling method to improve efficiency and accuracy.

5. Detectron2
   - A open source library by facebook ai research with implementations of state of the art detection algorithms like faster R-CNN, mask R-CNN, RetinaNet.

6. CenterNet
   - Uses keypoint estimation to find the center points of bounding boxes and regress the box size and object class.

7. DETR(Detection transformer)
   - Utilizes transformers for end to end object detection, combining CNNs for feature extraction and tranformers for object detection.

8. Vision transformers for object detection
   - Applies transformer models directly to sequences of image patches for object detection.


**Key Topics**

**Intersection Over Union (IoU)**

  - Intersection over union is a metric used to evaluate the accuracy of an object detection algorithm, it measures the overlap between the predicted bounding box and the ground truth bounding box.

<img width="728" height="736" alt="image" src="https://github.com/user-attachments/assets/26791532-4607-4629-895a-c27981a674f5" />


**How do we calculate IoU?**

1. Determine the coordinates of the intersection rectangle
   - calculate the coordinates of the intersection rectangle by finding the maximum of the left and top coordinates and the minimum of the right and bottom coordinates of the two bounding boxes.
     
   - Intersection coordinates:
  
       <img width="412" height="214" alt="image" src="https://github.com/user-attachments/assets/11bc29ce-db4f-4d54-aa81-80950425bf4b" />

2. Calculate the intersection area

   <img width="720" height="149" alt="image" src="https://github.com/user-attachments/assets/fafe1a1b-3131-4118-8897-4d63140c0801" />

   - width and height cant be negative otherwise intersection area is zero
  
3. Calculate the union area

   <img width="720" height="174" alt="image" src="https://github.com/user-attachments/assets/40af5dae-70c2-47bf-9405-4f19e3dfe6a7" />

4. Calculate IoU

   <img width="255" height="128" alt="image" src="https://github.com/user-attachments/assets/e88b67d7-c763-40ca-8d4d-22e2f96726ef" />

**Non Max Supression(NMS)**

  - one of the most common problems with object detection algorithms is that instead of detecting an object one it might detect it twice, to solve this problem we use non max supression to select one entity out of many overlaping entities.

<img width="720" height="238" alt="image" src="https://github.com/user-attachments/assets/0396c4d9-4c2c-462e-ac02-601613188dfc" />

**Importance of NMS**

1. Reduces Redundancy: Without NMS, the model might produce multiple bounding boxes for the same object, leading to redundant detections.
2. Improves Precision: By keeping only the most confident bounding boxes, NMS helps improve the overall precision of the object detection model.
3. Enhances Readability: The final output becomes cleaner and more interpretable, showing only one bounding box per detected object.


**How Non Maximum Supression works**

1. **Input**
   - A set of bounding boxes with associated confidence scores.

2. **Sort bounding boxes**
   - Sort all bounding boxes in descending order based on their confidences scores.

3. **Select the Highest confidence Box**
   - Select the bounding box with the highest confidence score and consider it as the current bounding box.

4. **Calculate IoU**
   - Calculate the Intersection over Union between the current bounding box and all other remaining bounding boxes.

5. **Supress Non maximum Boxes**
   - Remove all bounding boxes that have an IoU greater than a predefined threshold (e.g., 0.5) with the current bounding box.

6. **Reapeat**
   - Repeat steps 3 to 5 for the next highest confidence bounding box among the remaining boxes until all boxes have been processed.
  

**Anchor Box**

  - An anchor box is a template bounding box with a specific aspect ratio and size used as a reference point during the detection process. Multiple anchor boxes with different sizes and aspect ratios are typically associated with each grid cell or feature map cell in the detection network.

   <img width="1633" height="522" alt="image" src="https://github.com/user-attachments/assets/aa9adb5a-97ef-412f-b4a0-94988019f131" />


**How Anchor Box works**

1. Initialization
   - A set of Anchor boxes with different sizes and aspects ratio are defined, we can define three anchor boxes per grid cell with aspect ratios of 1:1, 2:1, 1:2, and there anchor boxes will be placed at each location on the map or grid cell.

2. During training
   - The network predicts class scores and bounding box offsets for each anchor box, adjusting them to better match ground truth objects, with the anchor having the highest IoU selected as the positive sample and the others treated as negative samples.

3. Bounding Box Regression
   - The network predicts offsets for an anchor box’s center coordinates and dimensions, which are then applied to the anchor’s original coordinates to compute the final bounding box.

4. Loss Function
   - The training loss combines a classification loss to ensure correct class prediction and a regression loss to make the predicted bounding boxes closely match the ground truth.

5. During inference
   - The network produces class probabilities and bounding box offsets for each anchor box, and non-maximum suppression is applied to eliminate overlapping predictions and retain only the best bounding boxes.

**Sumary**

Anchor boxes are predefined at multiple sizes and aspect ratios across the feature map, and during training the network predicts class scores and coordinate offsets for each box, matches the highest-IoU anchors to ground truth for learning via classification and regression losses, and during inference refines the boxes and applies non-maximum suppression to keep the best detections.

**Semantic Segmentation**

**What is semantic segmentation**

  - Semantic segmentation models borrow the concept of image classification models and improve upon them. Instead of labeling entire images, the segmentation model labels each pixel to a pre-defined class. All pixels associated with the same class are grouped together to create a segmentation mask

<img width="800" height="600" alt="image" src="https://github.com/user-attachments/assets/ffc96245-5a89-40c0-aec7-da0e828283ab" />

**How does it work**

  Semantic segmentation models generate a segmentation map of an image by assigning a semantic class label to every pixel. This produces segmentation masks, where different regions (such as a tree, ground, or sky) are separated and color-coded according to their class, to achieve this, the models use complex deep neural networks that both group related pixels into meaningful regions and correctly classify each region. These models must be trained on large, human-annotated datasets and learn by adjusting their parameters through techniques like backpropagation and gradient descent.

  <img width="800" height="418" alt="image" src="https://github.com/user-attachments/assets/a1c4bb36-1ae6-403b-a782-8c30425964fa" />


**Usefull Datasets**

Accurate image segmentation requires large, complex datasets containing pixel-level masks that label different objects in an image. These datasets are more detailed than typical machine learning datasets because they must precisely annotate each pixel, many open-source datasets support this task. For example, in driverless car applications, models must be trained to reliably recognize objects like pedestrians, bicycles, and cars to ensure safe braking.

Popular image segmentation datasets include:

**Pascal Visual Object Classes (Pascal VOC)** – Provides multiple object classes, bounding boxes, and detailed segmentation maps.

**MS COCO** – Contains about 330,000 images with annotations for detection, segmentation, and captioning tasks.

**Cityscapes** – Focuses on urban environments, with 5,000 images, 20,000 annotations, and 30 class labels.


**Semantic Segmentation Moduls**



| Model           | Year | Architecture Type     | Key Innovation                                | Strengths                                      | Limitations                     | Common Use Cases             |
| --------------- | ---- | --------------------- | --------------------------------------------- | ---------------------------------------------- | ------------------------------- | ---------------------------- |
| **FCN**         | 2015 | CNN (Encoder-only)    | Fully convolutional design + skip connections | Foundation model, simple                       | Coarse predictions              | Baseline research            |
| **U-Net**       | 2015 | Encoder–Decoder CNN   | Symmetric skip connections                    | Great for small datasets, precise localization | Heavy for large images          | Medical imaging              |
| **SegNet**      | 2017 | Encoder–Decoder CNN   | Pooling indices for upsampling                | Memory efficient                               | Lower accuracy vs modern models | Real-time applications       |
| **PSPNet**      | 2017 | CNN                   | Pyramid Pooling Module                        | Strong global context modeling                 | Computationally expensive       | Scene parsing                |
| **DeepLabv3**   | 2017 | CNN                   | Atrous Spatial Pyramid Pooling (ASPP)         | Multi-scale context                            | Heavy computation               | Autonomous driving           |
| **DeepLabv3+**  | 2018 | CNN (Encoder–Decoder) | ASPP + Decoder refinement                     | High accuracy, strong boundaries               | Slower inference                | High-precision segmentation  |
| **HRNet**       | 2019 | Multi-resolution CNN  | Maintains high-resolution representations     | Fine detail preservation                       | Complex architecture            | Fine-grained segmentation    |
| **SegFormer**   | 2021 | Transformer           | Efficient hierarchical transformer            | Lightweight + strong performance               | Needs larger datasets           | General-purpose segmentation |
| **Mask2Former** | 2022 | Transformer           | Masked attention for universal segmentation   | Works for semantic, instance & panoptic        | High training cost              | Advanced segmentation tasks  |


Best choice for us is DeeplabV3




