/**
 * @file exteriorprovider.cpp
 * @brief Implementation of ExteriorProvider class
 */

#include "providers/exteriorprovider.hpp"

ExteriorProvider::ExteriorProvider(QObject *parent)
    : BaseProvider(parent),
      _airTemperatureValue(0.0),
      _airTemperatureStr("0")
{
    qDebug() << "[ExteriorProvider] Initialized";
}

QString ExteriorProvider::airTemperature() const
{
    QMutexLocker locker(&_mutex);
    return _airTemperatureStr;
}

void ExteriorProvider::updateAirTemperature(double temperature)
{
    QMutexLocker locker(&_mutex);

    if (qFuzzyCompare(temperature, _airTemperatureValue))
        return;

    _airTemperatureValue = temperature;
    _airTemperatureStr = QString::number(qRound(_airTemperatureValue));

    locker.unlock();
    emit airTemperatureChanged();
}