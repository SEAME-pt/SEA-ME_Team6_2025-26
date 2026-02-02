# 🚗 Sensor Integration from Sensor to Kuksa - SEA:ME Project

---

## 🔌 Sensors Integrated in the STM32 (I2C)

### 1️⃣ **ISM330DHCX** - 6-Axis IMU
**Communication:** I2C (onboard STM32)  
**Function:** Inertial Measurement Unit

**Data sent via CAN → Kuksa:**
- ✅ Longitudinal Acceleration (X axis) - up to ±16g
- ✅ Lateral Acceleration (Y axis) - up to ±16g
- ✅ Vertical Acceleration (Z axis) - up to ±16g
- ✅ Roll Rate (X rotation) - up to ±2000 dps
- ✅ Pitch Rate (Y rotation) - up to ±2000 dps
- ✅ Yaw Rate (Z rotation) - up to ±2000 dps
- ✅ Sensor Temperature
- ✅ Motion and Free Fall Detection
- 📊 **Refresh Rate:** 100 Hz
- 🎯 **VSS Signals:** `Vehicle.Acceleration.Longitudinal`, `Vehicle.Acceleration.Lateral`, `Vehicle.Acceleration.Vertical`, `Vehicle.AngularVelocity.Roll`, `Vehicle.AngularVelocity.Pitch`, `Vehicle.AngularVelocity.Yaw`

---

### 2️⃣ **HTS221** - Environmental Sensor
**Communication:** I2C (onboard STM32)  
**Function:** Temperature and Relative Humidity

**Data sent via CAN → Kuksa:**
- ✅ Ambient temperature (-40°C to +120°C)
- ✅ Relative humidity (0% to 100%)
- 📊 **Update rate:** 1 Hz
- 🎯 **VSS Signal:** `Vehicle.Exterior.AirTemperature`, `Vehicle.Exterior.Humidity`

---

### 3️⃣ **LPS22HH** - Barometric Sensor
**Communication:** I2C (onboard STM32)  
**Function:** Atmospheric Pressure and Altitude

**Data sent via CAN → Kuksa:**
- ✅ Barometric pressure (260-1260 hPa)
- ✅ Sensor temperature
- ✅ Estimated altitude (derived from pressure)
- 📊 **Update rate:** 10 Hz
- 🎯 **VSS Signal:** `Vehicle.Exterior.AtmosphericPressure`, `Vehicle.CurrentLocation.Altitude`

---

### 4️⃣ **IIS2MDC** - Magnetometer
**Communication:** I2C (onboard STM32)  
**Function:** 3-Axis Digital Compass

**Data sent via CAN → Kuksa:**
- ✅ Magnetic field (X, Y, Z) - ±50 gauss (raw data, internal processing)
- ✅ Calculated Heading/Azimuth (0-360°)
- ✅ Tilt compensation (with IMU data fusion)
- 📊 **Update rate:** 20 Hz
- 🎯 **VSS Signal:** `Vehicle.CurrentLocation.Heading`

**Note:** Raw magnetometer values (X, Y, Z) are processed internally and not published directly to VSS. Only the fused heading is published.

---

### 5️⃣ **VL53L5CX** - Time-of-Flight Multizone
**Communication:** I2C (onboard STM32)  
**Function:** 8x8 Zone Distance Sensor  
**⚠️ ORIENTATION:** Points UP (STM32 mounted in base position)

**Data sent via CAN → Kuksa:**
- ✅ 8x8 distance matrix (up to 4 meters)
- ✅ Detection of objects above the vehicle
- ✅ Mapping of upper environment (tunnels, garages, roofs)
- ✅ Vertical proximity sensor
- 📊 **Update rate:** 15 Hz
- 🎯 **VSS Signal:** Internal processing only - no direct VSS mapping

