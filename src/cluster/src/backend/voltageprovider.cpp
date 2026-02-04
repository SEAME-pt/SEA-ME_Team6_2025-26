/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @desc voltageprovider Class functions
 */

#include "voltageprovider.hpp"

VoltageProvider::VoltageProvider(QObject *parent)
    : QObject(parent), 
      _voltageValue(0.0), 
      _currVoltage(QString::number(0.0, 'f', 1)),
      _currVoltageIcon(FLASH_ICON),
      _isCritical(false),
      _isLevelLow(false) {
}

void VoltageProvider::setVoltage(double voltage) {
    if (!qFuzzyCompare(voltage, _voltageValue)) {
        _voltageValue = voltage;
        _currVoltage = QString::number(_voltageValue, 'f', 1);
        emit voltageChanged();
    }
}

void VoltageProvider::setVoltageIcon(bool status, std::string source) {
    if (source == "critical")
        _isCritical = status;
    else if (source == "low")
        _isLevelLow = status;
    
    QString newIcon;
    if (_isCritical)
        newIcon = FLASH_DANGER_ICON;
    else if (_isLevelLow)
        newIcon = FLASH_WARNING_ICON;
    else
        newIcon = FLASH_ICON;
    
    if (newIcon != _currVoltageIcon) {
        _currVoltageIcon = newIcon;
        emit voltageIconChanged();
    }
}

QString VoltageProvider::currVoltage() const {
    return _currVoltage;
}

QString VoltageProvider::voltageIcon() const {
    return _currVoltageIcon;
}