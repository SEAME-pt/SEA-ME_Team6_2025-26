# Wire Connections in Car

This document covers two topics about things connected by wires in TEAM6 Car:



1. #### **Communication Protocols** — How the components in the car talk to each other (I2C, SPI, UART): what each protocol is, how it works, and where we use it.

   ![wiredConnection_lightTheme](https://github.com/user-attachments/assets/44f5b356-1969-4560-87cf-b150abed4986)

2. #### **Power Distribution** — How electricity flows from the battery to every component: the 3S LiPo setup, the step-down converters, voltage rails, and wiring practices.



---

## 1. Communication Protocols Overview

The PiRacer platform uses three serial communication protocols to connect its components. Each protocol serves a different purpose depending on the speed, distance, and complexity requirements of the connection.

### 1.1 I2C (Inter-Integrated Circuit)

![i2c](https://github.com/user-attachments/assets/8852b827-9ebb-455b-bbe6-e8d07ca73b3d)

I2C is a synchronous, half-duplex protocol that uses just two wires:

- **SDA** (Serial Data Line) — carries the data
- **SCL** (Serial Clock Line) — carries the clock signal

Communication is always initiated by a master device toward one or more slaves. Each slave has a unique 7-bit address on the bus, so multiple devices can share the same two wires.

**How it works:**

1. The master pulls SDA low while SCL is high (START condition)
2. The master sends the slave address + a read/write bit
3. The addressed slave responds with an ACK (pulls SDA low for one clock cycle)
4. Data is exchanged byte-by-byte, with an ACK after each byte
5. The master pulls SDA high while SCL is high (STOP condition)

**Strengths:**

- Only 2 wires for multiple devices
- Simple wiring and addressing
- Good for short-distance, low-speed sensor communication

**Weaknesses:**

- Limited speed (typically up to a few Mbit/s)
- Sensitive to noise over long wires
- Half-duplex — cannot send and receive simultaneously

**On our car:**

| Connection | Master | Slave | Address | Purpose |
|-----------|--------|-------|---------|---------|
| IMU (ISM330DHCX) | STM32 | ISM330DHCX | 0x6A | Accelerometer + gyroscope data |
| Motor driver (TB6612FNG) | STM32 | Grove ATmega8 | 0x28 | DC motor speed/direction commands |
| Power sensor (INA226) | STM32 | INA226 | 0x40 | Battery voltage and current monitoring |
| Ultrasonic (SRF08) | STM32 | SRF08 | 0x70 | Distance measurement for AEB |

All I2C devices on the STM32 share the same SDA/SCL bus. The STM32 acts as the single master.

---

### 1.2 SPI (Serial Peripheral Interface)

![spi](https://github.com/user-attachments/assets/dfd7c491-d42a-4b69-960c-9364f1e8a49c)

SPI is a synchronous, full-duplex protocol that uses four wires:

- **MOSI** (Master Out Slave In) — data from master to slave
- **MISO** (Master In Slave Out) — data from slave to master
- **SCK** (Serial Clock) — clock signal from master
- **SS/CS** (Slave Select / Chip Select) — one per slave, active low

Because it has separate lines for sending and receiving, SPI can transmit data in both directions simultaneously (full-duplex).

**How it works:**

1. The master pulls the target slave's SS line low to select it
2. The master generates a clock signal on SCK
3. Data is shifted out on MOSI (master → slave) and simultaneously in on MISO (slave → master)
4. When done, the master releases SS back to high

**Strengths:**

- High speed (several Mbit/s to tens of Mbit/s)
- Full-duplex — simultaneous send and receive
- Simple implementation, no addressing overhead

**Weaknesses:**

- Requires more wires (4 + one SS per additional slave)
- No built-in acknowledgment mechanism
- Not practical for many slaves (pin count grows)

**On our car:**

| Connection | Master | Slave | Purpose |
|-----------|--------|-------|---------|
| CAN controller (MCP2518FD) | RPi5 | MCP2518FD | SPI-to-CAN bridge on the Seeed CAN FD HAT |

The Seeed CAN FD HAT uses SPI to interface the MCP2518FD CAN controller with the RPi5. The actual CAN bus communication is handled by the MCP2518FD chip — SPI is just the link between it and the RPi5's processor.

---

### 1.3 UART (Universal Asynchronous Receiver/Transmitter)

![uart](https://github.com/user-attachments/assets/4325655b-1309-4ff5-8f73-487ad524ea76)

UART is an asynchronous, full-duplex protocol that uses two wires:

- **TX** (Transmit) — data out
- **RX** (Receive) — data in

Unlike I2C and SPI, UART has no clock line. Both sides must agree on the same **baud rate** (bits per second) beforehand. Common baud rates: 9600, 115200.

**How it works:**

1. The line sits high (idle) when no data is being sent
2. The sender pulls the line low for one bit period (START bit)
3. 5–9 data bits are sent, least significant bit first
4. An optional parity bit is sent for error detection
5. One or two STOP bits (line held high) mark the end of the frame

**Strengths:**

- Only 2 wires
- No clock needed — simple point-to-point connection
- Full-duplex communication
- Built-in error detection (parity bit)

**Weaknesses:**

- Point-to-point only — no native multi-device support
- Both sides must use the same baud rate (max 10% tolerance)
- Lower speed than SPI

**On our car:**

UART is primarily used for debug and console access:

| Connection | Device A | Device B | Baud Rate | Purpose |
|-----------|----------|----------|-----------|---------|
| Serial console | RPi5 | Host PC (via USB-UART) | 115200 | AGL boot logs and debug shell |
| STM32 debug | STM32 | Host PC (via ST-LINK VCP) | 115200 | ThreadX debug output |

---

### 1.4 Protocol Comparison

| Feature | I2C | SPI | UART |
|---------|-----|-----|------|
| **Wires** | 2 (SDA, SCL) | 4+ (MOSI, MISO, SCK, SS) | 2 (TX, RX) |
| **Speed** | Up to a few Mbit/s | Up to tens of Mbit/s | Up to ~1 Mbit/s typical |
| **Duplex** | Half-duplex | Full-duplex | Full-duplex |
| **Clock** | Yes (synchronous) | Yes (synchronous) | No (asynchronous) |
| **Multi-device** | Yes (addressing) | Yes (SS lines) | No (point-to-point) |
| **Error detection** | ACK/NACK | None built-in | Parity bit |
| **Best for** | Sensors, low-speed peripherals | High-speed data transfer | Debug consoles, GPS, serial links |

---

## 2. Power Distribution

  <img width="3000" height="2659" alt="image" src="https://github.com/user-attachments/assets/fbecb997-553b-4010-8223-e568b61243ad" />


### 2.1 Battery
 
The car is powered by a **3S LiPo battery** — three lithium-polymer cells connected in series.
 
| Parameter | Value |
|-----------|-------|
| Configuration | 3S (3 cells in series) |
| Nominal voltage | 11.1V (3 × 3.7V) |
| Fully charged | 12.6V (3 × 4.2V) |
| Minimum safe voltage | 9.9V (3 × 3.3V) |
| Capacity | 3200 mAh (3× 18650 cells) |
 
**Important:** Never discharge below 3.3V per cell (9.9V total). Deep discharges permanently damage LiPo cells and reduce battery life. A BMS (Battery Management System) or voltage alarm should be used to prevent over-discharge.
 
### 2.2 Voltage Rails
 
The 11.1V battery voltage is too high for most components. Two 5V step-down (buck) converters regulate it down. Components that need 3.3V (like the STM32) use their onboard voltage regulators from the 5V input.
 
The loads are split across two separate step-downs to avoid overloading a single converter:
 
```
[3S LiPo Battery — 11.1V nominal]
    │
    ├── Direct ──────────────► TB6612FNG motor supply (VM) → DC Motors (×2)
    │
    ├── [Step-Down 5V 5A] ───► 5V Rail A (Application domain)
    │       │
    │       ├── Raspberry Pi 5 (+ Hailo-8 HAT via PCIe)
    │       ├── Seeed CAN FD HAT (MCP2518FD)
    │       ├── SSD M.2 (via USB 3.0 on RPi5)
    │       └── Servo Motor (WS-MG99SR)
    │
    └── [Step-Down 5V 3A] ───► 5V Rail B (Safety-critical domain)
            │
            ├── STM32 B-U585I-IOT02A (onboard reg → 3.3V)
            ├── TB6612FNG logic (VCC)
            ├── MCP2515 CAN transceiver
            ├── INA226 power sensor
            ├── SRF08 ultrasonic sensor
            ├── ISM330DHCX IMU
            └── Hall effect speed sensor
```
 
**Why two separate step-downs instead of one big one?**
 
Splitting the power into two rails isolates the high-power application domain (RPi5 + Hailo-8 inference) from the safety-critical domain (STM32 + sensors + motor driver). If the RPi5 draws a heavy current spike during inference, it won't cause a voltage dip on the STM32 rail. This separation improves system stability and is a good practice in automotive architectures.
 
### 2.3 5V 5A Step-Down — Rail A (Application Domain)
 
This rail powers the RPi5 and everything connected to it.
 
| Parameter | Value |
|-----------|-------|
| Input voltage | 11.1V (from battery) |
| Output voltage | 5.0V |
| Converter rating | 5A continuous |
 
**Why 5A:** The Raspberry Pi 5 alone can draw up to 5A under heavy load (all cores active + Hailo-8 inference + USB peripherals including the SSD). The CAN HAT and servo add relatively little in steady state, but the servo can spike up to ~1.2A under stall. The 5A rating provides the headroom needed to handle these peaks without brownouts or random reboots.
 
### 2.4 5V 3A Step-Down — Rail B (Safety-Critical Domain)
 
This rail powers the STM32 and all its peripherals. The STM32 board has an onboard 3.3V regulator, so it takes 5V in and generates 3.3V internally.
 
| Parameter | Value |
|-----------|-------|
| Input voltage | 11.1V (from battery) |
| Output voltage | 5.0V |
| Converter rating | 3A continuous |
 
The total current demand on this rail is modest. The STM32 draws ~170 mA typical (up to ~500 mA with WiFi active), the TB6612FNG logic draws ~5 mA, and the I2C sensors (INA226, SRF08, ISM330DHCX, Hall sensor) draw a few mA each. Total peak is well under 1A, so the 3A converter has plenty of margin.
 
### 2.5 Motor Power
 
The DC motors are powered directly from battery voltage through the TB6612FNG H-bridge driver. The driver receives its logic power (VCC) from Rail B at 5V, but the motor supply pin (VM) connects straight to the battery. This keeps motor current off both step-down converters.
 
| Motor | Supply | Current | Notes |
|-------|--------|---------|-------|
| DC Motors (×2) | Battery via TB6612FNG (VM) | 1.2A continuous / 3.2A peak per channel | PWM speed control, H-bridge direction |
| Servo (WS-MG99SR) | Rail A (5V) | ~100 mA idle / ~1.2A stall | Connected to 5A step-down |
 
 
These are estimates without the display (not connected). Actual values depend on motor load, inference workload, and servo activity.

---

*Document created for SEA:ME Team 6 — ADAS Module*
