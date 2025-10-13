# Introduction

Brisa operates tolling infrastructure across several geographies and is exploring new, software-centric ways to run and expand these services. Today, tolling depends on costly roadside equipment and in-vehicle devices (on-board units, or OBUs). A “dematerialized” approach for detecting toll passages unising the vehicle’s own systems (e.g., GPS) and secure commucation to the backend could reduce costs, speed up deployment in new markets, and simplify the user experience.

Your project in this module mirrors a real commercial problem with clear business value: prototyping a Via Verde-like in-vehicle app on Android Automotive OS (AAOS) that detects toll events via GPS and communicates securely with Brisa’s backend.
This module will guide you through building a working prototype while learning modern automotive software practices: secure data handling, reliable toll passage event detection, backend integration, traceability, and testing.

# Goals for this module

## Goal 1 – Build a Via Verde in-vehicle app on Android Automotive OS (AAOS) in a virtualized environment (Android Cuttlefish)

**Description**
Set up an AAOS development environment and run the app on Android Cuttlefish.

## Goal 2 – Propose and implement cryptographically safe ways to obtain and process necessary in-vehicle data (e.g., GPS)

**Description**
To obtain the in-vehicle GPS Signal, we need to obtain and consume it an a cryptographicaly secure way, such that no external elements in the system can have access to it.

## Goal 3 – Implement strategies to monitor GPS and detect toll passage events

**Description**
Take advantage of stratagies like **geofencing** to accurately detect and validate a toll passage event.

## Goal 4 – Communicate with Brisa backend API

**Description**
Create a API layer that will communicate toll passages and other events to Brisa's backend.

## Goal 5 – Requirements and traceability to architecture and implementation using TSF in GitHub

**Description**
Use the **Trustable Software Framework (TSF)** to ensure and maintain evidence of traceability between requirements and tests.

## Goal 6 – Ensure proper test coverage

**Description**
* **Unit tests** for detection logic, crypto utilities, etc.
* **Integration tests** for API client.
* **Simulation tests** using recorded GPS traces, measuring precision/recall of toll detection.

### Bonus goals

* **Raspberry Pi 5 target:** run your AAOS enviornment on the Raspberry Pi 5 and integrate functionality developed in the past modules.
* **Explore new ideas:** be creative, suggest and implement new ideas that might be appealing to Brisa!

# What is expected by the end of this module

* A working **AAOS app** running in **Android Cuttlefish**.
* A **toll detection engine** that takes advantage of GPS to detect toll passage events.
* **Secure data handling** of the GPS and other in-vehicle signals.
* A **backend client**: to communicate with Brisa's backend.
* A **TSF traceability** set: requirements list, architecture notes/diagrams, and links from requirements → code → tests in GitHub.
* **Automated tests** unit, integration and simulation.
* **Bonus** Other use cases implemented:
    * AAOS environment running on Raspberry 5 with other car functions integrated.
    * Other mobility scnenarios interesting for Brisa (Parking, wrong-way detection and warning, etc.).

# References
[Brisa Hackathon Presentation 1](https://drive.google.com/file/d/14oSd0-ELNLIHx3Rhfadgm-hk0TPfBOY9/view?usp=sharing)

[Brisa Hackathon Presentation 2](https://docs.google.com/presentation/d/1Xu_kW0dbnZOloRfq2fD9HiBHdD2DLZCL/edit?usp=sharing&ouid=107858420601779353822&rtpof=true&sd=true)