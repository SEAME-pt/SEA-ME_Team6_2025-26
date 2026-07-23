/**
 * @file vehicleprovider.cpp
 * @brief Implementation of VehicleProvider class
 */

#include "providers/vehicleprovider.hpp"

VehicleProvider::VehicleProvider(QObject *parent)
    : BaseProvider(parent),
      _speedValue(0.0),
      _speedStr("0"),
      _maxSpeedTripValue(0.0),
      _maxSpeedTripStr("0"),
      _totalEnergyTripValue(0.0),
      _totalEnergyTripStr("0"),
      _traveledDistanceTripValue(0.0),
      _traveledDistanceTripStr("0"),
       _durationTripValue(0.0),
      _durationTripStr("0"),
      _energyPerKmTripValue(0.0),
      _energyPerKmTripStr("0"),
      _traveledDistanceValue(0.0),
      _traveledDistanceStr("0"),
      _averageSpeedValue(0.0),
      _averageSpeedStr("0")
{
    qDebug() << "[VehicleProvider] Initialized";
}

QString VehicleProvider::speed() const
{
    QMutexLocker locker(&_mutex);
    return _speedStr;
}

QString VehicleProvider::maxSpeedTrip() const
{
    QMutexLocker locker(&_mutex);
    return _maxSpeedTripStr;
}

QString VehicleProvider::totalEnergyTrip() const
{
    QMutexLocker locker(&_mutex);
    return _totalEnergyTripStr;
}

QString VehicleProvider::traveledDistanceTrip() const
{
    QMutexLocker locker(&_mutex);
    return _traveledDistanceTripStr;
}

QString VehicleProvider::durationTrip() const
{
    QMutexLocker locker(&_mutex);
    return _durationTripStr;
}

QString VehicleProvider::energyPerKmTrip() const
{
    QMutexLocker locker(&_mutex);
    return _energyPerKmTripStr;
}

QString VehicleProvider::traveledDistance() const
{
    QMutexLocker locker(&_mutex);
    return _traveledDistanceStr;
}

QString VehicleProvider::averageSpeed() const
{
    QMutexLocker locker(&_mutex);
    return _averageSpeedStr;
}

void VehicleProvider::updateSpeed(double speed)
{
    QMutexLocker locker(&_mutex);
    double percentageChange = 0.0;

    if (_speedValue > 0.0)
        percentageChange = std::abs(speed - _speedValue) / _speedValue;
    else if (speed > 0.0)
        percentageChange = SPEED_CHANGE_THRESHOLD + 1.0;

    if (percentageChange < SPEED_CHANGE_THRESHOLD && !qFuzzyIsNull(_speedValue))
        return;

    if (speed >= 6000)
        return;

    _speedValue = speed;
    _speedStr = QString::number(qRound(_speedValue));

    locker.unlock();
    emit speedChanged();
}

void VehicleProvider::updateMaxSpeedTrip(double maxSpeedTrip)
{
    QMutexLocker locker(&_mutex);

    const double maxSpeedTripKmh = maxSpeedTrip / 1000.0; // m/h -> km/h

    if (qFuzzyCompare(maxSpeedTripKmh, _maxSpeedTripValue))
        return;

    _maxSpeedTripValue = maxSpeedTripKmh;
    _maxSpeedTripStr = QString::number(_maxSpeedTripValue, 'f', 2);

    locker.unlock();
    emit maxSpeedTripChanged();
}

void VehicleProvider::updateTotalEnergyTrip(double totalEnergyTrip)
{
    QMutexLocker locker(&_mutex);

    if (qFuzzyCompare(totalEnergyTrip, _totalEnergyTripValue))
        return;

    _totalEnergyTripValue = totalEnergyTrip;
    _totalEnergyTripStr = QString::number(_totalEnergyTripValue, 'f', 2);

    locker.unlock();
    emit totalEnergyTripChanged();
}

void VehicleProvider::updateTraveledDistanceTrip(double traveledDistanceTrip)
{
    QMutexLocker locker(&_mutex);

    if (qFuzzyCompare(traveledDistanceTrip, _traveledDistanceTripValue))
        return;

    _traveledDistanceTripValue = traveledDistanceTrip;
    _traveledDistanceTripStr = QString::number(_traveledDistanceTripValue, 'f', 2);

    locker.unlock();
    emit traveledDistanceTripChanged();
}

void VehicleProvider::updateDurationTrip(double durationTrip)
{
    QMutexLocker locker(&_mutex);

    if (qFuzzyCompare(durationTrip, _durationTripValue))
        return;

    _durationTripValue = durationTrip;
    _durationTripStr = QString::number(_durationTripValue);

    locker.unlock();
    emit durationTripChanged();
}

void VehicleProvider::updateEnergyPerKmTrip(double energyPerKmTrip)
{
    QMutexLocker locker(&_mutex);

    if (qFuzzyCompare(energyPerKmTrip, _energyPerKmTripValue))
        return;

    _energyPerKmTripValue = energyPerKmTrip;
    _energyPerKmTripStr = QString::number(_energyPerKmTripValue, 'f', 2);

    locker.unlock();
    emit energyPerKmTripChanged();
}

void VehicleProvider::updateTraveledDistance(double traveledDistance)
{
    QMutexLocker locker(&_mutex);

    if (qFuzzyCompare(traveledDistance, _traveledDistanceValue))
        return;

    _traveledDistanceValue = traveledDistance;
    _traveledDistanceStr = QString::number(_traveledDistanceValue, 'f', 2);

    locker.unlock();
    emit traveledDistanceChanged();
}


void VehicleProvider::updateAverageSpeed(double averageSpeed)
{
    QMutexLocker locker(&_mutex);

    const double averageSpeedKmh = averageSpeed / 1000.0; // m/h -> km/h

    if (qFuzzyCompare(averageSpeedKmh, _averageSpeedValue))
        return;

    _averageSpeedValue = averageSpeedKmh;
    _averageSpeedStr = QString::number(_averageSpeedValue, 'f', 2);

    locker.unlock();
    emit averageSpeedChanged();
}
