/**
 * @file chassisprovider.hpp
 * @brief Provider for Vehicle Chassis signals
 * @desc Manages Steering Wheel Angle, Blinkers
 */

#ifndef CHASSISPROVIDER_HPP
#define CHASSISPROVIDER_HPP

#include "core/baseprovider.hpp"
#include <QString>
#include <QMutex>

/**
 * @class ChassisProvider
 * @brief Handles all chassis-related vehicle signals
 *
 * Manages:
 * - Steering Wheel Angle (Vehicle.Chassis.SteeringWheel.Angle)
 * - Blinkers
 */

class ChassisProvider : public BaseProvider
{
    Q_OBJECT

    Q_PROPERTY(QString steeringWheelAngle READ steeringWheelAngle NOTIFY steeringWheelAngleChanged)
    Q_PROPERTY(bool isBlinkerLeftActive READ isBlinkerLeftActive NOTIFY blinkerLeftChanged)
    Q_PROPERTY(bool isBlinkerRightActive READ isBlinkerRightActive NOTIFY blinkerRightChanged)

public:
    explicit ChassisProvider(QObject *parent = nullptr);
    ~ChassisProvider() override = default;

    QString providerName() const override { return "ChassisProvider"; }

    QString steeringWheelAngle() const;
    bool isBlinkerLeftActive() const;
    bool isBlinkerRightActive() const;

public slots:
    void updateSteeringWheelAngle(double steeringWheelAngle);

signals:
    void steeringWheelAngleChanged();
    void blinkerLeftChanged();
    void blinkerRightChanged();

private:
    // Raw Values
    double _steeringWheelAngleValue;
    bool _isBlinkerLeftActive;
    bool _isBlinkerRightActive;

    // Formatted strings for QML display
    QString _steeringWheelAngleStr;

    // Helper Functions
    bool updateBlinkersInternal();
};

#endif /* CHASSISPROVIDER_HPP */