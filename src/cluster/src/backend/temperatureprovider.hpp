/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @create date 2025-12-16 14:45:29
 * @modify date 2025-12-16 14:46:01
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
