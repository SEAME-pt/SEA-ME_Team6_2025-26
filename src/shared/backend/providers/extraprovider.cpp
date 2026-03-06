/**
 * @file extraprovider.cpp
 * @brief Implementation of ExtraProvider class
 */

#include "providers/extraprovider.hpp"

ExtraProvider::ExtraProvider(QObject *parent)
    : BaseProvider(parent)
{
    qDebug() << "[ExtraProvider] Initialized";
    updateTime();
    connect(&_timer, &QTimer::timeout, this, &ExtraProvider::updateTime);
    _timer.start(1000);
}

QString ExtraProvider::time() const
{
    QMutexLocker locker(&_mutex);
    return _timeStr;
}

QString ExtraProvider::date() const
{
    QMutexLocker locker(&_mutex);
    return _dateStr;
}

void ExtraProvider::updateTime()
{
    bool timeChanged = false, dateChanged = false;
    {
        QMutexLocker locker(&_mutex);
        QDateTime now = QDateTime::currentDateTime();
        QString newTime = now.toString("hh:mm");
        QString newDate = now.toString("ddd dd").toUpper();
        if (newTime != _timeStr) { _timeStr = newTime; timeChanged = true; }
        if (newDate != _dateStr) { _dateStr = newDate; dateChanged = true; }
    }
    if (timeChanged) emit this->timeChanged();
    if (dateChanged) emit this->dateChanged();
}