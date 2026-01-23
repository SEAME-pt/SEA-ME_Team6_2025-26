# SEA:ME PiRacer - Circuit Connection Diagram

## Overview

This document describes the electrical/comunication connections and architecture of the SEA:ME autonomous vehicle platform built on the PiRacer chassis. 
The system uses a dual-tier architecture with an STM32 microcontroller handling real-time safety-critical operations and a Raspberry Pi 5 managing high-level processing and AI inference.

---

## System Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                         POWER SYSTEM                                │
│  3S 18650 Battery → BMS 3S 40A → Switch → INA226 → Step-down Rails  │
└─────────────────────────────────────────────────────────────────────┘
                                    │
            ┌───────────────────────┼───────────────────────┐
            ▼                       ▼                       ▼
    ┌───────────────┐      ┌───────────────┐      ┌───────────────┐
    │   5V 5A Rail  │      │   5V 3A Rail  │      │  Motor Power  │
    │  (RPi5, LCD)  │      │ (STM32, CAN)  │      │   (Direct)    │
    └───────────────┘      └───────────────┘      └───────────────┘
```

---

## Components List

### Power Management

| Component | Function | Specifications |
|-----------|----------|----------------|
| 3S 18650 Battery Pack | Main power source | 11.1V nominal, ~2200mAh per cell |
| BMS 3S 40A | Battery protection & balancing | Over-discharge/charge protection |
| INA226 | Power monitoring | Voltage, current, power measurement via I2C |
| Step-down 5V 5A | High-current 5V rail | Powers RPi5, display |
| Step-down 5V 3A | Secondary 5V rail | Powers STM32 peripherals |
| Recharge Terminal | Battery charging port | External charger connection |
| Main Switch | System on/off | Inline with battery output |

### Processing Units

| Component | Role | Key Features |
|-----------|------|--------------|
| **Raspberry Pi 5** | High-level processing | AGL Linux, Kuksa.val, MQTT |
| **Hailo-8 HAT** | AI acceleration | 26 TOPS for computer vision |
| **STM32 B-U585I-IOT02A** | Real-time control | ThreadX RTOS, sensor fusion |

### Communication

| Component | Protocol | Connection |
|-----------|----------|------------|
| MCP2515 | CAN Bus | STM32 ↔ CAN network |
| MCP2518 (CAN HAT) | CAN Bus | RPi5 ↔ CAN network |
| I2C Hub | I2C | Multi-device I2C expansion |

### Actuators

| Component | Function | Control Interface |
|-----------|----------|-------------------|
| Motor Driver (TB6612FNG) | Dual DC motor control | PWM from STM32 |
| 2× DC Motors | Propulsion | Via motor driver |
| Servo Motor | Steering | PWM from STM32 |
| 3V Relay | Power switching | GPIO controlled |

### Sensors

| Component | Measurement | Interface |
|-----------|-------------|-----------|
| Speedometer (Hall Effect) | Wheel rotation/speed | Digital pulse to STM32 |
| SRF08 Ultrasonic | Distance measurement | I2C |
| Camera Module 3 | Visual input for AI | CSI (RPi5) |
| INA226 | Power consumption | I2C |

### Display

| Component | Type | Interface |
|-----------|------|-----------|
| LCD 1602 RGB | 16×2 character display | I2C (via I2C Hub) |

---

## Connection Details

### Power Distribution

```
Battery (11.1V)
    │
    ├──► BMS 3S 40A (Protection)
    │        │
    │        ├──► Switch (Main power control)
    │        │        │
    │        │        ├──► INA226 (Power monitoring)
    │        │        │        │
    │        │        │        ├──► Step-down 5V 5A
    │        │        │        │        ├──► Raspberry Pi 5
    │        │        │        │        └──► LCD Display
    │        │        │        │
    │        │        │        └──► Step-down 5V 3A
    │        │        │                 ├──► STM32
    │        │        │                 ├──► MCP2515
    │        │        │                 └──► I2C Hub
    │        │        │
    │        │        └──► Motor Driver (VM)
    │        │                 └──► DC Motors
    │        │
    │        └──► Servo Motor (direct battery or regulated)
    │
    └──► Recharge Terminal
```

### CAN Bus Network

```
┌─────────────┐         CAN_H / CAN_L         ┌─────────────┐
│   STM32     │◄─────────────────────────────►│ Raspberry   │
│  MCP2515    │                               │ Pi 5        │
│             │                               │ MCP2518     │
└─────────────┘                               └─────────────┘
      │                                             │
      │ SPI                                         │ SPI
      ▼                                             ▼
