/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @create date 2025-12-16 14:40:18
 * @modify date 2025-12-16 14:42:44
 * @desc Class that contains wheel angle functions and exposition to QML
 */

#ifndef wheelangleprovider
#define wheelangleprovider

#include <QObject>
#include <QTimer>
#include <QtMath>


class WheelAngleProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currWheelAngle READ currWheelAngle NOTIFY wheelAngleChanged)
    Q_PROPERTY(bool isBlinkerLeftActive READ isBlinkerLeftActive NOTIFY wheelAngleTurnLeft)
    Q_PROPERTY(bool isBlinkerRightActive READ isBlinkerRightActive NOTIFY wheelAngleTurnRight)

public:
    explicit WheelAngleProvider(QObject *parent = nullptr);

    QString currWheelAngle() const;
    bool    isBlinkerLeftActive() const;
    bool    isBlinkerRightActive() const;

public slots:
    void setWheelAngle(double wheelAngle);

signals:
    void wheelAngleChanged();
    void wheelAngleTurnLeft();
    void wheelAngleTurnRight();

private:
    QString _currWheelAngle;
    double _wheelAngleValue = 0.0;
    bool _isBlinkerLeftActive = false;
    bool _isBlinkerRightActive = false;
};

#endif /* wheelangleprovider */
