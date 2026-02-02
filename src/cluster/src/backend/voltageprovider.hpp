/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @desc Class that contains current voltage functions and exposition to QML
 */

#ifndef voltageprovider
#define voltageprovider

#include <QObject>
#include <QTimer>
#include <QtMath>

class VoltageProvider : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currVoltage READ currVoltage NOTIFY voltageChanged)

public:
    explicit VoltageProvider(QObject *parent = nullptr);

    QString currVoltage() const;

public slots:
    void setVoltage(double voltage);

signals:
    void voltageChanged();

private:
    QString _currVoltage;
    double _voltageValue = 0.0;
};


#endif /* voltageprovider */
