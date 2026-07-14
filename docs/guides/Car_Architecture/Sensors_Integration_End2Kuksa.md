# 🚗 Sensor Integration from Sensor to Kuksa - SEA:ME Project


---

## 🔌 Sensors Integrated in the STM32 (I2C)

### 1️⃣ **ISM330DHCX** - 6-Axis IMU
**Communication:** I2C (onboard STM32)
**Function:** Inertial Measurement Unit

**Data sent via CAN → Kuksa:**
- ✅ Acceleration (X, Y, Z) - up to ±16g
- ✅ Angular Velocity (X, Y, Z) - up to ±2000 dps
- ✅ Sensor Temperature
- ✅ Motion and Free Fall Detection
- 📊 **Refresh Rate:** 100 Hz
- 🎯 **Signal VSS:** `Vehicle.Chassis.Accelerometer.*`, `Vehicle.Chassis.Gyroscope.*`

---

### 2️⃣ **HTS221** - Environmental Sensor
**Communication:** I2C (onboard STM32)
**Function:** Temperature and Relative Humidity

**Data sent via CAN → Kuksa:**
- ✅ Ambient temperature (-40°C to +120°C)
- ✅ Relative humidity (0% to 100%)
- 📊 **Update rate:** 1 Hz
- 🎯 **VSS signal:** `Vehicle.Cabin.HVAC.AmbientAirTemperature`, `Vehicle.Exterior.Humidity`

---

### 3️⃣ **LPS22HH** - Barometric Sensor
**Communication:** I2C (onboard STM32)
**Function:** Atmospheric Pressure and Altitude

**Data sent via CAN → Kuksa:**
- ✅ Barometric pressure (260-1260 hPa)
- ✅ Sensor temperature
- ✅ Estimated altitude (derived from pressure)
- 📊 **Update rate:** 10 Hz
- 🎯 **VSS signal:** `Vehicle.Exterior.AirPressure`, `Vehicle.CurrentLocation.Altitude`

---

### 4️⃣ **IIS2MDC** - Magnetometer
**Communication:** I2C (onboard STM32)
**Function:** 3-Axis Digital Compass

**Data sent via CAN → Kuksa:**
- ✅ Magnetic field (X, Y, Z) - ±50 gauss
- ✅ Calculated Heading/Azimuth (0-360°)
- ✅ Tilt compensation (with IMU data)
- 📊 **Update rate:** 20 Hz
- 🎯 **VSS signal:** `Vehicle.CurrentLocation.Heading`, `Vehicle.Chassis.Magnetometer.*`

---

### 5️⃣ **VL53L5CX** - Time-of-Flight Multizone
**Communication:** I2C (onboard STM32)
**Function:** 8x8 Zone Distance Sensor
**⚠️ ORIENTATION:** Points UP (STM32 mounted in the correct position) (base)

**Data sent via CAN → Kuksa:**
- ✅ 8x8 distance matrix (up to 4 meters)
- ✅ Detection of objects above the vehicle
- ✅ Mapping of upper environment (tunnels, garages, roofs)
- ✅ Vertical proximity sensor
- 📊 **Update rate:** 15 Hz
- 🎯 **VSS signal:** `Vehicle.ADAS.ObstacleDetection.TopZone`, `Vehicle.Cabin.RoofSensor.*`

**Note:** Because the STM32 is mounted horizontally with sensors facing upwards, the VL53L5CX detects objects above, not frontally. For frontal detection, the ultrasonic SRF08 is used.

---

### 6️⃣ **VEML6030** - Ambient Light Sensor
**Communication:** I2C (onboard STM32)
**Function:** Light Measurement

**Data sent via CAN → Kuksa:**
- ✅ Light intensity (0-120k lux)
- ✅ Day/night detection
- ✅ Ambient lighting level
- 📊 **Update rate:** 2 Hz
- 🎯 **VSS Signal:** `Vehicle.Cabin.Lights.AmbientLight`, `Vehicle.Exterior.LightIntensity`