**Note:** Because the STM32 is mounted horizontally with sensors facing upwards, the VL53L5CX detects objects above, not frontally. For frontal detection, the ultrasonic SRF08 is used. The 8x8 ToF matrix is processed internally for upper clearance detection. Only critical warnings are published via custom signals or integrated into existing ADAS paths if needed.

---

### 6️⃣ **VEML6030** - Ambient Light Sensor
**Communication:** I2C (onboard STM32)  
**Function:** Light Measurement

**Data sent via CAN → Kuksa:**
- ✅ Light intensity (0-120k lux)
- ✅ Day/night detection
- ✅ Ambient lighting level
- 📊 **Update rate:** 2 Hz
- 🎯 **VSS Signal:** `Vehicle.Exterior.LightIntensity`

---

### 7️⃣ **INA226** - Power Monitor
**Communication:** I2C (external - connected to STM32)  
**Location:** Between BMS and system (monitoring from the main battery)

**Data sent via CAN → Kuksa:**
- ✅ Battery voltage (0-36V, accuracy ±1.25mV)
- ✅ System current (±20A, accuracy ±2.5mA)
- ✅ Instantaneous power (W)
- ✅ Stored energy (Wh)
- ✅ Estimated State of Charge (%)
- ✅ Remaining range (meters)
- 📊 **Refresh rate:** 5 Hz
- 🎯 **VSS Signal:** `Vehicle.Powertrain.TractionBattery.StateOfCharge.Current`, `Vehicle.Powertrain.TractionBattery.StateOfCharge.Displayed`, `Vehicle.Powertrain.TractionBattery.CurrentVoltage`, `Vehicle.Powertrain.TractionBattery.CurrentCurrent`, `Vehicle.Powertrain.TractionBattery.CurrentPower`, `Vehicle.Powertrain.TractionBattery.Range`, `Vehicle.Powertrain.TractionBattery.IsLevelLow`, `Vehicle.Powertrain.TractionBattery.IsCritical`

---

### 8️⃣ **SRF08** - Ultrasonic Sensor
**Communication:** I2C (external - connected to STM32)  
**Location:** Mounted on the FRONT of the vehicle (frontal detection)  
**Range:** 3 cm to 6 meters

**Data sent via CAN → Kuksa:**
- ✅ Distance to frontal obstacle (mm)
- ✅ Echo time (µs)
- ✅ Proximity alert (danger zone < 300mm)
- ✅ Multiple echo detection (up to 17 echoes)
- ✅ Light intensity reading (integrated ALS sensor)
- 📊 **Update rate:** 10-20 Hz (adjustable by range)
- 🎯 **VSS Signal:** `Vehicle.ADAS.ObstacleDetection.Front.Distance`, `Vehicle.ADAS.ObstacleDetection.Front.IsWarning`, `Vehicle.ADAS.ObstacleDetection.IsEnabled`

**Technical Specifications:**
- Aperture Angle: ~55° cone
- Resolution: 1 cm
- Ultrasonic Frequency: 40 kHz
- Programmable I2C Address: 0xE0-0xFE (default 0xE0)

**Advantages over ToF for frontal use:**
- ✅ Greater range (6m vs 4m)
- ✅ Works well on uneven surfaces
- ✅ Not affected by direct sunlight
- ✅ Better detection of soft/absorbent objects

---

### 9️⃣ **Hall Effect Speed Sensor**
**Communication:** I2C (external - connected to STM32)  
**Function:** Wheel RPM and Speed Measurement

**Data sent via CAN → Kuksa:**
- ✅ Wheel RPM
- ✅ Linear vehicle speed (km/h)
- ✅ Distance traveled (odometer)
- ✅ Trip distance counter
- ✅ Average speed
- ✅ Movement/stop detection
- 📊 **Update rate:** 20 Hz
- 🎯 **VSS Signal:** `Vehicle.Speed`, `Vehicle.TraveledDistance`, `Vehicle.TraveledDistanceSinceStart`, `Vehicle.AverageSpeed`, `Vehicle.IsMoving`, `Vehicle.Chassis.WheelSpeed`

