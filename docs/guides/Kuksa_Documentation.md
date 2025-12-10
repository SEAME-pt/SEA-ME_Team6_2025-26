# Introduction to Eclipse KUKSA

# What is kuksa ?

  **Kuksa** is an open source project that provides a **standardized data layer** for vehicles, in combines **VSS(Vehicle Signal specification)**, **VISS(Vehicle Information Service Specification)**,
  **Secure Databroker**, **uProtocol integration**, **Edge & Cloud documentation**.

  **Kuksa** enables applications to read and write vehicle signals in a *standardized**, **secure**, and **hardware independent** way.

# Why is Kuksa important

  **Kuksa** comes so to solve some of the industrie problems such as:
  
  - priority sensor interfaces.
  - diferent CAN messages formats.
  - Non-standard data acess.
  - difficult third-party app development.

  **Kuksa** solve this by:
  
  - Unifying **signal naming** with **VSS**.
  - Unifying **data access** with **VISS** like APIs.
  - Adding **security, permissions, authentication**.
  - Suporting **Containerized and distributed automotice apps**.

# Core components of KUKSA 

  **KUKSA.val Databroker**(most important thing about kuksa)

   this is basicly a **real time vehicle data broker** that stores, updates and distributes all vehicle signals using **vss**.

   **Key responsibilities:**
   
   - Hold the full **VSS**
    .
   - Maintains current values.
    
   - Handles subscriptions.
    
   - Enforces **permissions & access control**
    
   - Provides signal timestamps.

   To sumarize think of it as **Database + message bus** for all vehicle signals. Why is all this important ? so you no longer need to directly parse can frames or spi data in every
   application instead all the apps just ask the databroker for standardized signals.

 **VISS like API Server**

   
   A vehicle API layer similiar to covesa **VISSv2** that exposes vehicle signals through:
   
   **Rest API**
   
   - GET/vehicle.speed.
    
   - PUT/vehicle.speed.
   
   **Websocket API**
    
   - subscribe to continuous updates
   
   **Json based messages**

   **Key responsibilities**

   - interfaces between the databrokcer and applications.
   - converts broker messages into viss compatible messages.
   - manages WebSocket client sessions.
   - Handles authentication and authorization.

its basicly a **web server** that acts as a front door for reading/writing vehicle signals. why is that important? bacause any application can read vehicle data with zero knowledge of **CAN or SPI**.
