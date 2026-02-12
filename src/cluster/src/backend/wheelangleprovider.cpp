/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @create date 2025-12-16 14:41:53
 * @modify date 2025-12-17 13:03:17
 * @desc WheelAngleProvider Class functions
 */

#include "wheelangleprovider.hpp"

WheelAngleProvider::WheelAngleProvider(QObject *parent)
    : QObject(parent), _wheelAngleValue(0.0), _currWheelAngle(QString::number(0.0, 'f', 1)) {
}

void WheelAngleProvider::setWheelAngle(double wheelAngle) {

    if (!qFuzzyCompare(wheelAngle, _wheelAngleValue)) {
        _wheelAngleValue = wheelAngle;
        _currWheelAngle = QString::number(_wheelAngleValue, 'f', 1);
        emit wheelAngleChanged();

        if (wheelAngle < 0) {
            if (!_isBlinkerLeftActive) {
                _isBlinkerLeftActive = true;
                _isBlinkerRightActive = false;
                emit wheelAngleTurnLeft();
                emit wheelAngleTurnRight();
            }
        } else if (wheelAngle > 0) {
            if (!_isBlinkerRightActive) {
                _isBlinkerRightActive = true;
                _isBlinkerLeftActive = false;
                emit wheelAngleTurnRight();
                emit wheelAngleTurnLeft();
            }
        } else {
            if (_isBlinkerLeftActive || _isBlinkerRightActive) {
                _isBlinkerLeftActive = false;
                _isBlinkerRightActive = false;
                emit wheelAngleTurnLeft();
                emit wheelAngleTurnRight();
            }
        }
    }
}

QString WheelAngleProvider::currWheelAngle() const {
    return _currWheelAngle;
}

bool WheelAngleProvider::isBlinkerLeftActive() const {
    return _isBlinkerLeftActive;
}

bool WheelAngleProvider::isBlinkerRightActive() const {
    return _isBlinkerRightActive;
}