# CAN Protocol - Application-Layer Protocol

#### 1. Purpose

This document defines the **application-layer agreement** used for CAN communication between the **STM32 microcontroller** and the **Raspberry Pi gateway**.
The goal is to:

- transmit vehicle data in a **deterministic, compact, and automotive-style** manner
- decouple low-level CAN transport from high-level applications
- enable easy mapping to **VSS / KUKSA Databroker**

Basically we need to define **message ID's**, **data payload**, **units & scaling**, **direction** and **DLC** (length).

---
#### 2. General Notes

- **Bus:** Classic CAN (11-bit IDs)
- **Endianness:** Little-endian for all multi-byte values
- **Units & Scaling:**
	- km/h × 10 → value 253 = 25.3 km/h
	- ° × 10 → value -250 = -25.0°
	- V × 100 → value 1250 = 12.50 V
	- A × 100 → value 123 = 1.23 A
- **Direction:**
	- `STM32 → RPi` : Sensor/vehicle data
	- `RPi → STM32` : Commands from Raspberry Pi

---

#### 3. Message Table

| ID (hex) | Name                    | Direction    | DLC | Payload                                                                 | Description |
|----------|-------------------------|--------------|------------------|-----|-----------------------------------------------------------------------------------------------------------------------------------------------|
| 0x100 | SPEED | STM32 → RPi | 2 | `uint16` (2 bytes) example: speed: 23.4 km/h; raw value = 234; speed_kmh = raw / 10 | Speed value read from the speedometer. |
| 0x111 | TEMPERATURE | STM32 → RPi | 2 | `int16` (2 bytes) example: speed: 23.4 °C; raw value = 234; temp = raw / 10 | Temperature measured from the temperature sensor on teh STM32. |
| 0x112 | HEARTBEAT | STM32 → RPi | 1 | `uint8` (1 byte) counter -> raw value is the counter. Wraps around 255 | Counter increments on each transmission. Used to detect: MCU liveness; communication dropouts; MCU resets |
| 0x201 | MOTOR_CMD | RPi → STM32 | 2 | `uint16` (2 byte) value of accelaration to the DC motors | Command sent from the RPi to the STM32 to give accelatation to the motors. |
| 0x202 | STEER_CMD | RPi → STM32 | 2 | `uint16` (2 byte) value of angle of steer | Command sent from the RPi to the STM32 to give steering direction to the servo-motors. |

---

### 4. Messages Handlers


**Multi-byte data is read with little-endian encoding.**

**Example** (Speed message 0x100, DLC=2):

**Payload bytes:** data[0] = 0xEA, data[1] = 0x00

Read little-endian uint16 with bit-shift:

```
const std::uint16_t raw = u16_le(frame.data);
const double speed = raw / 10.0;
```

So `0x00EA = 234 → 234 / 10.0 = 23.4 km/h.

So there are helper functions that do this bit-shift operations:

`std::uint16_t u16_le(const std::uint8_t* d)`

`std::int16_t i16_le(const std::uint8_t* d)`

---

### IDs (work in progess - future IDs)

#### STM32 -> Raspberry Pi 5

- 0X100 - SPEED
- 0x111 - TEMPERATURE
- 0x112 - HEARTBEAT
- 0x120 - STEER
- 0x130 - MOTOR_STATUS
- 0x140 - POWER_STATUS
- 0x150 - HUMIDITY

#### Raspberry Pi 5 -> STM32
- 0x200 - CTRL_MODE - Select the mode (autonomous or manual).
- 0x210 - MOTOR_CMD - Send command to motors (PWM).
- 0x220 - STEER_CMD - Send command to servo-motors.
- 0x230 - ESTOP_CMD - Send command to stop the car.
