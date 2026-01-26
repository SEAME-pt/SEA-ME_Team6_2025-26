// inc/signals.hpp

/* --- How to use ----
    Include this header where you need to refer to KUKSA signal paths,
    e.g. in handlers or publishers.
    
    Example:
      #include "signals.hpp"
      ...
      kuksa.publishDouble(sig::VEHICLE_SPEED, speed_value);
------------------- */

#pragma once

namespace sig {

// --- Top-level common signals ---
inline constexpr const char* VEHICLE_SPEED = "Vehicle.Speed";
inline constexpr const char* VEHICLE_AVG_SPEED = "Vehicle.AverageSpeed";
inline constexpr const char* VEHICLE_TRAVELED_DISTANCE = "Vehicle.TraveledDistance";
inline constexpr const char* VEHICLE_TRAVELED_DISTANCE_SINCE_START = "Vehicle.TraveledDistanceSinceStart";
inline constexpr const char* VEHICLE_IS_MOVING = "Vehicle.IsMoving";

// --- VehicleIdentification ---
inline constexpr const char* VEHICLE_ID_BRAND = "Vehicle.VehicleIdentification.Brand";
inline constexpr const char* VEHICLE_ID_MODEL = "Vehicle.VehicleIdentification.Model";
inline constexpr const char* VEHICLE_ID_YEAR  = "Vehicle.VehicleIdentification.Year";

// --- VersionVSS ---
inline constexpr const char* VSS_VERSION_MAJOR = "Vehicle.VersionVSS.Major";
inline constexpr const char* VSS_VERSION_MINOR = "Vehicle.VersionVSS.Minor";
inline constexpr const char* VSS_VERSION_PATCH = "Vehicle.VersionVSS.Patch";
inline constexpr const char* VSS_VERSION_LABEL = "Vehicle.VersionVSS.Label";

// --- Acceleration (IMU) ---
inline constexpr const char* ACC_LONGITUDINAL = "Vehicle.Acceleration.Longitudinal";
inline constexpr const char* ACC_LATERAL      = "Vehicle.Acceleration.Lateral";
inline constexpr const char* ACC_VERTICAL     = "Vehicle.Acceleration.Vertical";

// --- AngularVelocity (Gyro) ---
inline constexpr const char* ANG_VEL_ROLL  = "Vehicle.AngularVelocity.Roll";
inline constexpr const char* ANG_VEL_PITCH = "Vehicle.AngularVelocity.Pitch";
inline constexpr const char* ANG_VEL_YAW   = "Vehicle.AngularVelocity.Yaw";

// --- CurrentLocation ---
inline constexpr const char* LOCATION_HEADING = "Vehicle.CurrentLocation.Heading";

// --- Exterior ---
inline constexpr const char* EXT_AIR_TEMPERATURE     = "Vehicle.Exterior.AirTemperature";
inline constexpr const char* EXT_HUMIDITY            = "Vehicle.Exterior.Humidity";
inline constexpr const char* EXT_LIGHT_INTENSITY     = "Vehicle.Exterior.LightIntensity";
inline constexpr const char* EXT_ATMOS_PRESSURE_KPA  = "Vehicle.Exterior.AtmosphericPressure";

// --- Chassis ---
inline constexpr const char* CHASSIS_STEER_ANGLE = "Vehicle.Chassis.SteeringWheel.Angle";
inline constexpr const char* CHASSIS_ACCEL_PEDAL = "Vehicle.Chassis.Accelerator.PedalPosition";
inline constexpr const char* CHASSIS_BRAKE_PEDAL = "Vehicle.Chassis.Brake.PedalPosition";

// --- ADAS ---
inline constexpr const char* ADAS_OBS_IS_ENABLED      = "Vehicle.ADAS.ObstacleDetection.IsEnabled";
inline constexpr const char* ADAS_FRONT_IS_WARNING    = "Vehicle.ADAS.ObstacleDetection.Front.IsWarning";
inline constexpr const char* ADAS_FRONT_DISTANCE_MM   = "Vehicle.ADAS.ObstacleDetection.Front.Distance";
inline constexpr const char* ADAS_ACTIVE_AUTONOMY_LVL = "Vehicle.ADAS.ActiveAutonomyLevel";

// --- Powertrain ---
inline constexpr const char* POWERTRAIN_TYPE = "Vehicle.Powertrain.Type";

// ElectricMotor
inline constexpr const char* EMOTOR_IS_RUNNING  = "Vehicle.Powertrain.ElectricMotor.IsRunning";
inline constexpr const char* EMOTOR_SPEED_RPM   = "Vehicle.Powertrain.ElectricMotor.Speed";
inline constexpr const char* EMOTOR_POWER_W     = "Vehicle.Powertrain.ElectricMotor.Power";
inline constexpr const char* EMOTOR_TEMP_C      = "Vehicle.Powertrain.ElectricMotor.Temperature";

// Transmission
inline constexpr const char* TRANS_CURRENT_GEAR = "Vehicle.Powertrain.Transmission.CurrentGear";
inline constexpr const char* TRANS_DRIVE_TYPE   = "Vehicle.Powertrain.Transmission.DriveType";

// TractionBattery (attributes)
inline constexpr const char* TBATT_ID            = "Vehicle.Powertrain.TractionBattery.Id";
inline constexpr const char* TBATT_NOMINAL_V     = "Vehicle.Powertrain.TractionBattery.NominalVoltage";
inline constexpr const char* TBATT_MAX_V         = "Vehicle.Powertrain.TractionBattery.MaxVoltage";
inline constexpr const char* TBATT_GROSS_CAP_MAH = "Vehicle.Powertrain.TractionBattery.GrossCapacity";

// TractionBattery (sensors)
inline constexpr const char* TBATT_VOLT_V   = "Vehicle.Powertrain.TractionBattery.CurrentVoltage";
inline constexpr const char* TBATT_CURR_A   = "Vehicle.Powertrain.TractionBattery.CurrentCurrent";
inline constexpr const char* TBATT_POWER_W  = "Vehicle.Powertrain.TractionBattery.CurrentPower";

// TractionBattery SOC
inline constexpr const char* TBATT_SOC_CURRENT   = "Vehicle.Powertrain.TractionBattery.StateOfCharge.Current";
inline constexpr const char* TBATT_SOC_DISPLAYED = "Vehicle.Powertrain.TractionBattery.StateOfCharge.Displayed";

// TractionBattery alarms + range
inline constexpr const char* TBATT_IS_LEVEL_LOW = "Vehicle.Powertrain.TractionBattery.IsLevelLow";
inline constexpr const char* TBATT_IS_CRITICAL  = "Vehicle.Powertrain.TractionBattery.IsCritical";
inline constexpr const char* TBATT_RANGE_M      = "Vehicle.Powertrain.TractionBattery.Range";

// --- LowVoltageBattery rails ---
inline constexpr const char* LV_5V_VOLT_V = "Vehicle.LowVoltageBattery.Rail5V.CurrentVoltage";
inline constexpr const char* LV_5V_CURR_A = "Vehicle.LowVoltageBattery.Rail5V.CurrentCurrent";

inline constexpr const char* LV_3V3_VOLT_V = "Vehicle.LowVoltageBattery.Rail3V3.CurrentVoltage";
inline constexpr const char* LV_3V3_CURR_A = "Vehicle.LowVoltageBattery.Rail3V3.CurrentCurrent";

// --- LowVoltageSystemState ---
inline constexpr const char* LV_SYSTEM_STATE = "Vehicle.LowVoltageSystemState";

// --- ECU.ApplicationDomain (RPi5) ---
inline constexpr const char* ECU_APP_IS_CONNECTED = "Vehicle.ECU.ApplicationDomain.IsConnected";
inline constexpr const char* ECU_APP_HEARTBEAT    = "Vehicle.ECU.ApplicationDomain.Heartbeat";
inline constexpr const char* ECU_APP_TEMPERATURE  = "Vehicle.ECU.ApplicationDomain.Temperature";
inline constexpr const char* ECU_APP_CPU_LOAD     = "Vehicle.ECU.ApplicationDomain.CPULoad";
inline constexpr const char* ECU_APP_MEM_USAGE    = "Vehicle.ECU.ApplicationDomain.MemoryUsage";

// --- ECU.SafetyCritical (STM32) ---
inline constexpr const char* ECU_SC_IS_CONNECTED = "Vehicle.ECU.SafetyCritical.IsConnected";
inline constexpr const char* ECU_SC_HEARTBEAT    = "Vehicle.ECU.SafetyCritical.Heartbeat";
inline constexpr const char* ECU_SC_TEMPERATURE  = "Vehicle.ECU.SafetyCritical.Temperature";
inline constexpr const char* ECU_SC_ERROR_CODE   = "Vehicle.ECU.SafetyCritical.ErrorCode";

// --- ECU.Display (RPi4 cluster) ---
inline constexpr const char* ECU_DISP_IS_CONNECTED = "Vehicle.ECU.Display.IsConnected";
inline constexpr const char* ECU_DISP_HEARTBEAT    = "Vehicle.ECU.Display.Heartbeat";

// --- AIAccelerator (Hailo-8) ---
inline constexpr const char* AI_IS_CONNECTED      = "Vehicle.AIAccelerator.IsConnected";
inline constexpr const char* AI_IS_ENABLED        = "Vehicle.AIAccelerator.IsEnabled";
inline constexpr const char* AI_IS_INFER_ACTIVE   = "Vehicle.AIAccelerator.IsInferenceActive";
inline constexpr const char* AI_TEMPERATURE       = "Vehicle.AIAccelerator.Temperature";
inline constexpr const char* AI_POWER_W           = "Vehicle.AIAccelerator.Power";
inline constexpr const char* AI_INFER_RATE_FPS    = "Vehicle.AIAccelerator.InferenceRate";
inline constexpr const char* AI_CURRENT_MODEL     = "Vehicle.AIAccelerator.CurrentModel";
inline constexpr const char* AI_UTILIZATION_PCT   = "Vehicle.AIAccelerator.Utilization";

// --- Relay ---
inline constexpr const char* RELAY_MAIN_IS_ON = "Vehicle.Relay.Main.IsOn";
inline constexpr const char* RELAY_MOTOR_IS_ON = "Vehicle.Relay.Motor.IsOn";
inline constexpr const char* RELAY_DISPLAY_IS_ON = "Vehicle.Relay.Display.IsOn";
inline constexpr const char* RELAY_AI_IS_ON = "Vehicle.Relay.AIAccelerator.IsOn";

// --- Communication.CAN ---
inline constexpr const char* CAN_IS_CONNECTED   = "Vehicle.Communication.CAN.IsConnected";
inline constexpr const char* CAN_ERROR_COUNT    = "Vehicle.Communication.CAN.ErrorCount";
inline constexpr const char* CAN_BUS_LOAD_PCT   = "Vehicle.Communication.CAN.BusLoad";
inline constexpr const char* CAN_MSG_RATE       = "Vehicle.Communication.CAN.MessageRate";

// --- Diagnostics ---
inline constexpr const char* DTC_COUNT = "Vehicle.Diagnostics.DTCCount";
inline constexpr const char* DTC_LIST  = "Vehicle.Diagnostics.DTCList";

} // namespace sig
