/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @desc Class that contains wheelspeed functions and exposition to QML
 */

#ifndef wheelspeedprovider
#define wheelspeedprovider

#include <QObject>
#include <QTimer>
#include <QtMath>

#define MAX_WHEELSPEED 1000.0

class WheelSpeedProvider : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currWheelSpeed READ currWheelSpeed NOTIFY wheelSpeedChanged)

    public:
        explicit WheelSpeedProvider(QObject *parent = nullptr);
        QString currWheelSpeed() const;

    public slots:
        void setWheelSpeed(double wheelSpeed);

    signals:
        void wheelSpeedChanged();

    private:
        QString _currWheelSpeed;
        double _wheelSpeedValue = 0.0;
};

#endif /* wheelspeedprovider */