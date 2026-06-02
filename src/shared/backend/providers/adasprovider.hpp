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

/**
 * @class ADASProvider
 * @brief Handles all adas-related vehicle signals
 * 
 * Manages:
 * - Front Distance (Vehicle.ADAS.ObstacleDetection.Front.Distance)
 */

class ADASProvider : public BaseProvider
{
    Q_OBJECT

    Q_PROPERTY(QString frontDistance READ frontDistance NOTIFY frontDistanceChanged)

public:
    explicit ADASProvider(QObject *parent = nullptr);
    ~ADASProvider() override = default;

    QString providerName() const override { return "ADASProvider"; }

    QString frontDistance() const;

public slots:
    void updateFrontDistance(double frontDistance);

signals:
    void frontDistanceChanged();

private:
    // Raw Values
    double _frontDistanceValue;

    // formatted strings for QML display
    QString _frontDistanceStr;
};

#endif /* ADASPROVIDER_HPP */