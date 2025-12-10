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

    this is basicly a real time vehicle data broker that stores, updates and distributes all vehicle signals using vss.
