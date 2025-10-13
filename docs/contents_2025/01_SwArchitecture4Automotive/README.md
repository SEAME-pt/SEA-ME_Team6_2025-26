# Introduction

Modern automotive systems are becoming increasingly sophisticated, relying heavily on software and advanced user interfaces (UIs) to deliver essential information to drivers clearly and safely. Instrument clusters, which display speed, warnings, vehicle status, and more, must have intuitive, clean, and responsive UIs. Moreover, behind the scenes, automotive systems are complex distributed networks, where numerous components communicate via specialized bus systems like CAN, Automotive Ethernet, LIN, and FlexRay.

As automotive technology evolves, systems are becoming more centralized with fewer but more powerful controllers. However, reliable and standardized internal communication remains crucial. Additionally, automotive manufacturers (OEMs) face the challenge of developing robust infrastructure capable of seamless and efficient over-the-air (OTA) updates. This enables different system components to be updated independently, without requiring full system reflashes.

# Goals

### Goal 1 – Integrate Speedometer with Hardware

Mount and integrate a physical speedometer onto a PiRacer wheel. Establish communication between the speedometer and the B-U585I-IOT02A development board via the I²C interface, ensuring accurate and efficient data transfer with the STM32U585AI Microcontroller.

### Goal 2 – Data Processing with ThreadX RTOS

Use the Eclipse ThreadX Real-Time Operating System (RTOS) on the STM32U585AI microcontroller to asynchronously process incoming speed data. Prepare this data efficiently to maintain real-time performance essential for automotive applications.

### Goal 3 – Implement Communication using COVESA and uProtocol

Apply COVESA (Connected Vehicle Systems Alliance) standards and the uProtocol messaging protocol to package processed data. Transmit this structured data securely and reliably over a CAN bus network to a Raspberry Pi 5.

### Goal 4 – Develop an Instrument Cluster UI with Qt Framework

Design and implement an intuitive and visually appealing instrument cluster UI using the Qt Framework on the Raspberry Pi 5. This interface will display real-time speed data and additional vehicle information clearly and effectively.

### Goal 5 – Setup CI/CD Pipeline for OTA Updates

Create and configure a GitHub Actions pipeline that includes continuous integration practices like linting, testing, and building. Automate the deployment of OTA updates to the system components, ensuring seamless integration and efficient update management without the need for full reflashes.

# What is Expected by the End of This Module

By completing this module, you will:

* Defined proper module requirements using the TSF framework.
  - Prepare a pull request to the course book that lists the requirements expected to be satisfied along the module  
* Successfully integrate hardware (speedometer) with a STM32U585AI microcontroller within the B-U585I-IOT02A development board via I²C.
* Use Eclipse ThreadX RTOS for efficient real-time data processing.
* Apply standardized communication protocols (COVESA and uProtocol) for structured and reliable data transfer.
* Develop a responsive and user-friendly instrument cluster UI using the Qt Framework.
* Ensure a testing infrastructure and unit test coverage report generation.
* Implement a robust CI/CD pipeline to automate testing and over-the-air updates.
* Gain practical experience with automotive industry-standard tools and workflows, preparing you for real-world automotive software engineering challenges.

Here’s the updated list with **official websites** and **GitHub repositories (if available)** for each topic:

---

## Useful Links

* **uProtocol**
  * Website: [https://uprotocol.org](https://uprotocol.org)
  * GitHub: [https://github.com/uptane/uprotocol-core](https://github.com/uptane/uprotocol-core)
* **COVESA (Connected Vehicle Systems Alliance)**
  * Website: [https://www.covesa.global](https://www.covesa.global)
  * GitHub: [https://github.com/COVESA](https://github.com/COVESA)

* **ThreadX RTOS (Azure RTOS ThreadX)**
  * Website: [https://learn.microsoft.com/en-us/azure/rtos/threadx/](https://learn.microsoft.com/en-us/azure/rtos/threadx/)
  * GitHub: [https://github.com/azure-rtos/threadx](https://github.com/azure-rtos/threadx)

* **Qt Framework**
  * Website: [https://www.qt.io/product/framework](https://www.qt.io/product/framework)
  * GitHub (Community/Mirror): [https://github.com/qt](https://github.com/qt)

## System Architecture after module completion

![System Architecture](https://res.cloudinary.com/dtyy8f2os/image/upload/v1753392358/Sw4Auto-Arch.drawio_t5mfz3.png)