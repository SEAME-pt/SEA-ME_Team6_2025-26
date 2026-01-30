/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @desc voltageprovider Class functions
 */

#include "voltageprovider.hpp"

VoltageProvider::VoltageProvider(QObject *parent)
    : QObject(parent), _voltageValue(0.0), _currVoltage(QString::number(0.0, 'f', 1)) {
    }

void VoltageProvider::setVoltage(double voltage) {
    if (!qFuzzyCompare(voltage, _voltageValue)) {
        _voltageValue = voltage;
        _currVoltage = QString::number(_voltageValue, 'f', 1);
        emit voltageChanged();
    }
}

QString VoltageProvider::currVoltage() const {
    return _currVoltage;
}