# Covesa Docoumentation

# What is covesa ?
  - **Covesa** is an industry alliance focused on advancing connected vehicle technologies through open standards, shared innovation and collaboration.
    
So what is the main goal of Covesa ?
  - Covesa main goal is to enable a more intelligent, interoperable and personalized mobility experience.
     
Whats interesting for us? **VSS(Vehicle Signal Specification)** and whats that?
  VSS is an open, standardized data model created by **COVESA (Connected Vehicle Systems Alliance)** to describe the signals and data inside a vehicle in a consistent, interoperable way.
  
Now, im gonna give you some examples of what VSS is used for:
  - Vehicle Speed.
  - Vehicle Power Train.
  - Door Status(tells the user which door is open).

  Basicly it stores data like **RPM**, **KM/h**, **ºc** and many more.

So why do we use **VSS** and why its important ?

  Every Automaker makes their own apps, their own software, and that makes it hard for apps to run across diferent brands, makes it hard on suplies when it comes to integrate software and makes it hard for devs o want to reuse code. And thats when **VSS** comes to save the day and solve those problems by giving the industry a shared and open source standard.
Below are few examples on what vss can help you:

| Without VSS        | With VSS                        |
| ------------------ | ------------------------------- |
| “SPD_veh_raw_01”   | `Vehicle.Speed`                 |
| “engrpm_val02”     | `Vehicle.Powertrain.Engine.RPM` |
| Inconsistent units | Standard units                  |
| Hard to integrate  | Apps work across vehicles       |

So now that you understand what **VSS** is here is an example of a **VSS** structure.

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
Now lets see what **VISS** is. 

**VISS(Vehicle information Service Specification)** its an oped-standard from covesa for accessing data throug a **web style API**.
It will allow you to read signals suchs as **Speed**, **battery state**, **GPS**, **Doorlock status**, etc.

And now you ask me what the main purpose of **VISS** ?  **VISS** main porpuse is to solve the case where a dev would have to have to learn every car proprietary system and as you can tell that's not optimal so **VISS** gives vehicles a common way to share signals with applications.


Now that we know what **VSS** and **VISS** i can show you an exame on how covesa really works.

# How Covesa Works

below im gonna give an example of a data flow to show you how Covesa works:

1. **Sensors/ECUs** generate raw data.
2. **VSS** gives the data standardized name & structures.
3. **MiddleWare(uProtocol in our case)** exposes signals via VISS.
4. Apps subscribe , read or process the signals.
