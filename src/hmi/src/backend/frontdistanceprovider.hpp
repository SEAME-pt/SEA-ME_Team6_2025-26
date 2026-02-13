/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @desc Class that contains sensor front distance functions and exposition to QML
 */

#ifndef frontdistanceprovider
#define frontdistanceprovider

#include <QObject>
#include <QTimer>
#include <QtMath>

class FrontDistanceProvider : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currFrontDistance READ currFrontDistance NOTIFY frontDistanceChanged)

    public:
        explicit FrontDistanceProvider(QObject *parent = nullptr);
        QString currFrontDistance() const;

    public slots:
        void setFrontDistance(double frontDistance);

    signals:
        void frontDistanceChanged();

    private:
        QString _currFrontDistance;
        double _frontDistanceValue = 0.0;
};

#endif /* frontdistanceprovider */