#pragma once

#include <vector>
#include <string>

namespace sig {

// All VSS paths used by the display / reader / Qt
inline const std::vector<std::string>& all_paths()
{
    static const std::vector<std::string> paths = {

        // --- Top-level common signals ---
        "Vehicle.Speed",
        "Vehicle.AverageSpeed",
        "Vehicle.TraveledDistance",
        "Vehicle.TraveledDistanceSinceStart",
        "Vehicle.IsMoving",

        // --- VehicleIdentification ---
        "Vehicle.VehicleIdentification.Brand",
        "Vehicle.VehicleIdentification.Model",
        "Vehicle.VehicleIdentification.Year",

        // --- VersionVSS ---
        "Vehicle.VersionVSS.Major",
        "Vehicle.VersionVSS.Minor",
        "Vehicle.VersionVSS.Patch",
        "Vehicle.VersionVSS.Label",

        // --- Acceleration (IMU) ---
        "Vehicle.Acceleration.Longitudinal",
        "Vehicle.Acceleration.Lateral",
        "Vehicle.Acceleration.Vertical",

        // --- AngularVelocity (Gyro) ---
        "Vehicle.AngularVelocity.Roll",
        "Vehicle.AngularVelocity.Pitch",
        "Vehicle.AngularVelocity.Yaw",

        // --- CurrentLocation ---
        "Vehicle.CurrentLocation.Heading",

        // --- Exterior ---
        "Vehicle.Exterior.AirTemperature",
        "Vehicle.Exterior.Humidity",
        "Vehicle.Exterior.LightIntensity",
        "Vehicle.Exterior.AtmosphericPressure",

        // --- Chassis ---
        "Vehicle.Chassis.SteeringWheel.Angle",
        "Vehicle.Chassis.Accelerator.PedalPosition",
        "Vehicle.Chassis.Brake.PedalPosition",

        // --- ADAS ---
        "Vehicle.ADAS.ObstacleDetection.IsEnabled",
        "Vehicle.ADAS.ObstacleDetection.Front.IsWarning",
        "Vehicle.ADAS.ObstacleDetection.Front.Distance",
        "Vehicle.ADAS.ActiveAutonomyLevel",

        // --- Powertrain ---
        "Vehicle.Powertrain.Type",

        // ElectricMotor
        "Vehicle.Powertrain.ElectricMotor.IsRunning",
        "Vehicle.Powertrain.ElectricMotor.Speed",
        "Vehicle.Powertrain.ElectricMotor.Power",
        "Vehicle.Powertrain.ElectricMotor.Temperature",

        // Transmission
        "Vehicle.Powertrain.Transmission.CurrentGear",
        "Vehicle.Powertrain.Transmission.DriveType",

        // TractionBattery (attributes)
        "Vehicle.Powertrain.TractionBattery.Id",
        "Vehicle.Powertrain.TractionBattery.NominalVoltage",
        "Vehicle.Powertrain.TractionBattery.MaxVoltage",
        "Vehicle.Powertrain.TractionBattery.GrossCapacity",

        // TractionBattery (sensors)
        "Vehicle.Powertrain.TractionBattery.CurrentVoltage",
        "Vehicle.Powertrain.TractionBattery.CurrentCurrent",
        "Vehicle.Powertrain.TractionBattery.CurrentPower",

        // TractionBattery SOC
        "Vehicle.Powertrain.TractionBattery.StateOfCharge.Current",
        "Vehicle.Powertrain.TractionBattery.StateOfCharge.Displayed",

        // TractionBattery alarms + range
        "Vehicle.Powertrain.TractionBattery.IsLevelLow",
        "Vehicle.Powertrain.TractionBattery.IsCritical",
        "Vehicle.Powertrain.TractionBattery.Range",

        // --- LowVoltageBattery rails ---
        "Vehicle.LowVoltageBattery.Rail5V.CurrentVoltage",
        "Vehicle.LowVoltageBattery.Rail5V.CurrentCurrent",
        "Vehicle.LowVoltageBattery.Rail3V3.CurrentVoltage",
        "Vehicle.LowVoltageBattery.Rail3V3.CurrentCurrent",

        // --- LowVoltageSystemState ---
        "Vehicle.LowVoltageSystemState",

        // --- ECU.ApplicationDomain (RPi5) ---
        "Vehicle.ECU.ApplicationDomain.IsConnected",
        "Vehicle.ECU.ApplicationDomain.Heartbeat",
        "Vehicle.ECU.ApplicationDomain.Temperature",
        "Vehicle.ECU.ApplicationDomain.CPULoad",
        "Vehicle.ECU.ApplicationDomain.MemoryUsage",

        // --- ECU.SafetyCritical (STM32) ---
        "Vehicle.ECU.SafetyCritical.IsConnected",
        "Vehicle.ECU.SafetyCritical.Heartbeat",
        "Vehicle.ECU.SafetyCritical.Temperature",
        "Vehicle.ECU.SafetyCritical.ErrorCode",

        // --- ECU.Display (RPi4 cluster) ---
        "Vehicle.ECU.Display.IsConnected",
        "Vehicle.ECU.Display.Heartbeat",

        // --- AIAccelerator (Hailo-8) ---
        "Vehicle.AIAccelerator.IsConnected",
        "Vehicle.AIAccelerator.IsEnabled",
        "Vehicle.AIAccelerator.IsInferenceActive",
        "Vehicle.AIAccelerator.Temperature",
        "Vehicle.AIAccelerator.Power",
        "Vehicle.AIAccelerator.InferenceRate",
        "Vehicle.AIAccelerator.CurrentModel",
        "Vehicle.AIAccelerator.Utilization",

        // --- Relay ---
        "Vehicle.Relay.Main.IsOn",
        "Vehicle.Relay.Motor.IsOn",
        "Vehicle.Relay.Display.IsOn",
        "Vehicle.Relay.AIAccelerator.IsOn",

        // --- Communication.CAN ---
        "Vehicle.Communication.CAN.IsConnected",
        "Vehicle.Communication.CAN.ErrorCount",
        "Vehicle.Communication.CAN.BusLoad",
        "Vehicle.Communication.CAN.MessageRate",

        // --- Diagnostics ---
        "Vehicle.Diagnostics.DTCCount",
        "Vehicle.Diagnostics.DTCList"
    };

    return paths;
}

} // namespace sig