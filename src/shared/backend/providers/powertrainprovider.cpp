/**
 * @file powertrainprovider.cpp
 * @brief Implementation of PowertrainProvider class
 */

#include "providers/powertrainprovider.hpp"

PowertrainProvider::PowertrainProvider(QObject *parent)
    : BaseProvider(parent),
      _batteryVoltageValue(0.0),
      _motorSpeedValue(0.0),
      _isBatteryLow(false),
      _isBatteryCritical(false),
      _batteryVoltageStr("0.0"),
      _batteryVoltageIcon(FLASH_ICON),
      _motorSpeedStr("0")
{
    qDebug() << "[PowertrainProvider] Initialized";
}

QString PowertrainProvider::batteryVoltage() const
{
    QMutexLocker locker(&_mutex);
    return _batteryVoltageStr;
}

QString PowertrainProvider::batteryVoltageIcon() const
{
    QMutexLocker locker(&_mutex);
    return _batteryVoltageIcon;
}

QString PowertrainProvider::motorSpeed() const
{
    QMutexLocker locker(&_mutex);
    return _motorSpeedStr;
}

bool PowertrainProvider::isBatteryLow() const
{
    QMutexLocker locker(&_mutex);
    return _isBatteryLow;
}

bool PowertrainProvider::isBatteryCritical() const
{
    QMutexLocker locker(&_mutex);
    return _isBatteryCritical;
}

void PowertrainProvider::updateBatteryVoltage(double voltage)
{
    QMutexLocker locker(&_mutex);
    
    if (qFuzzyCompare(voltage, _batteryVoltageValue))
        return;
    
    _batteryVoltageValue = voltage;
    _batteryVoltageStr = QString::number(_batteryVoltageValue, 'f', 1);
    
    locker.unlock();
    emit batteryVoltageChanged();
}

void PowertrainProvider::updateMotorSpeed(double speed)
{
    QMutexLocker locker(&_mutex);
    
    if (qFuzzyCompare(speed, _motorSpeedValue))
        return;
    
    if (speed >= 500)
        return;

    _motorSpeedValue = speed;
    _motorSpeedStr = QString::number(static_cast<int>(_motorSpeedValue));
    
    locker.unlock();
    emit motorSpeedChanged();
}

void PowertrainProvider::updateBatteryLowStatus(bool isLow)
{
    bool iconChanged = false;
    {
        QMutexLocker locker(&_mutex);
        if (_isBatteryLow == isLow) return;
        _isBatteryLow = isLow;
        iconChanged = updateBatteryIconInternal();
    }
    emit batteryLowStatusChanged();
    if (iconChanged) emit batteryVoltageIconChanged();
}

void PowertrainProvider::updateBatteryCriticalStatus(bool isCritical)
{
    bool iconChanged = false;
    {
        QMutexLocker locker(&_mutex);
        if (_isBatteryCritical == isCritical) return;
        _isBatteryCritical = isCritical;
        iconChanged = updateBatteryIconInternal();
    }
    emit batteryCriticalStatusChanged();
    if (iconChanged) emit batteryVoltageIconChanged();
}

bool PowertrainProvider::updateBatteryIconInternal()
{
    QString newIcon = _isBatteryCritical ? FLASH_DANGER_ICON
                    : _isBatteryLow      ? FLASH_WARNING_ICON
                                         : FLASH_ICON;
    if (newIcon == _batteryVoltageIcon) return false;
    _batteryVoltageIcon = newIcon;
    return true;
}