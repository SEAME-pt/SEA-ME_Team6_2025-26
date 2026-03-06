/**
 * @file vehicleprovider.hpp
 * @brief Provider for Vehicle Top-Level signals
 * @desc Manages Speed, Average Speed, Traveled Distance, Traveled Distance Since Start and Is Moving
 */

#ifndef VEHICLEPROVIDER_HPP
#define VEHICLEPROVIDER_HPP

#include "core/baseprovider.hpp"
#include <QString>
#include <QMutex>
#include <cmath>
#include <unistd.h>

#define SPEED_CHANGE_THRESHOLD 0.05

/**
 * @class VehicleProvider
 * @brief Handles all top-level-related vehicle signals
 *
 * Manages:
 * - Car Speed (Vehicle.Speed)
 */
class VehicleProvider : public BaseProvider
{
    Q_OBJECT

    Q_PROPERTY(QString speed READ speed NOTIFY speedChanged)

public:
    explicit VehicleProvider(QObject *parent = nullptr);
    ~VehicleProvider() override = default;

    QString providerName() const override { return "VehicleProvider"; }

    QString speed() const;

public slots:
    void updateSpeed(double speed);

signals:
    void speedChanged();

private:
    // Raw Values
    double _speedValue;

    // Formatted strings for QML display
    QString _speedStr;
};

#endif /* VEHICLEPROVIDER_HPP */