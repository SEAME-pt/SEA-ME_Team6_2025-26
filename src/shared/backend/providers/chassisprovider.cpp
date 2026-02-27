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

void ChassisProvider::updateSteeringWheelAngle(double steeringWheelAngle)
{
    QMutexLocker locker(&_mutex);

    if (qFuzzyCompare(steeringWheelAngle, _steeringWheelAngleValue))
        return;

    _steeringWheelAngleValue = steeringWheelAngle;
    _steeringWheelAngleStr = QString::number(qRound(_steeringWheelAngleValue));

    updateBlinkersInternal();

    locker.unlock();
    emit steeringWheelAngleChanged();
}

void ChassisProvider::updateBlinkersInternal()
{
    if (_steeringWheelAngleValue < 0)
    {

        if (!_isBlinkerLeftActive)
        {
            _isBlinkerLeftActive = true;
            _isBlinkerRightActive = false;
        }
    }
    else if (_steeringWheelAngleValue > 0)
    {

        if (!_isBlinkerRightActive)
        {
            _isBlinkerRightActive = true;
            _isBlinkerLeftActive = false;
        }
    }
    else
    {

        if (_isBlinkerLeftActive || _isBlinkerRightActive)
        {
            _isBlinkerLeftActive = false;
            _isBlinkerRightActive = false;
        }
    }
    emit blinkerLeftChanged();
    emit blinkerRightChanged();
}