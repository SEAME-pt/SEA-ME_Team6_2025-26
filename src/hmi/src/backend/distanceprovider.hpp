/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @desc Class that contains distance traveled functions and exposition to QML
 */

#ifndef distanceprovider
#define distanceprovider

#include <QObject>
#include <QTimer>
#include <QtMath>

class DistanceProvider : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currDistance READ currDistance NOTIFY distanceChanged)

    public:
        explicit DistanceProvider(QObject *parent = nullptr);
        QString currDistance() const;

    public slots:
        void setDistance(double distance);

    signals:
        void distanceChanged();

    private:
        QString _currDistance;
        double _distanceValue = 0.0;
};

#endif /* distanceprovider */