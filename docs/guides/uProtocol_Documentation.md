# uProtocol - A unified Communication Framework for Software Defined Vehicles

# What is uProtocol 

  uProtocol is an open and standardized communication framework designed for **software-defined vehicles**. It provides a unified way for apps, services, sensors, etc, to communicate, no matter who the vendor is.

  In our project, uProtocol will be our middleware, so it facilitates the communication between our speedometer and our other devices. It will work with the **Kuksa** and **Covesa protocol**.

# Why is uProtocol used?

  In the modern-day automotive industry, different vehicles have different ECUs, multiple OS and apps from different vendors. As you can imagine, this is not  optimal because every vendor uses different protocols.
  uProtocol solves this problem by providing one universal layer of communication, making it easier.

    **Traditional:**
  
    [ECU A] --CAN--> [ECU B]
    [ECU C] --Proprietary--> [ECU D]
    
    **With uProtocol:**
    
    [ECU A] \
    [ECU B]  -- uProtocol -- [ECU C]
    [ECU D] /


# Key Goals of uProtocol

  - **Interoperabiliry**
    
    Every app or ECU communicates the same way, regardless of platform.
    
      - very important for our project as we deal with a lot of ECUs such as Brake Control and Motor control.

  - **Open and vendor-neutral**

    Part of the COVESA open ecosystem.
    
      - This will work perfectly as we are using the Covesa protocol.

  - **Lightweight & efficient**
    
    Designed for embedded + real-time systems.

      - This is self-explanatory as we are dealing with a Pi racer with a RTOS software(threadX).

# uProtocol Core Layers

**uTransport**
  - This layer will handle how messages move. What I mean by this is that this will define how data is delivered over existing networks(MQTT and CAN).

Why this matter in our project?
this will allow the speedometer to communicate without worrying about CAN specifics and MQTT implementation details and will make it easier to switch or add transports later.

**uCore**
  - this is the main part of uProtocol, this is where we will define our messages format, messages metadata and identifiers, also makes sure that all messages look the same across systems.

  - this part will also be responsabel for routing messages, validating messages and ensure the Quality of Service

**message flow through uCore**

     [Publisher ECU]
            │
            ▼
    uCore: Format & Metadata
            │
            ▼
      Transport Layer
            │
            ▼
    [Subscriber ECU / App]

Why is this important to us ?
This will guarantee reliable and consistent communication between the speedometer, motor control ECU and brake control ECU.

**uAPI**

  - This layer will provide apps a standard API, will support communication patterns such as Publish / Subscribe  and Request / Response.

        Publish/Subscribe:
        [Sensor] --> [uProtocol] --> [Dashboard / Cloud / ADAS]
        
        Request/Response:
        [App] --> [uProtocol] --> [ECU]
        [ECU] --> [uProtocol] --> [App]


**Application layer**

  - this layer contains all vehicle apps and services such as speedometer service, brake monitoring and motor control logic

how will this work ?

  - **apps use uAPI.**
  - **messages are handled by uCore.**
  - **Data is delivered by uTransport**

 