---

## 🎯 VSS Mapping (Vehicle Signal Specification)

### Main Signals Published to Kuksa.val

```yaml
Vehicle:
  # Speed and Movement
  Speed: [Hall Sensor]
  AverageSpeed: [Hall Sensor - calculated]
  TraveledDistance: [Hall Sensor - odometer]
  TraveledDistanceSinceStart: [Hall Sensor - trip counter]
  IsMoving: [Hall Sensor - boolean]
  
  # Acceleration (IMU)
  Acceleration:
    Longitudinal: [ISM330DHCX - accel X in m/s²]
    Lateral: [ISM330DHCX - accel Y in m/s²]
    Vertical: [ISM330DHCX - accel Z in m/s²]
  
  # Angular Velocity (Gyroscope)
  AngularVelocity:
    Roll: [ISM330DHCX - gyro X in °/s]
    Pitch: [ISM330DHCX - gyro Y in °/s]
    Yaw: [ISM330DHCX - gyro Z in °/s]
  
  # Navigation and Location
  CurrentLocation:
    Heading: [IIS2MDC + IMU fusion - 0-360°, 0=North]
    Altitude: [LPS22HH - derived from atmospheric pressure]
    Latitude: [GPS - future implementation]
    Longitude: [GPS - future implementation]
  
  # Chassis
  Chassis:
    WheelSpeed: [Hall Sensor]
    SteeringWheel:
      Angle: [Servo position feedback - future]
    Accelerator:
      PedalPosition: [Motor controller - future, 0-100%]
    Brake:
      PedalPosition: [Motor controller - future, 0-100%]
  
  # Environmental Sensors
  Exterior:
    AirTemperature: [HTS221 - in °C]
    Humidity: [HTS221 - in %]
    AtmosphericPressure: [LPS22HH - in kPa]
    LightIntensity: [VEML6030 - in lux]
  
  # Powertrain
  Powertrain:
    TractionBattery:
      StateOfCharge:
        Current: [INA226 - in %]
        Displayed: [INA226 - filtered for UI, in %]
      CurrentVoltage: [INA226 - in V]
      CurrentCurrent: [INA226 - in A]
      CurrentPower: [INA226 - in W]
      Range: [INA226 - calculated, in meters]
      IsLevelLow: [INA226 - boolean, <20%]
      IsCritical: [INA226 - boolean, <10%]
    
    ElectricMotor:
      IsRunning: [Motor controller - future]
      Speed: [Motor controller - RPM]
      Power: [INA226 or motor controller - W]
      Temperature: [Motor temp sensor - future]
    
    Transmission:
      CurrentGear: [Controller - 0=Neutral, 1=Forward, -1=Reverse]
  
  # ADAS (Advanced Driver Assistance Systems)
  ADAS:
    ObstacleDetection:
      IsEnabled: [System state - boolean]
      Front:
        Distance: [SRF08 - in mm]
        IsWarning: [SRF08 - boolean, <300mm]
    
    LaneDepartureDetection:
      Status: [Camera Module 3 + AI]
      IsWarning: [Camera + AI - boolean]
    
    ObjectDetection:
      Objects: [Camera Module 3 + Hailo-8 - array of detected objects]
    
    ActiveAutonomyLevel: [System - SAE_0/SAE_1/SAE_2]
  
  # ECUs (Electronic Control Units)
  ECU:
    ApplicationDomain:  # Raspberry Pi 5 (AGL)
      IsConnected: [CAN heartbeat - boolean]
      Heartbeat: [Counter]
      Temperature: [System monitor - °C]
      CPULoad: [System monitor - %]
      MemoryUsage: [System monitor - %]
    
    SafetyCritical:  # STM32 B-U585I-IOT02A
      IsConnected: [CAN heartbeat - boolean]
      Heartbeat: [Counter]
      Temperature: [STM32 internal temp sensor - °C]
      ErrorCode: [Diagnostic code]
    
    Display:  # Raspberry Pi 4 (Display Unit)
      IsConnected: [CAN/MQTT heartbeat - boolean]
      Heartbeat: [Counter]
  
  # AI Accelerator
  AIAccelerator:  # Hailo-8 (26 TOPS)
    IsConnected: [System state - boolean]
    IsEnabled: [Power state - boolean]
    IsInferenceActive: [Processing state - boolean]
    Temperature: [Hailo monitor - °C]
    Power: [INA226 or Hailo monitor - W]
    InferenceRate: [FPS counter]
    CurrentModel: [Model name - string]
    Utilization: [Percentage 0-100]
  
  # Low Voltage Power Rails
  LowVoltageBattery:
    Rail5V:
      CurrentVoltage: [Voltage monitor - future, in V]
      CurrentCurrent: [Current monitor - future, in A]
    Rail3V3:
      CurrentVoltage: [Voltage monitor - future, in V]
      CurrentCurrent: [Current monitor - future, in A]
  
  # Power Relays
  Relay:
    Main:
      IsOn: [Relay controller - boolean]
    Motor:
      IsOn: [Relay controller - boolean]
    Display:
      IsOn: [Relay controller - boolean]
    AIAccelerator:
      IsOn: [Relay controller - boolean]
  
  # Communication
  Communication:
    CAN:
      IsConnected: [CAN interface state - boolean]
      ErrorCount: [CAN error counter]
      BusLoad: [Calculated percentage 0-100]
      MessageRate: [Messages per second]
  
  # Diagnostics
  Diagnostics:
    DTCCount: [Diagnostic Trouble Code counter]
    DTCList: [String array of DTC codes]
  
  # Vehicle Identification
  VehicleIdentification:
    Brand: "SEA:ME"
    Model: "Team6"
    Year: 2025
  
  # System State
  LowVoltageSystemState: [OFF/ON/READY - string]
```

