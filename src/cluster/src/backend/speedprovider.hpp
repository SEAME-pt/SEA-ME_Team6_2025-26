/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @create date 2025-12-16 14:40:18
 * @modify date 2025-12-16 14:42:44
 * @desc Class that contains speed functions and exposition to QML
 */

#ifndef speedprovider
#define speedprovider

#include <QObject>
#include <QTimer>
#include <QtMath>

class SpeedProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currSpeed READ currSpeed NOTIFY speedChanged)

public:
    explicit SpeedProvider(QObject *parent = nullptr);

    QString currSpeed() const;

public slots:
    void setSpeed(double speed);

signals:
    void speedChanged();

private:
    QString _currSpeed;
    double _speedValue = 0.0;
};

#endif /* speedprovider */