┌─────────────┐                               ┌─────────────┐
│ STM32 MCU   │                               │    RPi5     │
│ B-U585I     │                               │   (AGL)     │
└─────────────┘                               └─────────────┘
```

### I2C Bus Devices

Connected via I2C Hub:
- LCD 1602 RGB Display
- SRF08 Ultrasonic Sensor
- INA226 Power Monitor
- Motor Driver TB6612FNG

**Note:** The STM32 B-U585I-IOT02A also has onboard I2C sensors (IMU, temperature, pressure, etc.) on its internal I2C buses.

### STM32 GPIO Allocation

| Function | GPIO/Peripheral | Notes |
|----------|-----------------|-------|
| Motor PWM | TIM channels | TB6612FNG control |
| Servo PWM | TIM channel | Steering control |
| Speedometer | EXTI / TIM input capture | Hall effect pulse counting |
| CAN (MCP2515) | SPI + GPIO (CS, INT) | CAN bus interface |
| Relay | GPIO output | Power switching |
| I2C | I2C1/I2C2 | Sensor communication |

### Raspberry Pi 5 Connections

| Function | Interface | Notes |
|----------|-----------|-------|
| Hailo-8 HAT | PCIe / GPIO | AI accelerator |
| CAN HAT (MCP2518) | SPI0 | CAN bus interface |
| Camera Module 3 | CSI | Computer vision input |
| Display (optional) | HDMI/DSI | Dashboard display |

---

## Signal Flow

### Sensor Data Flow
```
Physical World
      │
      ▼
┌─────────────────────────────────────────────────┐
│              SENSORS                            │
│  Hall Sensor → STM32 (speed calculation)        │
│  SRF08 → I2C → STM32 (distance)                │
│  Camera → RPi5 (visual processing)              │
│  Onboard IMU → STM32 (orientation)              │
│  INA226 → I2C → monitoring (power status)       │
└─────────────────────────────────────────────────┘
      │
      ▼ CAN Bus
┌─────────────────────────────────────────────────┐
│           DATA PROCESSING                       │
│  STM32: Real-time sensor fusion                 │
│  RPi5: AI inference, path planning              │
│  Kuksa.val: VSS signal management               │
└─────────────────────────────────────────────────┘
      │
      ▼ CAN Bus / PWM
┌─────────────────────────────────────────────────┐
│              ACTUATORS                          │
│  Motor Driver → DC Motors (propulsion)          │
│  Servo → Steering mechanism                     │
│  Relay → Auxiliary power control                │
└─────────────────────────────────────────────────┘
```

### CAN Message Flow
```
STM32 (Sender)                    RPi5 (Receiver)
┌────────────────┐               ┌────────────────┐
│ Sensor Thread  │──── CAN ────► │ Kuksa.val      │
│ - Speed        │               │ Databroker     │
│ - Distance     │               │                │
│ - IMU data     │               │                │──► Dashboard
│ - Temperature  │               │                │
└────────────────┘               └────────────────┘

RPi5 (Sender)                     STM32 (Receiver)
┌────────────────┐               ┌────────────────┐
│ Control Logic  │──── CAN ────► │ Motor Control  │
│ - Throttle cmd │               │ Thread         │
│ - Steering cmd │               │                │
│ - Mode changes │               │ Servo Control  │
└────────────────┘               └────────────────┘
```

---

## Power Consumption Summary

| Component     | Typical (W)       | Peak (W) |
|---------------|-------------------|----------|
| Rpi5  | 5-8 | 25 |
| Hailo-8 HAT | 2.5 | 12.5 |
| STM32 B-U585I | 0.5 | 1.65 |
| DC Motors (2×) | 5 | 48 |
| Servo Motor | 1 | 7.2 |
| Display LCD | 0.5 | 1 |
| Sensors & Peripherals | 1 | 2 |
| **Total** | **~15-18** | **~97** |

**Estimated Runtime (3S 2200mAh):**
- Idle: ~1.5 hours
- Normal operation: ~25-30 minutes
- Consider 3S2P or 3S3P configuration for extended testing

---

## References

- [STM32 B-U585I-IOT02A User Manual](https://www.st.com/resource/en/user_manual/um2839.pdf)
- [Raspberry Pi 5 Documentation](https://www.raspberrypi.com/documentation/)
- [TB6612FNG Datasheet](https://www.sparkfun.com/datasheets/Robotics/TB6612FNG.pdf)
- [MCP2515 CAN Controller](https://ww1.microchip.com/downloads/en/DeviceDoc/MCP2515-Stand-Alone-CAN-Controller-with-SPI-20001801J.pdf)
- [COVESA VSS Specification](https://covesa.github.io/vehicle_signal_specification/)
- [2-Channel CAN-BUS HAT (MCP2518FD)](https://wiki.seeedstudio.com/2-Channel-CAN-BUS-FD-Shield-for-Raspberry-Pi/)
- [INA 226 Power Monitor Documentation](https://strawberry-linux.com/pub/ina226.pdf)
- [Joy-It Speedometer LM393 Datasheet/Manual](https://joy-it.net/files/files/Produkte/SEN-Speed/SEN-Speed_Manual_2024-05-03.pdf)
- [SRF08 Ultrasonic range finder Datasheet](https://www.robot-electronics.co.uk/htm/srf08tech.html)

---


