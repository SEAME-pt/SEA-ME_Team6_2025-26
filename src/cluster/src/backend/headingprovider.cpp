/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @desc headingprovider Class functions
 */

#include "headingprovider.hpp"

HeadingProvider::HeadingProvider(QObject *parent)
    : QObject(parent), _headingValue(0.0), _currHeading(QString::number(0.0, 'f', 1)) {
    }

void HeadingProvider::setHeading(double heading) {
    if (!qFuzzyCompare(heading, _headingValue)) {
        _headingValue = heading;
        _currHeading = QString::number(_headingValue, 'f', 1);
        emit headingChanged();
    }
}

QString HeadingProvider::currHeading() const {
    return _currHeading;
}