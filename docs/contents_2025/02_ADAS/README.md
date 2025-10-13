# INTRO

**Advanced Driver-Assistance Systems (ADAS)** are a fusion between ECUs integrated into vehicles and software algorithms to assist drivers with driving and parking tasks. These systems rely on advanced sensors, such as cameras, radar, and lidar—to evaluate the environment around the vehicle. 

The **impact of ADAS on the automotive industry is bigger than you can expect**. The introduction of these technologies has led to significant safety improvements, resulting in a huge decrease in accidents and dead’s. This new era also brings economic benefits like lower insurance costs. Regulatory entities worldwide are increasingly pushing the integration of certain ADAS features in new vehicles, creating higher safety standards. At the same time, ADAS is accelerating the adoption of advanced technologies such as artificial intelligence, IoT, and high-performance sensors. 
This evolution also generates new business opportunities in software development, sensor manufacturing, data analytics, fleet management, and insurance.

In summary, ADAS represents a transformative step in vehicle design and road safety. It proves to be an enabler for the development of fully autonomous vehicles. As technology matures and regulations evolve, the significance of ADAS will continue to grow across the automotive landscape.

# Goals for this Module
## Goal 1: Getting Familiarized with Advanced Driver-Assistance Systems (ADAS)

We are looking to you become familiar with **Key ADAS features**, like:
- Automatic emergency braking (AEB)
- Lane departure warning (LDW), 
- Adaptive cruise control (ACC), 
- Blind-spot detection, 
- Traffic sign recognition (TSR), 
- Parking assistance (PA). 

## Goal 2: Getting Familiarized with Perception, Control, Localization and path-planning, Communication, Simulation


A comprehensive understanding of key words is essential for developing autonomous driving systems.
- **Perception:** 
    - involves getting data using sensors (cameras, radar, lidar) and advanced algorithms to interpret the vehicle’s environment.
- **Control:** 
    - algorithms cable to create physical outputs actions, managing steering, acceleration, and braking.
- **Localization and Path-Planning:** 
    - model to provide and generate optimal trajectories for safe navigation.
- **Communication:** 
    - technologies (V2V, V2I, V2X) allow vehicles to share data with each other and infrastructure.
- **Simulation:** 
    - simulation platforms like CARLA support the development and testing of these components in multiple test cases. With this type of tools we can improve, safety and efficiency, our model before real-world deployment. 

Those are the main foundation of Autonomous Driving.  As part of this project, your goal is to study, select, and implement a control model, such as Model Predictive Control (MPC),in your car, preferably the system should operate in autonomous manner.



## Goal 3: Getting Familiarized with comprehensive simulation environmnents using platforms like CARLA

Simulation environments like CARLA are a big advantage in the automotive industry, particularly for the development and testing of Advanced Driver-Assistance Systems (ADAS). CARLA is an open-source simulator, it can provide a highly realistic, customizable 3D world in which vehicles, sensors, pedestrians, and diverse weather and traffic conditions can be simulated.

The objective is for you to install CARLA and set up the necessary configurations to allow the group to test and validate the algorithms.

## Goal 4: Getting Familiarized with Advanced Driver-Assistance Systems (ADAS) Algorithms , Artificial Inteligence (AI) , Machine Learning (ML) and Deep Learning

Advanced Driver-Assistance Systems (ADAS) has gain more followers during the time by improving safety, convenience, and efficiency. 
One of the main reasson for this growing are sophisticated algorithms powered by Artificial Intelligence (AI), Machine Learning (ML), and Deep Learning. 
These technologies allow cars to get and analyze their surroundings, make complex decisions, and assist or even automate multiples driving functions.

Our objective is to search and evaluate the available open source models for autonomous driving and obstacle detection/avoidance.
The group is required to prepare an Architecture Decision Record (ADR) that explains the research  work behind the selection of the model(s) to be implemented in the car.

## Goal 5: Getting Familiarized with GenAI applied to Advanced Driver-Assistance Systems

As part of this project, students are expected to make their first contact with **Generative Artificial Intelligence (GenAI)** tools, such as Generative Adversarial Networks (GANs), Variational Autoencoders (VAEs), diffusion models, and transformer-based architectures—to support the development of their autonomous driving algorithms. 

This includes using GenAI for tasks like data augmentation, scenario generation, sensor data synthesis, and policy modeling. 

With the using of this tool, you will be cable to upgrade your training, testing, and validation metrics, ensuring greater accuracy and robustness in real-world conditions.

## Goal 6: Getting Familiarized with Advanced Driver-Assistance Systems Software orchestration and updates

The ability to update ADAS software throughout the need of a service visit is really important for Autonomous World.
For that the OEM decided to introduce into their systems the **Over-the-air (OTA) updates** . 
OTA enables deliver remotely security patches, introduce new features, and continuously improve system algorithms. By minimizing downtime and costs for both sides, manufacturers and cars owners.

As part of this project, you are challenge to implement an OTA update mechanism for the ADAS system, thereby ensuring that the vehicle remains secure, up-to-date, and adaptable to future requirements.

---

# What is Expected by the End of this Module


By completing this module, you will:

* Develop and deploy a control model, preferably where the system should operate in autonomous manner, to handle with the acceleration and steering of the car.
* Develop and deploy a Lane Keep Assistant (LKA) feature capable of keeping the car between the road through a full lap, Basic Cruise Control (CC).
    - Bonus: Adaptative Cruise Control (ACC)
* Develop and deploy an Object Detection and Avoidance feature capable to stop the car in case of collision possibility, Emergency Brake Assist (EBA).
    - Bonus: Truck Platooning;
* Defined proper module requirements using the TSF framework.
    - Prepare a pull request to the course book that lists the requirements expected to be satisfied along the module  
* ADR for the Models Selection Decision
* Update a cluster UI using the Qt Framework to integrate the new ADAS features.
* Ensure a testing infrastructure and unit test coverage report generation.
* Gain practical experience with automotive industry-standard tools and workflows, preparing you for real-world automotive software engineering challenges.

## System Architecture after module completion

![System Architecture](https://res.cloudinary.com/dtyy8f2os/image/upload/v1753393082/ADAS-Arch.drawio_rbtxoa.png)