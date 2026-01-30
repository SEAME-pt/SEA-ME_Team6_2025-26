/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @desc frontdistanceprovider Class functions
 */

#include "frontdistanceprovider.hpp"

FrontDistanceProvider::FrontDistanceProvider(QObject *parent)
    : QObject(parent), _frontDistanceValue(0.0), _currFrontDistance(QString::number(0.0, 'f', 1)) {
    }

void FrontDistanceProvider::setFrontDistance(double frontDistance) {
    if (!qFuzzyCompare(frontDistance, _frontDistanceValue)) {
        _frontDistanceValue = frontDistance;
        _currFrontDistance = QString::number(_frontDistanceValue, 'f', 1);
        emit frontDistanceChanged();
    }
}

QString FrontDistanceProvider::currFrontDistance() const {
    return _currFrontDistance;
}