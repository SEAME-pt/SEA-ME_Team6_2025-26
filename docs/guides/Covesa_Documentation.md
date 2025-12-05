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

