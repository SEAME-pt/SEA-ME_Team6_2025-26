/**
 * @file signalrouter.cpp
 * @brief Implementation of SignalRouter class
 */

#include "signalrouter.hpp"
#include "providers/powertrainprovider.hpp"
#include "providers/vehicleprovider.hpp"
#include "providers/exteriorprovider.hpp"
#include "providers/adasprovider.hpp"
#include "providers/currentlocationprovider.hpp"
#include "providers/chassisprovider.hpp"

SignalRouter::SignalRouter(QObject *parent)
    : QObject(parent),
      _powertrainProvider(nullptr),
      _vehicleProvider(nullptr),
      _exteriorProvider(nullptr),
      _adasProvider(nullptr),
      _currentLocationProvider(nullptr),
      _chassisProvider(nullptr)
{
    qDebug() << "[SignalRouter] Created";
}

void SignalRouter::registerPowertrainProvider(PowertrainProvider *provider)
{
    _powertrainProvider = provider;
    qDebug() << "[SignalRouter] PowertrainProvider registered";
}

void SignalRouter::registerVehicleProvider(VehicleProvider *provider)
{
    _vehicleProvider = provider;
    qDebug() << "[SignalRouter] VehicleProvider registered";
}

void SignalRouter::registerExteriorProvider(ExteriorProvider *provider)
{
    _exteriorProvider = provider;
    qDebug() << "[SignalRouter] ExteriorProvider registered";
}

void SignalRouter::registerADASProvider(ADASProvider *provider)
{
    _adasProvider = provider;
    qDebug() << "[SignalRouter] ADASProvider registered";
}

void SignalRouter::registerCurrentLocationProvider(CurrentLocationProvider *provider)
{
    _currentLocationProvider = provider;
    qDebug() << "[SignalRouter] CurrentLocationProvider registered";
}

void SignalRouter::registerChassisProvider(ChassisProvider *provider)
{
    _chassisProvider = provider;
    qDebug() << "[SignalRouter] ChassisProvider registered";
}

void SignalRouter::routeSignal(const QString &path, const QVariant &value)
{
    if (!value.isValid()) {
        qWarning() << "[SignalRouter] Received invalid value for path:" << path;
        return;
    }

    // Route based on signal path prefix
    if (path.startsWith("Vehicle.Powertrain"))
        routePowertrainSignal(path, value);
    else if (path.startsWith("Vehicle.Exterior"))
        routeExteriorSignal(path, value);
    else if (path.startsWith("Vehicle.ADAS"))
        routeADASSignal(path, value);
    else if (path.startsWith("Vehicle.CurrentLocation"))
        routeCurrentLocationSignal(path, value);
    else if (path.startsWith("Vehicle.Chassis"))
         routeChassisSignal(path, value);
    else if (path.startsWith("Vehicle")){
        routeVehicleSignal(path, value);
    }
    else {
        qWarning() << "[SignalRouter] Unhandled signal path:" << path;
        emit routingError(QString("Unhandled signal: %1").arg(path));
    }
}

void SignalRouter::routePowertrainSignal(const QString &path, const QVariant &value)
{
    if (!isProviderRegistered(_powertrainProvider, "PowertrainProvider"))
        return;

    if (path == "Vehicle.Powertrain.TractionBattery.CurrentVoltage") {
        _powertrainProvider->updateBatteryVoltage(value.toDouble());
    }
    else if (path == "Vehicle.Powertrain.TractionBattery.IsLevelLow") {
        _powertrainProvider->updateBatteryLowStatus(value.toBool());
    }
    else if (path == "Vehicle.Powertrain.TractionBattery.IsCritical") {
        _powertrainProvider->updateBatteryCriticalStatus(value.toBool());
    }
    else if (path == "Vehicle.Powertrain.ElectricMotor.Speed") {
        _powertrainProvider->updateMotorSpeed(value.toDouble());
    }
    else {
        qDebug() << "[SignalRouter] Unknown powertrain signal:" << path;
    }
}

void SignalRouter::routeExteriorSignal(const QString &path, const QVariant &value)
{
    if (!isProviderRegistered(_exteriorProvider, "ExteriorProvider"))
        return;

    if (path == "Vehicle.Exterior.AirTemperature") {
        _exteriorProvider->updateAirTemperature(value.toDouble());
    }
    else {
        qDebug() << "[SignalRouter] Unknown top-level signal:" << path;
    }
}


void SignalRouter::routeVehicleSignal(const QString &path, const QVariant &value)
{
    if (!isProviderRegistered(_vehicleProvider, "VehicleProvider"))
        return;

    if (path == "Vehicle.Speed") {
        _vehicleProvider->updateSpeed(value.toDouble());
    }
    else {
        qDebug() << "[SignalRouter] Unknown top-level signal:" << path;
    }
}

void SignalRouter::routeADASSignal(const QString &path, const QVariant &value)
{
    if (!isProviderRegistered(_adasProvider, "ADASProvider"))
        return;

    if (path == "Vehicle.ADAS.ObstacleDetection.Front.Distance") {
        _adasProvider->updateFrontDistance(value.toDouble());
    }
    else {
        qDebug() << "[SignalRouter] Unknown top-level signal:" << path;
    }
}

void SignalRouter::routeCurrentLocationSignal(const QString &path, const QVariant &value)
{
    if (!isProviderRegistered(_currentLocationProvider, "CurrentLocationProvider"))
        return;

    if (path == "Vehicle.CurrentLocation.Heading") {
        _currentLocationProvider->updateHeading(value.toDouble());
    }
    else {
        qDebug() << "[SignalRouter] Unknown top-level signal:" << path;
    }
}

void SignalRouter::routeChassisSignal(const QString &path, const QVariant &value)
{
    if (!isProviderRegistered(_chassisProvider, "ChassisProvider"))
        return;

    if (path == "Vehicle.Chassis.SteeringWheel.Angle") {
        _chassisProvider->updateSteeringWheelAngle(value.toDouble());
    }
    else {
        qDebug() << "[SignalRouter] Unknown chassis signal:" << path;
    }
}


template<typename T>
bool SignalRouter::isProviderRegistered(T* provider, const QString &providerName)
{
    if (!provider) {
        qWarning() << "[SignalRouter]" << providerName << "not registered - signal dropped";
        emit routingError(QString("%1 not registered").arg(providerName));
        return false;
    }
    return true;
}