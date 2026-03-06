/**
 * @file currentlocationprovider.cpp
 * @brief Implementation of CurrentLocationProvider class
 */

#include "providers/currentlocationprovider.hpp"

CurrentLocationProvider::CurrentLocationProvider(QObject *parent)
    : BaseProvider(parent),
      _headingValue(0.0),
      _headingStr("0")
{
    qDebug() << "[CurrentLocationProvider] Initialized";
}

QString CurrentLocationProvider::heading() const
{
    QMutexLocker locker(&_mutex);
    return _headingStr;
}

void CurrentLocationProvider::updateHeading(double heading)
{
    QMutexLocker locker(&_mutex);

    if (qFuzzyCompare(heading, _headingValue))
        return;

    _headingValue = heading;
    _headingStr = QString::number(qRound(_headingValue));

    locker.unlock();
    emit headingChanged();
}