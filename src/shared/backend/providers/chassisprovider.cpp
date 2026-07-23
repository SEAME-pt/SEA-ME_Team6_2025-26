/**
 * @file chassisprovider.cpp
 * @brief Implementation of ChassisProvider class
 */

#include "providers/chassisprovider.hpp"

ChassisProvider::ChassisProvider(QObject *parent)
    : BaseProvider(parent),
      _steeringWheelAngleValue(0.0),
      _isBlinkerLeftActive(false),
      _isBlinkerRightActive(false),
      _steeringWheelAngleStr("0")
{
    qDebug() << "[ChassisProvider] Initialized";
}

QString ChassisProvider::steeringWheelAngle() const
{
    QMutexLocker locker(&_mutex);
    return _steeringWheelAngleStr;
}

bool ChassisProvider::isBlinkerLeftActive() const
{
    QMutexLocker locker(&_mutex);
    return _isBlinkerLeftActive;
}

bool ChassisProvider::isBlinkerRightActive() const
{
    QMutexLocker locker(&_mutex);
    return _isBlinkerRightActive;
}

void ChassisProvider::updateSteeringWheelAngle(double angle)
{
    bool angleChanged = false, blinkerChanged = false;
    {
        QMutexLocker locker(&_mutex);
        if (qFuzzyCompare(angle, _steeringWheelAngleValue)) return;
        _steeringWheelAngleValue = angle;
        _steeringWheelAngleStr = QString::number(qRound(angle));
        blinkerChanged = updateBlinkersInternal();
        angleChanged = true;
    }
    if (angleChanged) emit steeringWheelAngleChanged();
    if (blinkerChanged) { emit blinkerLeftChanged(); emit blinkerRightChanged(); }
}

bool ChassisProvider::updateBlinkersInternal()
{
    bool prev_left = _isBlinkerLeftActive;
    bool prev_right = _isBlinkerRightActive;

    qDebug() << "[ChassisProvider] updateBlinkersInternal";

    if (_steeringWheelAngleValue < 0) {
        _isBlinkerLeftActive = true;
        _isBlinkerRightActive = false;
    } else if (_steeringWheelAngleValue > 0) {
        _isBlinkerRightActive = true;
        _isBlinkerLeftActive = false;
    } else {
        _isBlinkerLeftActive = false;
        _isBlinkerRightActive = false;
    }
    return (_isBlinkerLeftActive != prev_left || _isBlinkerRightActive != prev_right);
}