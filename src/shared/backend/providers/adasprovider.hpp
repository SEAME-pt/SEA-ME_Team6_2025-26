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
#include <QVariant>
#include <QStringList>

/**
 * @class ADASProvider
 * @brief Handles all adas-related vehicle signals
 * 
 * Manages:
 * - Front Distance     (Vehicle.ADAS.ObstacleDetection.Front.Distance)
 * - LKA Status         (Vehicle.ADAS.LaneKeepAssist.IsEnabled)
 * - Lateral Deviation  (Vehicle.ADAS.LaneKeepAssist.LateralDeviation)
 * - Lane Status        (Vehicle.ADAS.LaneKeepAssist.LaneStatus)
 * - TSR Status         (Vehicle.ADAS.ObjectDetection.IsEnabled)
 * - Speed Limit        (Vehicle.ADAS.ObjectDetection.SpeedLimit)
 * - Traffic Light      (Vehicle.ADAS.ObjectDetection.TrafficLight)
 * - StreetSignals      (Vehicle.ADAS.ObjectDetection.StreetSignals)
 * - Extras             (Vehicle.ADAS.ObjectDetection.Extras)
 * - CC Status          (Vehicle.ADAS.CruiseControl.IsEnabled)
 * - AEB Status         (Vehicle.ADAS.AEB.IsEnabled)
 *
 * Triggers:
 * - Features on/off    (Vehicle.ADAS.LaneKeepAssist.IsEnabled)
 *                      (Vehcile.ADAS.CruiseControl.IsEnabled)
 *                      (Vehicle.ADAS.AEB.IsEnabled)
 *                      (Vehicle.ADAS.ObjectDetection.IsEnabled)
 *
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
    Q_PROPERTY(bool LKAStatus READ LKAStatus NOTIFY LKAStatusChanged)
    Q_PROPERTY(bool CCStatus READ CCStatus NOTIFY CCStatusChanged)
    Q_PROPERTY(bool AEBStatus READ AEBStatus NOTIFY AEBStatusChanged)
    Q_PROPERTY(bool TSRStatus READ TSRStatus NOTIFY TSRStatusChanged)

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
    bool LKAStatus() const;
    bool CCStatus() const;
    bool AEBStatus() const;
    bool TSRStatus() const;

public slots:
    void updateFrontDistance(double frontDistance);
    void updateLateralDeviation(double lateralDeviation);
    void updateLaneStatus(QString laneStatus);
    void updateSpeedLimit(double speedLimit);
    void updateTrafficLight(QString trafficLight);
    void updateStreetSignals(const QStringList &streetSignals);
    void updateExtras(const QStringList &extras);
    void updateLKAStatus(bool LKAStatus);
    void updateCCStatus(bool CCStatus);
    void updateAEBStatus(bool AEBStatus);
    void updateTSRStatus(bool statTSRStatusus);
    Q_INVOKABLE void triggerUpdateADASFeature(QString VSSPath, bool value);

signals:
    void frontDistanceChanged();
    void lateralDeviationChanged();
    void laneStatusChanged();
    void speedLimitChanged();
    void trafficLightChanged();
    void streetSignalsChanged();
    void extrasChanged();
    void LKAStatusChanged();
    void CCStatusChanged();
    void AEBStatusChanged();
    void TSRStatusChanged();
    void requestWrite(QString path, QVariant value);

private:
    // Raw Values
    double _frontDistanceValue;
    double _lateralDeviationValue;
    QString _laneStatusValue;
    double _speedLimitValue;
    QString _trafficLightValue;
    QStringList _streetSignalsValue;
    QStringList _extrasValue;
    bool _LKAValue;
    bool _CCValue;
    bool _AEBValue;
    bool _TSRValue;

    // formatted strings for QML display
    QString _frontDistanceStr;
};

#endif /* ADASPROVIDER_HPP */