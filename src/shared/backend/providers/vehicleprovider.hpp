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
#include <QProcess>
#include <QTimer>

#define SPEED_CHANGE_THRESHOLD 0.05

/**
 * @class VehicleProvider
 * @brief Handles all top-level-related vehicle signals
 *
 * Manages:
 * - Car Speed          (Vehicle.Speed)
 * - Max Speed Trip     (Vehicle.MaxSpeedTrip)
 * - Odometer           (Vehicle.TraveledDistance)
 * - Total Energy Trip  (Vehicle.TotalEnergyUsedTrip)
 * - Distance Traveled (Vehicle.TraveledDistanceSinceStart)
 * - Energy Per Km      (Vehicle.EnergyPerKmTrip)
 * - Trip Duration      (Vehicle.TripDuration)
 * - Average Speed      (Vehicle.AverageSpeed)
 */
class VehicleProvider : public BaseProvider
{
    Q_OBJECT

    Q_PROPERTY(QString speed READ speed NOTIFY speedChanged)
    Q_PROPERTY(QString maxSpeedTrip READ maxSpeedTrip NOTIFY maxSpeedTripChanged)
    Q_PROPERTY(QString totalEnergyTrip READ totalEnergyTrip NOTIFY totalEnergyTripChanged)
    Q_PROPERTY(QString traveledDistanceTrip READ traveledDistanceTrip NOTIFY traveledDistanceTripChanged)
    Q_PROPERTY(QString durationTrip READ durationTrip NOTIFY durationTripChanged)
    Q_PROPERTY(QString energyPerKmTrip READ energyPerKmTrip NOTIFY energyPerKmTripChanged)
    Q_PROPERTY(QString traveledDistance READ traveledDistance NOTIFY traveledDistanceChanged)
    Q_PROPERTY(QString averageSpeed READ averageSpeed NOTIFY averageSpeedChanged)

public:
    explicit VehicleProvider(QObject *parent = nullptr);
    ~VehicleProvider() override = default;

    QString providerName() const override { return "VehicleProvider"; }

    QString speed() const;
    QString maxSpeedTrip() const;
    QString totalEnergyTrip() const;
    QString traveledDistanceTrip() const;
    QString durationTrip() const;
    QString energyPerKmTrip() const;
    QString traveledDistance() const;
    QString averageSpeed() const;

public slots:
    void updateSpeed(double speed);
    void updateMaxSpeedTrip(double maxSpeedTrip);
    void updateTotalEnergyTrip(double totalEnergyTrip);
    void updateTraveledDistanceTrip(double traveledDistanceTrip);
    void updateDurationTrip(double durationTrip);
    void updateEnergyPerKmTrip(double energyPerKmTrip);
    void updateTraveledDistance(double traveledDistance);
    void updateAverageSpeed(double averageSpeed);

signals:
    void speedChanged();
    void maxSpeedTripChanged();
    void totalEnergyTripChanged();
    void traveledDistanceTripChanged();
    void durationTripChanged();
    void energyPerKmTripChanged();
    void traveledDistanceChanged();
    void averageSpeedChanged();

private:
    // Raw Values
    double _speedValue;
    double _maxSpeedTripValue;
    double _totalEnergyTripValue;
    double _traveledDistanceTripValue;
    double _durationTripValue;
    double _energyPerKmTripValue;
    double _traveledDistanceValue;
    double _averageSpeedValue;

    // Formatted strings for QML display
    QString _speedStr;
    QString _maxSpeedTripStr;
    QString _totalEnergyTripStr;
    QString _traveledDistanceTripStr;
    QString _durationTripStr;
    QString _energyPerKmTripStr;
    QString _traveledDistanceStr;
    QString _averageSpeedStr;
};

#endif /* VEHICLEPROVIDER_HPP */