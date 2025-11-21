#include "timeprovider.hpp"

TimeProvider::TimeProvider(QObject *parent): QObject(parent) {
    updateTime();
    connect(&_timer, &QTimer::timeout, this, &TimeProvider::updateTime);
    _timer.start(1000);
}

void TimeProvider::updateTime() {
    QDateTime now = QDateTime::currentDateTime();

    QString newTime = now.toString("h:mm AP");
    QString newDate = now.toString("dddd | MMMM d, yyyy");

    if (newTime != _currTime) {
        _currTime = newTime;
        emit timeChanged();
    }
    if (newDate != _currDate) {
        _currDate = newDate;
        emit dateChanged();
    }
}

QString TimeProvider::currTime() const { return _currTime; }
QString TimeProvider::currDate() const { return _currDate; }

//g++ main.cpp -o datetime_test $(pkg-config --cflags --libs Qt6Core)
