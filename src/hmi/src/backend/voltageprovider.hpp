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

#define FLASH_ICON "qrc:/assets/icons/flash.svg"
#define FLASH_WARNING_ICON "qrc:/assets/icons/flash-warning.svg"
#define FLASH_DANGER_ICON "qrc:/assets/icons/flash-danger.svg"


class VoltageProvider : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currVoltage READ currVoltage NOTIFY voltageChanged)
    Q_PROPERTY(QString voltageIcon READ voltageIcon NOTIFY voltageIconChanged)

public:
    explicit VoltageProvider(QObject *parent = nullptr);

    QString currVoltage() const;
    QString voltageIcon() const;

public slots:
    void setVoltage(double voltage);
    void setVoltageIcon(bool status, std::string source);

signals:
    void voltageChanged();
    void voltageIconChanged();

private:
    QString _currVoltage;
    QString _currVoltageIcon;
    double _voltageValue = 0.0;
    bool _isCritical = false;
    bool _isLevelLow = false;
};


#endif /* voltageprovider */
