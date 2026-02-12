/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @desc Class that contains temperature functions and exposition to QML
 */

#ifndef temperatureprovider
#define temperatureprovider

#include <QObject>
#include <QTimer>
#include <QtMath>

class TemperatureProvider : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currTemperature READ currTemperature NOTIFY temperatureChanged)

public:
    explicit TemperatureProvider(QObject *parent = nullptr);

    QString currTemperature() const;

public slots:
    void setTemperature(double temperature);

signals:
    void temperatureChanged();

private:
    QString _currTemperature;
    double _temperatureValue = 0.0;
};


#endif /* temperatureprovider */
