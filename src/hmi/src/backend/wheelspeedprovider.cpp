/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @desc wheelspeedprovider Class functions
 */

#include "wheelspeedprovider.hpp"

WheelSpeedProvider::WheelSpeedProvider(QObject *parent)
    : QObject(parent), _wheelSpeedValue(0.0), _currWheelSpeed(QString::number(0.0, 'f', 1)) {
    }

void WheelSpeedProvider::setWheelSpeed(double wheelSpeed) {
    if (!qFuzzyCompare(wheelSpeed, _wheelSpeedValue)) {
        _wheelSpeedValue = wheelSpeed;
        _currWheelSpeed = QString::number(_wheelSpeedValue, 'f', 1);
        emit wheelSpeedChanged();
    }
}

QString WheelSpeedProvider::currWheelSpeed() const {
    return _currWheelSpeed;
}