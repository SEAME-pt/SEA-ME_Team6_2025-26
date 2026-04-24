/**
 * @file adasprovider.cpp
 * @brief Implementation of ADASProvider class
 */

#include "providers/adasprovider.hpp"

ADASProvider::ADASProvider(QObject *parent)
    : BaseProvider(parent),
      _frontDistanceValue(0.0),
      _frontDistanceStr("0"),
      _lateralDeviationValue(0.0),
      _lateralDeviationStr("0"),
      _laneStatusValue("none"),
      _laneStatusStr("none")
{
    qDebug() << "[ADASProvider] Initialized";
}

QString ADASProvider::frontDistance() const
{
    QMutexLocker locker(&_mutex);
    return _frontDistanceStr;
}

QString ADASProvider::lateralDeviation() const
{
    QMutexLocker locker(&_mutex);
    return _lateralDeviationStr;
}

QString ADASProvider::laneStatus() const
{
    QMutexLocker locker(&_mutex);
    return _laneStatusStr;
}

QList<double> ADASProvider::lateralDeviationHistory() const
{
    QMutexLocker locker(&_mutex);
    return _lateralDeviationHistory;
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

void ADASProvider::updateLateralDeviation(double lateralDeviation)
{
    QMutexLocker locker(&_mutex);

    if (qFuzzyCompare(lateralDeviation, _lateralDeviationValue))
        return;

    _lateralDeviationValue = lateralDeviation;
    _lateralDeviationStr = QString::number(_lateralDeviationValue, 'f', 2);

    _lateralDeviationHistory.append(lateralDeviation);
    if (_lateralDeviationHistory.size() > 30)
        _lateralDeviationHistory.removeFirst();

    locker.unlock();
    emit lateralDeviationChanged();
}

void ADASProvider::updateLaneStatus(QString laneStatus)
{
    QMutexLocker locker(&_mutex);

    if (laneStatus == _laneStatusValue)
        return;

    _laneStatusValue = laneStatus;
    _laneStatusStr = laneStatus;

    locker.unlock();
    emit laneStatusChanged();
}