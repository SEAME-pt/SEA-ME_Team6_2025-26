# Covesa Documentation

# What is Covesa?
  - **Covesa** is an industry alliance focused on advancing connected vehicle technologies through open standards, shared innovation and collaboration.
    
So what is the main goal of Covesa?
  - Covesa's main goal is to enable a more intelligent, interoperable, and personalized mobility experience.
     
What's interesting for us? **VSS(Vehicle Signal Specification)** and whats that?
  VSS is an open, standardized data model created by **COVESA (Connected Vehicle Systems Alliance)** to describe the signals and data inside a vehicle in a consistent, interoperable way.
  
Now, im gonna give you some examples of what VSS is used for:
  - Vehicle Speed.
  - Vehicle Power Train.
  - Door Status(tells the user which door is open).

  Basically it stores data like **RPM**, **KM/h**, **ºc**, and many more.

So why do we use **VSS** and why is it important?

  Every Automaker makes its own apps, its own software, and that makes it hard for apps to run across different brands, makes it hard on suppliers when it comes to integrating software, and makes it hard for devs who want to reuse code. And that's when **VSS** comes to save the day and solve those problems by giving the industry a shared and open source standard.
Below are a few examples of what VSS can help you:

| Without VSS        | With VSS                        |
| ------------------ | ------------------------------- |
| “SPD_veh_raw_01”   | `Vehicle.Speed`                 |
| “engrpm_val02”     | `Vehicle.Powertrain.Engine.RPM` |
| Inconsistent units | Standard units                  |
| Hard to integrate  | Apps work across vehicles       |

So now that you understand what **VSS** is, here is an example of a **VSS** structure.

```
Vehicle:
  Speed:
    type: float
    unit: km/h
  Powertrain:
    Engine:
      RPM:
        type: int
        unit: rpm
```
Now, let's see what **VISS** is. 

**VISS(Vehicle Information Service Specification)** is an open-standard from Covesa for accessing data through a **web-style API**.
It will allow you to read signals such as **Speed**, **battery state**, **GPS**, **Doorlock status**, etc.

Other cool things you can do with **VISS**:
  - Stream Live driving data to the laptop.
  - Send logs to the cloud.
  - Provide web dashboards.
  - Enable remote monitoring during races.
  
And now you ask me what the main purpose of **VISS** is?  **VISS** main purpose is to solve the case where a dev would have to learn every car's proprietary system, and as you can tell, that's not optimal, so **VISS** gives vehicles a common way to share signals with applications.

Now that we know what **VSS** and **VISS** are, I can show you an example of how Covesa really works.

# How Covesa Works

Below, I'm gonna give an example of a data flow to show you how Covesa works:

1. **Sensors/ECUs** generate raw data.
2. **VSS** gives the data standardized name & structures.
3. **MiddleWare(uProtocol in our case)** exposes signals via **VISS**.
4. Apps subscribe, read, or process the signals.

This architecture allows any compliant app to work with any compliant vehicle.

# Implementing Covesa

How do we implement Covesa in our project? Covesa is not a software that we can simply install, we just implement **VSS** tree so that our midleware can use it.
So below is an example of how to implement **VSS**.

**Define Custom VSS tree**
  Create a YAML file, in our case, we will name it **VSS.yaml**.
  
  ```
  Vehicle:
  Speed:
    type: int
    unit: km/h
    description: "Current vehicle speed measured from the speedometer."

Chassis:
  Steering Wheel:
    Angle:
      type: float
      unit: degrees
      description: "Current steering angle."

Powertrain:
  Battery:
    StateOfCharge:
      type: float
      unit: percent
      description: "Battery charge level."

Sensors:
  IMU:
    AccelX:
      type: float
      unit: m/s2
    AccelY:
      type: float
      unit: m/s2
    AccelZ:
      type: float
      unit: m/s2
```
This is how you do a **VSS** implementation.

# Benefits of Covesa
  - **Interoperability** - Same app on different Vehicles.
  - **Scalability** - Easier integration of new modules.
  - **Developer friendly** - Web technologies.
  - **Open ecosystem** - community driven innovation.
  - **Faster prototyping** - Less time reinventing data interfaces.

