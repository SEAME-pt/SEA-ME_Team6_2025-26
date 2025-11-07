# CAN Bus Overview

## 1. Introduction

The **Controller Area Network (CAN)** is a communication protocol widely used in automotive and embedded systems.  
It allows multiple electronic control units (ECUs) to exchange data efficiently over a **shared two-wire bus** — **CAN_H** and **CAN_L** — reducing wiring complexity and improving reliability.

CAN is robust against electrical noise, supports real-time communication, and includes built-in error detection and priority-based message arbitration.

---

**First test implementation** - Microcontroller + speed sensor + MCP2551 module

![alt text](<CANbus Scheme Updated.png>)

## 2. Purpose in Our Project

In this project, we use the **CAN bus** to enable communication between the main computing boards and low-level controllers.  
Specifically:

| Device | Role | Description |
|:--|:--|:--|
| **Raspberry Pi 5** | High-level node | Receives data from STM32 via CAN and displays it in the cluster UI (Qt application). |
| **STM32U585** | Low-level node | Reads sensor data (wheel speed, etc.), processes it, and sends CAN frames to the Raspberry Pi through the MCP2515 module. |
| **MCP2515 + MCP2551 CAN Module** | CAN interface | Provides CAN controller and transceiver functions for STM32 and Raspberry Pi communication. |

This architecture replicates how modern vehicles share information between ECUs in real time.

---

## 3. System Architecture

Wheel Speed Sensor -> STM32U585 MCU ⇄ **MCP2515 + MCP2551 CAN Module** ═══ CAN_H / CAN_L ═══ **MCP2515 + MCP2551 CAN Module** ⇄ Raspberry Pi 5

- The **STM32U585** communicates with the **MCP2515 module** via SPI, which handles the CAN protocol logic.  
- The **MCP2515** also acts as a CAN controller for the **Raspberry Pi 5**, communicating over SPI.  
- Both modules integrate **MCP2551** transceivers for electrical signaling on CAN_H and CAN_L.  
- The CAN bus is terminated with **120 Ω resistors** at both ends.

---

## 4. Communication Configuration

| Parameter | Value | Notes |
|:--|:--|:--|
| **Protocol** | Classical CAN 2.0A/B | 11-bit identifiers |
| **Bitrate** | 500 kbit/s | Standard for embedded automotive systems |
| **Frame ID** | 0x101 | Wheel speed data |
| **Data Length** | 2 bytes | Encodes speed in 0.1 km/h resolution |
| **Termination** | 120 Ω × 2 | One at each bus end |

---

## 5. Hardware Components

| Component | Function |
|:--|:--|
| **STM32U585 (B-U585I-IOT02A)** | Reads speed sensor, computes speed, and sends data to MCP2515 over SPI |
| **MCP2515 + MCP2551 CAN Module (x2)** | Provides CAN controller + transceiver interface for both STM32 and Raspberry Pi |
| **Raspberry Pi 5** | Receives CAN data via MCP2515 (SPI) and displays it via Qt |
| **120 Ω resistor** | Terminates the bus |
| **Wheel Speed Sensor** | Generates pulse signal proportional to speed |

---

## 6. Data Flow

1. The **speed sensor** sends digital pulses to the STM32.  
2. The **STM32** measures the pulse frequency and computes the speed.  
3. The **STM32** writes this value to the **MCP2515 module**, which transmits a CAN frame (ID **0x101**, **2 bytes**).  
4. The **Raspberry Pi** receives the frame through its **MCP2515** CAN interface.  
5. The **Qt application** updates the cluster display in real time.

---

## 7. Benefits of Using CAN

- **Reliability:** Built-in error detection and retransmission.  
- **Determinism:** Message priority and fixed timing.  
- **Scalability:** Easy to add more ECUs or sensors.  
- **Industry Standard:** Mirrors real automotive systems.

---

## 8. Future Extensions

- Add additional CAN messages (temperature, steering angle).

---


---

## 9. References

- **Microchip MCP2515 Datasheet**
- **Raspberry Pi SPI Pinout** – [pinout.xyz](https://pinout.xyz/pinout/spi)
- **CAN Tutorial** - [CAN-tutorial-setup](https://lastminuteengineers.com/mcp2515-can-module-arduino-tutorial/)   

---
