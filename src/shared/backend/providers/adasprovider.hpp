/**
 * @file adasprovider.hpp
 * @brief Provider for Vehicle ADAS signals
 * @desc Manages Obstacle Detection Front Distance
 */

#ifndef ADASPROVIDER_HPP
#define ADASPROVIDER_HPP

#include "core/baseprovider.hpp"
#include <QString>
#include <QMutex>
#include <QList>
#include <QStringList>

/**
 * @class ADASProvider
 * @brief Handles all adas-related vehicle signals
 *
 * Manages:
 * - Front Distance     (Vehicle.ADAS.ObstacleDetection.Front.Distance)
 * - Lateral Deviation  (Vehicle.ADAS.LaneKeepAssist.LateralDeviation)
 * - Lane Status        (Vehicle.ADAS.LaneKeepAssist.LaneStatus)
 * - Speed Limit        (Vehicle.ADAS.ObjectDetection.SpeedLimit)
 * - Traffic Light      (Vehicle.ADAS.ObjectDetection.TrafficLight)
 * - StreetSignals      (Vehicle.ADAS.ObjectDetection.StreetSignals)
 * - Extras             (Vehicle.ADAS.ObjectDetection.Extras)
 */

class ADASProvider : public BaseProvider
{
    Q_OBJECT

    Q_PROPERTY(QString frontDistance READ frontDistance NOTIFY frontDistanceChanged)
    Q_PROPERTY(QString lateralDeviation READ lateralDeviation NOTIFY lateralDeviationChanged)
    Q_PROPERTY(QString laneStatus READ laneStatus NOTIFY laneStatusChanged)
    Q_PROPERTY(QList<double> lateralDeviationHistory READ lateralDeviationHistory NOTIFY lateralDeviationChanged)
    Q_PROPERTY(QString speedLimit READ speedLimit NOTIFY speedLimitChanged)
    Q_PROPERTY(QString trafficLight READ trafficLight NOTIFY trafficLightChanged)
    Q_PROPERTY(QStringList streetSignals READ streetSignals NOTIFY streetSignalsChanged)
    Q_PROPERTY(QStringList extras READ extras NOTIFY extrasChanged)

public:
    explicit ADASProvider(QObject *parent = nullptr);
    ~ADASProvider() override = default;

    QString providerName() const override { return "ADASProvider"; }

    QString frontDistance() const;
    QString lateralDeviation() const;
    QString laneStatus() const;
    QList<double> lateralDeviationHistory() const;
    QString speedLimit() const;
    QString trafficLight() const;
    QStringList streetSignals() const;
    QStringList extras() const;

public slots:
    void updateFrontDistance(double frontDistance);
    void updateLateralDeviation(double lateralDeviation);
    void updateLaneStatus(QString laneStatus);
    void updateSpeedLimit(double speedLimit);
    void updateTrafficLight(QString trafficLight);
    void updateStreetSignals(const QStringList &streetSignals);
    void updateExtras(const QStringList &extras);

signals:
    void frontDistanceChanged();
    void lateralDeviationChanged();
    void laneStatusChanged();
    void speedLimitChanged();
    void trafficLightChanged();
    void streetSignalsChanged();
    void extrasChanged();

private:
    // Raw Values
    double _frontDistanceValue;
    double _lateralDeviationValue;
    QString _laneStatusValue;
    double _speedLimitValue;
    QString _trafficLightValue;
    QStringList _streetSignalsValue;
    QStringList _extrasValue;

    // formatted strings for QML display
    QString _frontDistanceStr;
    QString _lateralDeviationStr;
    QString _speedLimitStr;

    // Helpers
    QList<double> _lateralDeviationHistory;
};

#endif /* ADASPROVIDER_HPP */