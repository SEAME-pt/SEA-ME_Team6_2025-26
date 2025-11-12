# Power Consumption Analysis - SEA:ME Project (Without Display)

## Component Consumption Table

| Component | Quantity | Voltage (V) | Min Current (mA) | Max Current (mA) | Min Power (W) | Max Power (W) | Notes |
|-----------|----------|--------------|------------------|------------------|---------------|---------------|-------|
| **Raspberry Pi 5** | 1 | 5.0 | 500 | 5000 | 2.5 | 25.0 | Idle: ~3W, Max Performance: 25W |
| **Hailo-8 HAT (26 TOPS)** | 1 | 3.3-5.0 | 200 | 2500 | 0.66 | 12.5 | Typical: 2.5W, Max: ~12.5W |
| **B-U585I-IOT02A** | 1 | 5.0 | 2 | 500 | 0.007 | 1.65 | With WiFi active: ~170mA @ 5V |
| **Servo Motor WS-MG99SR** | 1 | 4.8-6.0 | 100 | 1200 | 0.5 | 7.2 | Idle: ~100mA, Stall: ~1200mA |
| **DC Motors (propulsion)** | 2 | 6.0-12.0 | 200 | 2000 | 2.4 | 48.0 | Per motor: 1.2W-24W (depends on specific motor) |
| **MCP2515 CAN Transceiver** | 2 | 5.0 | 1 | 5 | 0.01 | 0.05 | Typical: 5mA/module, Standby: 1μA |
| **SSD M.2 (via PCIe)** | 1 | 3.3 | 50 | 500 | 0.165 | 1.65 | Depends on specific model |
| **PCI-E to USB Converter** | 1 | 5.0 | 50 | 200 | 0.25 | 1.0 | Passive conversion |
| **Speed Sensor (Hall)** | 1 | 3.3-5.0 | 5 | 15 | 0.025 | 0.075 | Typical Hall effect sensor |
| **LM2596 Step-Down** | 1 | - | - | - | - | 0.4 | Efficiency losses (~92%) |
| **XL4015 Step-Down** | 1 | - | - | - | - | 0.8 | Efficiency losses (~90%) |
| **TB6612FNG Driver** | 1 | 2.7-5.5 (logic) | 5 | 3200 | 0.025 | 16.0 | VCC + VM, 1.2A cont./channel (3.2A peak) |

---

## System Total Summary (WITHOUT DISPLAY)

| Condition | Battery Voltage | Total Current (A) | Total Power (W) | Observations |
|-----------|-----------------|-------------------|-----------------|--------------|
| **Minimum Idle** | 11.1V (3S) | ~1.15 | ~12.8 | All components at rest |
| **Normal (driving)** | 11.1V (3S) | ~5.25 | ~58.3 | Typical operation with motors |
| **Maximum (peak)** | 11.1V (3S) | ~15.2 | ~168.8 | All motors/servo at maximum load |

### Comparison - WITH or WITHOUT Display

| Condition | WITH Display (W) | WITHOUT Display (W) | Savings (W) | Savings (%) |
|-----------|------------------|---------------------|-------------|-------------|
| **Minimum Idle** | 15.5 | 12.8 | 2.7 | 17.4% |
| **Normal** | 61.0 | 58.3 | 2.7 | 4.4% |
| **Maximum** | 172.0 | 168.8 | 3.2 | 1.9% |


## System Voltage Distribution

| Voltage Rail | Source | Powered Components |
|--------------|--------|-------------------|
| **11.1V (Battery)** | 3S LiPo | Main input, DC Motors, Servo |
| **5V** | Step-Down LM2596 | Raspberry Pi 5, MCP2515 (2x), USB Converter |
| **3.3V** | Step-Down XL4015 | B-U585I-IOT02A, Speed Sensor, SSD M.2 |
| **Motors** | Direct battery | TB6612FNG motor supply |



## Detailed Autonomy Calculations (WITHOUT Display)

### Scenario 1: Idle (System on, no movement)
```
Consumption: ~12.8W
Current @ 11.1V: 12.8W / 11.1V = 1.15A

3S (2200mAh):   2.2Ah / 1.15A = 1.91h  ≈ 1h 55min
```

### Scenario 2: Normal (Typical driving)
```
Consumption: ~58.3W
Current @ 11.1V: 58.3W / 11.1V = 5.25A

3S (2200mAh):   2.2Ah / 5.25A = 0.42h  ≈ 25min
```

### Scenario 3: Maximum (All components at 100%)
```
Consumption: ~168.8W
Current @ 11.1V: 168.8W / 11.1V = 15.2A

3S (2200mAh):   2.2Ah / 15.2A = 0.14h  ≈ 9min
```


## Power Distribution Diagram (WITHOUT Display)

```
[3S LiPo Battery 11.1V]
    |
    +-- [Motors] --> TB6612FNG --> DC Motors (2x)
    |
    +-- [Servo Motor WS-MG99SR]
    |
    +-- [LM2596 Step-Down] --> 5V Rail
    |       |
    |       +-- Raspberry Pi 5
    |       +-- MCP2515 (2x)
    |       +-- TB6612FNG (VCC)
    |       +-- USB Converter
    |       +-- SSD M.2
    |       +-- Hailo-8 HAT (via Pi)			
    |
    +-- [XL4015 Step-Down] --> 3.3V Rail
            |
            +-- B-U585I-IOT02A
            +-- Speed Sensor
```


## Final Comparison: WITH vs WITHOUT Display

| Metric | WITH Display | WITHOUT Display | Difference |
|--------|--------------|-----------------|------------|
| **Idle Power** | 15.5W | 12.8W | -2.7W (-17.4%) |
| **Normal Power** | 61.0W | 58.3W | -2.7W (-4.4%) |
| **Maximum Power** | 172.0W | 168.8W | -3.2W (-1.9%) |
| **Idle Autonomy (3S)** | 1h 34min | 1h 55min | +21min |
| **Normal Autonomy (3S)** | 24min | 25min | +1min |
| **LM2596 Load (peak)** | 6.3A | 5.65A | -640mA |
| **No. Components** | 13 | 12 | -1 |