---

### 7️⃣ **INA226** - Power Monitor
**Communication:** I2C (external - connected to STM32)
**Location:** Between BMS and system (monitoring) (from the main battery)

**Data sent via CAN → Kuksa:**
- ✅ Battery voltage (0-36V, accuracy ±1.25mV)
- ✅ System current (±20A, accuracy ±2.5mA)
- ✅ Instantaneous power (W)
- ✅ Stored energy (Wh)
- ✅ Estimated State of Charge (%)
- ✅ Remaining runtime
- 📊 **Refresh rate:** 5 Hz
- 🎯 **VSS signal:** `Vehicle.Powertrain.TractionBattery.StateOfCharge.Current`, `Vehicle.Powertrain.TractionBattery.CurrentVoltage`, `Vehicle.Powertrain.TractionBattery.CurrentCurrent`, `Vehicle.Powertrain.TractionBattery.CurrentPower`


---

### 8️⃣ **SRF08** - Ultrasonic Sensor
**Communication:** I2C (external - connected to STM32)
**Location:** Mounted on the FRONT of the vehicle (frontal detection)
**Range:** 3 cm to 6 meters

**Data sent via CAN → Kuksa:**
- ✅ Distance to frontal obstacle (cm)
- ✅ Echo time (µs)
- ✅ Proximity alert (danger zone < 30cm)
- ✅ Multiple echo detection (up to 17 echoes)
- ✅ Light intensity reading (integrated ALS sensor)
- 📊 **Update rate:** 10-20 Hz (adjustable by range)
- 🎯 **VSS Signal:** `Vehicle.ADAS.ObstacleDetection.DistanceToObject`, `Vehicle.ADAS.ObstacleDetection.FrontDistance`

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

### 9️⃣ **Hall Effect Speed ​​Sensor**
**Communication:** I2C (external - connected) (to STM32)

**Function:** Wheel RPM and Speed ​​Measurement

**Data sent via CAN → Kuksa:**
- ✅ Wheel RPM
- ✅ Linear vehicle speed (km/h)
- ✅ Distance traveled (odometer)
- ✅ Movement/stop detection
- 📊 **Update rate:** 20 Hz
- 🎯 **VSS signal:** `Vehicle.Speed`, `Vehicle


---

## 🎯 Mapeamento VSS (Vehicle Signal Specification)

### Sinais Principais Publicados no Kuksa.val

```yaml
Vehicle:
  Speed: [Hall Sensor]
  TraveledDistance: [Hall Sensor]
  
  CurrentLocation:
    Latitude: [GPS - futuro]
    Longitude: [GPS - futuro]
    Altitude: [LPS22HH]
    Heading: [IIS2MDC + IMU fusion]
  
  Chassis:
    Accelerometer:
      X, Y, Z: [ISM330DHCX]
    Gyroscope:
      X, Y, Z: [ISM330DHCX]
    Magnetometer:
      X, Y, Z: [IIS2MDC]
    WheelSpeed: [Hall Sensor]
  
  Exterior:
    Temperature: [HTS221]
    Humidity: [HTS221]
    AirPressure: [LPS22HH]
    LightIntensity: [VEML6030]
  
  Powertrain:
    TractionBattery:
      StateOfCharge:
        Current: [INA226]
      CurrentVoltage: [INA226]
      CurrentCurrent: [INA226]
      CurrentPower: [INA226]
      RemainingTime: [INA226 - calculado]
  
  ADAS:
    ObstacleDetection:
      DistanceToObject: [SRF08 - frontal]
      FrontDistance: [SRF08]
      FrontZone: [SRF08 proximity alert]
      TopZone: [VL53L5CX 8x8 matrix - ceiling]
    LaneDepartureDetection:
      Status: [Camera Module 3]
      IsWarning: [Camera + AI]
    ObjectDetection:
      Objects[]: [Camera Module 3 + Hailo-8]
```
