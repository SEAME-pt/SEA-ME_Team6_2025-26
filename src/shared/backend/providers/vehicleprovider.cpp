/**
 * @file vehicleprovider.cpp
 * @brief Implementation of VehicleProvider class
 */

#include "providers/vehicleprovider.hpp"

VehicleProvider::VehicleProvider(QObject *parent)
    : BaseProvider(parent),
      _speedValue(0.0),
      _speedStr("0")
{
    qDebug() << "[VehicleProvider] Initialized";
}

QString VehicleProvider::speed() const
{
    QMutexLocker locker(&_mutex);
    return _speedStr;
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

    _speedValue = speed;
    _speedStr = QString::number(qRound(_speedValue));

    locker.unlock();
    emit speedChanged();
}