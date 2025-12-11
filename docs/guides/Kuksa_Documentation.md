# Introduction to Eclipse KUKSA

# What is kuksa?

  **Kuksa** is an open source project that provides a **standardized data layer** for vehicles, in combines **VSS(Vehicle Signal specification)**, **VISS(Vehicle Information Service Specification)**,
  **Secure Databroker**, **uProtocol integration**, **Edge & Cloud documentation**.

  Kuska also manages routes and stores the data produced by devices such as sensors and provides a platform for efficiently transferring this data between devices, storage and applications.

  **Kuksa** enables applications to read and write vehicle signals in a *standardized**, **secure**, and **hardware independent** way.

  **Why is Kuksa important for our project?**

   Kuksa is perfect for us because of its low latency and real-time data communication, making it ideal for our project as we deal with a critical environment, and we need to communicate as fast as possible 
   to reduce the chance of failure.

   Below are the key features that led us to choose **Kuksa**:

   - **Data Routing** - What is data routing, you ask? Data routing is the process of taking data from its source(sensors) to its destination, which could be a Database, a cloud platform, or a dashboard. This is extremely important because we need to ensure that the data reaches its destination in real time, and Kuksa grants us that.
     
     Now I'm going to tell you how Kuksa handles the routing, so Kuksa uses a Publish-Subscribe model(communication pattern), which is ideal for environments where data needs to be routed efficiently to multiple systems.
     
      - Publishers are devices or applications that send data to the broker.
      - Subscribers are entities (such as cloud platforms, databases, or other devices) that request to receive specific data from the broker.

     Now, a quick example so you can understand it better.

     A temperature sensor sends data to the broker, which might be consumed by:

      - A dashboard that displays real-time temperature data to users.
    
      - A cloud application that aggregates temperature data for analysis or long-term storage.
    
      - A control system that automatically adjusts heating/cooling based on real-time temperature.

     Now you can see how important it is to have efficient data routing.

   - **Real-time Data Processing** - Real-time data processing means that the data is transmitted and processed as soon as it’s generated, with little or no delay. Kuksa is designed to work in environments where timeliness is crucial.

     How can Kuksa ensure us Read-time processing?

      - Kuksa is optimized for low-latency data transmission. This means the broker processes data as soon as its published, with almost no delay. It also supports **asynchronous communication** that allows devices to keep sending and receiving data without waiting for a response, reducing the processing time and improving performance.
    
     Quick example to understand it better:

     In an autonomous vehicle system, real-time data from sensors (cameras) is routed through Kuksa. The broker processes this data almost instantly, enabling the vehicle’s decision-making system to take actions ( stopping the vehicle, changing lanes) in real time.

   - **Scalable and Flexible** - When we talk about scalability, we are referring to the ability of a system to handle an increasing amount of work. Kuksa is designed to scale horizontally, meaning you can add more broker nodes (servers) to handle increased data volumes or to support more devices.
     
      How Kuksa Supports scalability?

      **horizontal scalability** ensures that as your deployment grows—from dozens of devices to thousands and the system can continue to manage the load efficiently.

     <img width="1122" height="765" alt="image" src="https://github.com/user-attachments/assets/07c47945-bb18-4639-9f83-b03f276f91a3" />

     Now talking about flexibility, we have 2 important configurationsÇ

      - **Edge deployment:** Brokers are deployed closer to the devices for local data processing, reducing latency.
    
      - **Cloud deployment:** Kuksa DataBroker can scale up in the cloud to manage large, globally distributed IoT networks.

     Quick example for better understanding:

      - In a smart city project with thousands of sensors (e.g., traffic lights, air quality monitors, weather stations), Kuksa DataBroker can scale out to accommodate the growing number of devices. You can deploy a set of brokers in the cloud to handle high volumes of city-wide data and add more brokers if necessary.


  **Security** - Security is crucial because devices and the data they generate can be highly sensitive. Kuksa implements several layers of security to protect data during transit, prevent unauthorized access, and maintain privacy.

   - Key Security Features:

     - TLS Encryption: ensures that all data transmitted between devices, brokers, and subscribers is encrypted.
    
     - Authentication: Devices, users, and applications must authenticate before connecting to the broker.
    
     - Authorization & Access Control: Kuksa allows administrators to set up fine-grained access control policies.

     Quick example for better understanding:

      - In a healthcare application, patient data must be encrypted when transmitted to avoid privacy violations. Kuksa DataBroker ensures that all communication between medical devices and cloud storage is encrypted and that only authorized healthcare professionals can access the data.

# KUKSA Architecture

**Central Components:**
 - **Broker Node:** The heart of Kuksa DataBroker, responsible for handling data messages and routing them between publishers and subscribers.

 - **Publisher:** Any device or application that sends data to the broker.

 - **Subscriber:** Any entity that listens for specific data streams from the broker.

 - **Storage Layer:** Kuksa can integrate with databases or other data storage systems for persistent storage.

**Protocols Supported:**

 - MQTT: A lightweight messaging protocol optimized for low-bandwidth, high-latency environments, perfect for IoT communication.

 - HTTP: Standard protocol for communication between devices and the broker over the web.

 - WebSocket: A protocol for persistent, full-duplex communication between clients and the broker.

In our case, we are going to use MQTT protocol because we want to send data to our QT.

# Benefits of Kuksa

 - **Scalability**
   
   - Kuksa DataBroker supports a distributed architecture, allowing you to expand the system by adding more broker nodes as your IoT deployment grows.
     
 - **Low Latency:**
   
   - Optimized for real-time data transfer with low overhead, ensuring that data from IoT sensors or devices is transmitted with minimal delay.

 - **Interoperability:**
   
   - Supports multiple protocols and can integrate with a variety of IoT devices, applications, and data storage systems.
     
 - **Easy Integration**

   - Kuksa DataBroker offers APIs and SDKs for developers to integrate it into existing applications and systems.


     

     
     
    
