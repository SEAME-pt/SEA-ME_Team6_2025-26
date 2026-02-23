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

  - Forward facing camera (main sensor)
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

**Adaptive Cruise Control(ACC)**

  - Adaptive Cruise Control uses radar, lasers, or cameras to automatically adjust a vehicle's speed to maintain a safe, driver-defined distance from the car in front.

![acc](https://github.com/user-attachments/assets/aac9c2ad-1003-4f7c-bc84-7dd889df9ea6)

**How it works**

  - **Scanning:** Sensors constantly ping the area in front of your car to detect the distance and speed of other vehicles.
  - **Calculating:** An onboard computer analyzes that data against your set speed and gap distance. It determines if you are catching up to a slower car too quickly.
  - **Adjusting:** If the road is clear, it holds your speed. If someone slows down, it automatically reduces throttle or applies the brakes to match their pace and maintain your chosen gap.

**Sensors:**

  - Forward Facing Radar (Main Sensor)
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
