/**
 * @file currentlocationprovider.hpp
 * @brief Provider for Vehicle CurrentLocation signals
 * @desc Manages Heading
 */

#ifndef CURRENTLOCATIONPROVIDER_HPP
#define CURRENTLOCATIONPROVIDER_HPP

#include "core/baseprovider.hpp"
#include <QString>
#include <QMutex>

/**
 * @class CurrentLocationProvider
 * @brief Handles all current-location-related vehicle signals
 *
 * Manages:
 * - Heading (Vehicle.CurrentLocation.Heading)
 */

class CurrentLocationProvider : public BaseProvider
{
    Q_OBJECT

    Q_PROPERTY(QString heading READ heading NOTIFY headingChanged)

public:
    explicit CurrentLocationProvider(QObject *parent = nullptr);
    ~CurrentLocationProvider() override = default;

    QString providerName() const override { return "CurrentLocationProvider"; }

    QString heading() const;

public slots:
    void updateHeading(double heading);

signals:
    void headingChanged();

private:
    // Raw Values
    double _headingValue;

    // Formatted strings for QML display
    QString _headingStr;
};

#endif /* CURRENTLOCATIONPROVIDER_HPP */