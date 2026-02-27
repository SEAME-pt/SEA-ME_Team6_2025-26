/**
 * @file powertrainprovider.hpp
 * @brief Provider for Vehicle Powertrain signals
 * @desc Manages battery voltage, motor speed, and battery status signals
 */

#ifndef POWERTRAINPROVIDER_HPP
#define POWERTRAINPROVIDER_HPP

#include "core/baseprovider.hpp"
#include <QString>
#include <QMutex>

#define FLASH_ICON "qrc:/assets/icons/flash.svg"
#define FLASH_WARNING_ICON "qrc:/assets/icons/flash-warning.svg"
#define FLASH_DANGER_ICON "qrc:/assets/icons/flash-danger.svg"

/**
 * @class PowertrainProvider
 * @brief Handles all powertrain-related vehicle signals
 *
 * Manages:
 * - Battery voltage (Vehicle.Powertrain.TractionBattery.CurrentVoltage)
 * - Battery voltage icon
 * - Motor speed (Vehicle.Powertrain.ElectricMotor.Speed)
 * - Battery low status (Vehicle.Powertrain.TractionBattery.IsLevelLow)
 * - Battery critical status (Vehicle.Powertrain.TractionBattery.IsCritical)
 */
class PowertrainProvider : public BaseProvider
{
    Q_OBJECT

    Q_PROPERTY(QString batteryVoltage READ batteryVoltage NOTIFY batteryVoltageChanged)
    Q_PROPERTY(QString batteryVoltageIcon READ batteryVoltageIcon NOTIFY batteryVoltageIconChanged)
    Q_PROPERTY(QString motorSpeed READ motorSpeed NOTIFY motorSpeedChanged)
    Q_PROPERTY(bool isBatteryLow READ isBatteryLow NOTIFY batteryLowStatusChanged)
    Q_PROPERTY(bool isBatteryCritical READ isBatteryCritical NOTIFY batteryCriticalStatusChanged)

public:
    explicit PowertrainProvider(QObject *parent = nullptr);
    ~PowertrainProvider() override = default;

    QString providerName() const override { return "PowertrainProvider"; }

    QString batteryVoltage() const;
    QString batteryVoltageIcon() const;
    QString motorSpeed() const;
    bool isBatteryLow() const;
    bool isBatteryCritical() const;

public slots:
    void updateBatteryVoltage(double voltage);
    void updateMotorSpeed(double speed);
    void updateBatteryLowStatus(bool isLow);
    void updateBatteryCriticalStatus(bool isCritical);

signals:
    void batteryVoltageChanged();
    void batteryVoltageIconChanged();
    void motorSpeedChanged();
    void batteryLowStatusChanged();
    void batteryCriticalStatusChanged();

private:
    // Raw values
    double _batteryVoltageValue;
    double _motorSpeedValue;
    bool _isBatteryLow;
    bool _isBatteryCritical;

    // Formatted strings for QML display
    QString _batteryVoltageStr;
    QString _batteryVoltageIcon;
    QString _motorSpeedStr;

    // Helper Functions
    void updateBatteryIconInternal();
};

#endif /* POWERTRAINPROVIDER_HPP */