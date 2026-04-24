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

/**
 * @class ADASProvider
 * @brief Handles all adas-related vehicle signals
 *
 * Manages:
 * - Front Distance     (Vehicle.ADAS.ObstacleDetection.Front.Distance)
 * - Lateral Deviation  (Vehicle.ADAS.LaneKeepAssist.LateralDeviation)
 * - Lane Status        (Vehicle.ADAS.LaneKeepAssist.LaneStatus)
 */

class ADASProvider : public BaseProvider
{
    Q_OBJECT

    Q_PROPERTY(QString frontDistance READ frontDistance NOTIFY frontDistanceChanged)
    Q_PROPERTY(QString lateralDeviation READ lateralDeviation NOTIFY lateralDeviationChanged)
    Q_PROPERTY(QString laneStatus READ laneStatus NOTIFY laneStatusChanged)
    Q_PROPERTY(QList<double> lateralDeviationHistory READ lateralDeviationHistory NOTIFY lateralDeviationChanged)

public:
    explicit ADASProvider(QObject *parent = nullptr);
    ~ADASProvider() override = default;

    QString providerName() const override { return "ADASProvider"; }

    QString frontDistance() const;
    QString lateralDeviation() const;
    QString laneStatus() const;
    QList<double> lateralDeviationHistory() const;

public slots:
    void updateFrontDistance(double frontDistance);
    void updateLateralDeviation(double lateralDeviation);
    void updateLaneStatus(QString laneStatus);

signals:
    void frontDistanceChanged();
    void lateralDeviationChanged();
    void laneStatusChanged();

private:
    // Raw Values
    double _frontDistanceValue;
    double _lateralDeviationValue;
    QString _laneStatusValue;

    // formatted strings for QML display
    QString _frontDistanceStr;
    QString _lateralDeviationStr;
    QString _laneStatusStr;

    // Helpers
    QList<double> _lateralDeviationHistory;
};

#endif /* ADASPROVIDER_HPP */