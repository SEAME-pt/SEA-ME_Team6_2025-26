#include "speedprovider.hpp"

SpeedProvider::SpeedProvider(QObject *parent)
    : QObject(parent), _currSpeed("0") {

    connect(&_timer, &QTimer::timeout, this, &SpeedProvider::updateSpeed);
    _timer.start(150);
}

void SpeedProvider::updateSpeed() {
    _speedValue += _increment;

    if (_speedValue > 40.0) {
        _speedValue = 40.0;
        _increment = -_increment;
    } else if (_speedValue < 0.0) {
        _speedValue = 0.0;
        _increment = -_increment;
    }

    _currSpeed = QString::number(_speedValue, 'f', 1);
    emit speedChanged();
}

QString SpeedProvider::currSpeed() const {
    return _currSpeed;
}
