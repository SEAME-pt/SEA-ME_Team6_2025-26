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
      _speedLimitValue(0.0),
      _speedLimitStr("0"),
      _trafficLightValue("none"),
      _streetSignalsValue({}),
      _extrasValue({}),
      _LKAValue(true),
      _CCValue(true),
      _AEBValue(true),
      _TSRValue(true)
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
    return _laneStatusValue;
}

QList<double> ADASProvider::lateralDeviationHistory() const
{
    QMutexLocker locker(&_mutex);
    return _lateralDeviationHistory;
}

QString ADASProvider::speedLimit() const
{
    QMutexLocker locker(&_mutex);
    return _speedLimitStr;
}

QString ADASProvider::trafficLight() const
{
    QMutexLocker locker(&_mutex);
    return _trafficLightValue;
}

QStringList ADASProvider::streetSignals() const
{
    QMutexLocker locker(&_mutex);
    return _streetSignalsValue;
}

QStringList ADASProvider::extras() const
{
    QMutexLocker locker(&_mutex);
    return _extrasValue;
}

bool ADASProvider::LKAStatus() const
{
    QMutexLocker locker(&_mutex);
    return _LKAValue;
}

bool ADASProvider::CCStatus() const
{
    QMutexLocker locker(&_mutex);
    return _CCValue;
}

bool ADASProvider::AEBStatus() const
{
    QMutexLocker locker(&_mutex);
    return _AEBValue;
}

bool ADASProvider::TSRStatus() const
{
    QMutexLocker locker(&_mutex);
    return _TSRValue;
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

void ADASProvider::updateSpeedLimit(double speedLimit)
{
    QMutexLocker locker(&_mutex);

    if (qFuzzyCompare(speedLimit, _speedLimitValue))
        return;

    _speedLimitValue = speedLimit;
    _speedLimitStr = QString::number(_speedLimitValue);

    locker.unlock();
    emit speedLimitChanged();
}

void ADASProvider::updateTrafficLight(QString trafficLight)
{
    QMutexLocker locker(&_mutex);

    if (trafficLight == _trafficLightValue)
        return;

    _trafficLightValue = trafficLight;

    locker.unlock();
    emit trafficLightChanged();
}

void ADASProvider::updateLaneStatus(QString laneStatus)
{
    QMutexLocker locker(&_mutex);

    if (laneStatus == _laneStatusValue)
        return;

    _laneStatusValue = laneStatus;

    locker.unlock();
    emit laneStatusChanged();
}

void ADASProvider::updateStreetSignals(const QStringList &streetSignals)
{
    QMutexLocker locker(&_mutex);

    if (streetSignals == _streetSignalsValue)
        return;

    _streetSignalsValue = streetSignals;

    locker.unlock();
    emit streetSignalsChanged();
}

void ADASProvider::updateExtras(const QStringList &extras)
{
    QMutexLocker locker(&_mutex);

    if (extras == _extrasValue)
        return;

    _extrasValue = extras;

    locker.unlock();
    emit extrasChanged();
}

void ADASProvider::updateLKAStatus(bool LKAStatus)
{
    QMutexLocker locker(&_mutex);

    if (LKAStatus == _LKAValue)
        return;

    _LKAValue = LKAStatus;

    locker.unlock();
    emit LKAStatusChanged();
}

void ADASProvider::updateCCStatus(bool CCStatus)
{
    QMutexLocker locker(&_mutex);

    if (CCStatus == _CCValue)
        return;

    _CCValue = CCStatus;

    locker.unlock();
    emit CCStatusChanged();
}

void ADASProvider::updateAEBStatus(bool AEBStatus)
{
    QMutexLocker locker(&_mutex);

    if (AEBStatus == _AEBValue)
        return;

    _AEBValue = AEBStatus;

    locker.unlock();
    emit AEBStatusChanged();
}

void ADASProvider::updateTSRStatus(bool TSRStatus)
{
    QMutexLocker locker(&_mutex);

    if (TSRStatus == _TSRValue)
        return;

    _TSRValue = TSRStatus;

    locker.unlock();
    emit TSRStatusChanged();
}

void ADASProvider::triggerUpdateADASFeature(QString VSSPath, bool value)
{
    qDebug() << "[ADASProvider] Triggering update on " << VSSPath;
    emit requestWrite(VSSPath, QVariant(value));
}