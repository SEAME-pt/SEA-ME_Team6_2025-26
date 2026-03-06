/**
 * @file adasprovider.cpp
 * @brief Implementation of ADASProvider class
 */

#include "providers/adasprovider.hpp"

ADASProvider::ADASProvider(QObject *parent)
    : BaseProvider(parent),
      _frontDistanceValue(0.0),
      _frontDistanceStr("0")
{
    qDebug() << "[ADASProvider] Initialized";
}

QString ADASProvider::frontDistance() const
{
    QMutexLocker locker(&_mutex);
    return _frontDistanceStr;
}

void ADASProvider::updateFrontDistance(double frontDistance)
{
    QMutexLocker locker(&_mutex);

    if (qFuzzyCompare(frontDistance, _frontDistanceValue))
        return;

    _frontDistanceValue = frontDistance;
    _frontDistanceStr = QString::number(qRound(_frontDistanceValue));

    locker.unlock();
    emit frontDistanceChanged();
}