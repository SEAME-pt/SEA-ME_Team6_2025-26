# ✅ Simple CAN Communication Test — Raspberry Pi 5 (AGL) ↔ STM32 (ThreadX)

This test verifies that the **Raspberry Pi 5** can send a CAN frame to the **STM32**, and that the STM32 can correctly **receive it and send back an ACK frame**.

It is meant as a **minimal working communication proof** before implementing more complex CAN protocols.

---

🔧 Important:
Any STM32-side source files in this folder are meant to be added to an existing STM32CubeIDE project that is already fully configured (clock, SPI, GPIO, ThreadX, MCP2515, etc.).
This README does not cover the full CubeMX / CubeIDE configuration — it assumes that part is done and the project can already build and run on the STM32.

---

## 📡 Overview

- The **Pi 5** runs a small C program using **SocketCAN**.  
- Every second, the Pi:
  1. Sends a CAN frame with:
     - **ID:** `0x100`  
     - **Payload:** `0x01`
  2. Waits for any incoming CAN frame.
  3. Prints the received frame (expected: ACK from STM32).

- The **STM32** (via MCP2515 driver) listens for frames with ID `0x100`.  
  When it receives one, it sends back an **ACK** frame:
  - **ID:** `0x101`  
  - **Payload:** e.g. `0xAC`

This test confirms **bidirectional CAN communication** is working.

---

## 🖥️ Raspberry Pi Program (AGL)

### ✔ What the code does

1. Opens a CAN RAW socket (`PF_CAN` + `SOCK_RAW`).
2. Binds the socket to the interface **can0**.
3. Enters an infinite loop:
   - Sends a frame:  
     ```text
     ID = 0x100  
     DLC = 1  
     DATA = 0x01  
     ```
   - Reads any received frame.
   - Prints it to the terminal.

## ▶️ How to Run the Test

### 1. Bring up the CAN interface on AGL (Raspberry Pi 5)

```bash
ip link set can0 up type can bitrate 500000
````

### 2. Compile the Pi program

```bash
gcc can_test.c -o can_test
````

### 3. Run it

```bash
./can_test
````

### Expected output example

```bash
CAN socket bound to can0
Sent CAN ID 0x100 data 0x01
Received CAN ID 0x101 len=1 data=AA
Sent CAN ID 0x100 data 0x01
Received CAN ID 0x101 len=1 data=AA
...

````