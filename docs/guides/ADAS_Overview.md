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
