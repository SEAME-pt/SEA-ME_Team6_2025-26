/**
 * @file exteriorprovider.hpp
 * @brief Provider for Vehicle Exterior signals
 * @desc Manages Air Temperature
 */

#ifndef EXTERIORPROVIDER_HPP
#define EXTERIORPROVIDER_HPP

#include "core/baseprovider.hpp"
#include <QString>
#include <QMutex>

/**
 * @class ExteriorProvider
 * @brief Handles all exterior-related vehicle signals
 *
 * Manages:
 * - Air Temperature (Vehicle.Exterior.AirTemperature)
 */
class ExteriorProvider : public BaseProvider
{
    Q_OBJECT

    Q_PROPERTY(QString airTemperature READ airTemperature NOTIFY airTemperatureChanged)

public:
    explicit ExteriorProvider(QObject *parent = nullptr);
    ~ExteriorProvider() override = default;

    QString providerName() const override { return "ExteriorProvider"; }

    QString airTemperature() const;

public slots:
    void updateAirTemperature(double temperature);

signals:
    void airTemperatureChanged();

private:
    // Raw Values
    double _airTemperatureValue;

    // Formatted strings for QML display
    QString _airTemperatureStr;
};

#endif /* EXTERIORPROVIDER_HPP */