---

## 📊 Sensor-to-VSS Mapping Summary

| Sensor | Primary VSS Signals | Update Rate |
|--------|---------------------|-------------|
| **ISM330DHCX** | `Acceleration.*`, `AngularVelocity.*` | 100 Hz |
| **HTS221** | `Exterior.AirTemperature`, `Exterior.Humidity` | 1 Hz |
| **LPS22HH** | `Exterior.AtmosphericPressure`, `CurrentLocation.Altitude` | 10 Hz |
| **IIS2MDC** | `CurrentLocation.Heading` | 20 Hz |
| **VL53L5CX** | Internal processing (ceiling clearance) | 15 Hz |
| **VEML6030** | `Exterior.LightIntensity` | 2 Hz |
| **INA226** | `Powertrain.TractionBattery.*` | 5 Hz |
| **SRF08** | `ADAS.ObstacleDetection.Front.*` | 10-20 Hz |
| **Hall Sensor** | `Speed`, `TraveledDistance`, `IsMoving`, etc. | 20 Hz |

---

## 🔄 Data Flow Pipeline

```
STM32 Sensors (I2C) 
    ↓
ThreadX RTOS Processing
    ↓
CAN Message Formatting (VSS-compliant)
    ↓
CAN Bus @ 500 kbps
    ↓
Raspberry Pi 5 (MCP2515 via SPI)
    ↓
SocketCAN (Linux kernel)
    ↓
Kuksa.val Databroker
    ↓
VSS Signal Tree
    ↓
Dashboard / Applications (Qt6, MQTT, gRPC)
```

---

**Project:** SEA:ME - Software Engineering for Autonomous Mobility Ecosystems  
**Institution:** 42 School  
**Team:** Team 6  
**Year:** 2025  
**Architecture:** Dual-Tier (STM32 ThreadX RTOS + Raspberry Pi 5 AGL)  
**VSS Compliance:** COVESA Vehicle Signal Specification
