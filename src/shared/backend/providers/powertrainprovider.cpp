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
    
    _motorSpeedValue = speed;
    _motorSpeedStr = QString::number(static_cast<int>(_motorSpeedValue));
    
    locker.unlock();
    emit motorSpeedChanged();
}

void PowertrainProvider::updateBatteryLowStatus(bool isLow)
{
    QMutexLocker locker(&_mutex);
    
    if (_isBatteryLow == isLow)
        return;
    
    _isBatteryLow = isLow;

    updateBatteryIconInternal();
    
    locker.unlock();
    emit batteryLowStatusChanged();
}

void PowertrainProvider::updateBatteryCriticalStatus(bool isCritical)
{
    QMutexLocker locker(&_mutex);
    
    if (_isBatteryCritical == isCritical)
        return;
    
    _isBatteryCritical = isCritical;
    
    updateBatteryIconInternal();
    
    locker.unlock();
    emit batteryCriticalStatusChanged();
}

void PowertrainProvider::updateBatteryIconInternal()
{
    QString newIcon;
    
    if (_isBatteryCritical)
        newIcon = FLASH_DANGER_ICON;
    else if (_isBatteryLow)
        newIcon = FLASH_WARNING_ICON;
    else
        newIcon = FLASH_ICON;
    
    if (newIcon != _batteryVoltageIcon) {
        _batteryVoltageIcon = newIcon;
        emit batteryVoltageIconChanged();
    }
}