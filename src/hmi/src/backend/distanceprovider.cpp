/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @desc distanceprovider Class functions
 */

#include "distanceprovider.hpp"

DistanceProvider::DistanceProvider(QObject *parent)
    : QObject(parent), _distanceValue(0.0), _currDistance(QString::number(0.0, 'f', 1)) {
    }

void DistanceProvider::setDistance(double distance) {
    if (!qFuzzyCompare(distance, _distanceValue)) {
        _distanceValue = distance;
        _currDistance = QString::number(_distanceValue, 'f', 1);
        emit distanceChanged();
    }
}

QString DistanceProvider::currDistance() const {
    return _currDistance;
}