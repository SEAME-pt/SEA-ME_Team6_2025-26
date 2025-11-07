# How CAN Bus Works

# Index

1. [Introduction](#1-introduction)  
2. [Physical Signal and Electrical Layer](#2-physical-signal-and-electrical-layer)  
3. [Bus Topology](#3-bus-topology)  
4. [Half-Duplex Communication](#4-half-duplex-communication)  
5. [Asynchronous Transmission](#5-asynchronous-transmission)  
6. [Message Arbitration](#6-message-arbitration)  
7. [CAN Data Frame Structure](#7-can-data-frame-structure)  
8. [Summary of Key Properties](#8-summary-of-key-properties)  
9. [Why It Matters in Automotive Systems](#9-why-it-matters-in-automotive-systems)  
10. [References](#10-references)

## 1. Introduction

The **Controller Area Network (CAN)** is a serial communication protocol developed by Bosch in the 1980s, designed for reliable data exchange between electronic control units (ECUs) in vehicles and embedded systems.

Unlike simple serial links (UART, SPI, I²C), CAN allows **multiple devices to share the same communication lines** while maintaining **high reliability**, **error detection**, and **real-time deterministic behavior**.

---

## 2. Physical Signal and Electrical Layer

CAN uses **two differential signal lines**:
- **CAN_H (High)**
- **CAN_L (Low)**

![alt text](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/images/CANsignal.png)

Both lines carry complementary voltages that represent logical bits:

| Bus State | CAN_H Voltage | CAN_L Voltage | Logical Level | Description |
|:--|:--|:--|:--|:--|
| **Dominant** | ≈ 3.5 V | ≈ 1.5 V | `0` | Actively driven by one or more nodes (overrides recessive) |
| **Recessive** | ≈ 2.5 V | ≈ 2.5 V | `1` | Idle state, no node drives the bus |

🧩 Because CAN uses **differential signaling**, it is highly resistant to electromagnetic noise — both lines rise or fall together, so interference affects them equally and cancels out.

---

## 3. Bus Topology

CAN is implemented as a **multi-drop bus** — all nodes share the same two wires (CAN_H, CAN_L):

![can-topology](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/images/CANtopology2.png)


### Topology Characteristics
- **Linear (bus) structure**, not star or ring.
- **120 Ω termination resistors** at both physical ends to prevent signal reflections.
- Any node can transmit, but only **one at a time**.
- Nodes can be added or removed easily without changing the others.

![can-topology-2](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/images/CANtopology.png)

---

## 4. Half-Duplex Communication

The CAN bus is **half-duplex**, meaning:
- All nodes share the same physical medium.
- Data flows **in one direction at a time**.
- When one node transmits, all others listen simultaneously.

![half-duplex](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/images/Half-duplex-communication.png)

Unlike full-duplex systems (e.g., Ethernet), CAN avoids collisions using an **arbitration mechanism** — nodes monitor the bus and back off when they detect higher-priority messages.

---

## 5. Asynchronous Transmission

CAN is **asynchronous**, meaning:
- There is **no shared clock line**.
- Each node uses its **own local oscillator**.
- Synchronization is achieved through **bit timing segments** and **edge sampling** within the CAN protocol.

Every frame starts with a **Start-of-Frame (SOF)** bit, which synchronizes all nodes before data transmission.

This design allows devices running at slightly different clock frequencies to communicate reliably.

---

## 6. Message Arbitration

When multiple nodes attempt to transmit simultaneously:
1. Each node sends its **message ID** bit by bit.
2. The bus operates on a **wired-AND** principle:
   - If any node transmits a `0` (dominant), it overrides all `1`s (recessive).
3. Nodes monitor the bus in real-time:
   - If a node sends a `1` but reads a `0`, it knows a higher-priority message is on the bus and **backs off** immediately.

Result:  
✅ The node with the **lowest message ID** (highest priority) continues transmitting without collision.

---

## 7. CAN Data Frame Structure

Each CAN message is called a **frame**.  
In Classical CAN, a standard frame includes:

![can-frame](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/images/CANFrame.png)

| Field | Bits | Description |
|:--|:--|:--|
| **Start of Frame (SOF)** | 1 | Marks the beginning of a message (dominant bit). |
| **Identifier** | 11 | Defines message priority (lower = higher priority). |
| **RTR (Remote Transmission Request)** | 1 | Differentiates data and request frames. |
| **IDE (Identifier Extension)** | 1 | `0` for standard 11-bit, `1` for extended 29-bit ID. |
| **r0 (Reserved)** | 1 | Reserved bit (dominant). |
| **Data Length Code (DLC)** | 4 | Number of data bytes (0–8). |
| **Data Field** | 0–64 | The actual payload (0–8 bytes in Classic CAN). |
| **CRC Field** | 15 + 1 | Error-detection code plus delimiter. |
| **ACK Field** | 2 | Acknowledgment bits (sent by receivers). |
| **End of Frame (EOF)** | 7 | Marks the end of transmission (recessive bits). |
| **Inter-Frame Space** | 3+ | Bus idle period before next frame. |

---

## 8. Summary of Key Properties

| Property | Description |
|:--|:--|
| **Signaling** | Differential pair (CAN_H / CAN_L) |
| **Communication** | Half-duplex, multi-master |
| **Transmission** | Asynchronous (self-clocking) |
| **Frame length** | 0–8 bytes (Classic CAN) |
| **Bus speed** | Up to 1 Mbit/s (Classic)|
| **Topology** | Linear bus with 120 Ω termination |
| **Error handling** | Automatic detection, retransmission, and node isolation |

---

## 9. Why It Matters in Automotive Systems

- **Low cost & reliability:** Proven over decades in automotive and industrial environments.
- **Deterministic timing:** Critical for safety functions (ABS, engine control).  
- **Shared bus:** Dozens of ECUs communicate over the same wires.  
- **Prioritized IDs:** Ensures urgent signals (e.g., brake) always transmit first.  


---

## 10. References
 
- Microchip, *MCP2551 Datasheet*  
- [Texas Instruments – Understanding and Applying CAN Bus](https://www.ti.com/lit/an/sloa101b/sloa101b.pdf)

---
