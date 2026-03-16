/**
 * @file extraprovider.hpp
 * @brief Provider for Vehicle Extra signals
 * @desc Manages Time and Date
 */

#ifndef EXTRAPROVIDER_HPP
#define EXTRAPROVIDER_HPP

#include "core/baseprovider.hpp"
#include <QString>
#include <QMutex>
#include <QTimer>
#include <QDateTime>
#include <cmath>
#include <unistd.h>

/**
 * @class ExtraProvider
 * @brief Handles all top-level-related vehicle signals
 *
 * Manages:
 * - Car Speed (Vehicle.Speed)
 */
class ExtraProvider : public BaseProvider
{
    Q_OBJECT

    Q_PROPERTY(QString time READ time NOTIFY timeChanged)
    Q_PROPERTY(QString date READ date NOTIFY dateChanged)

public:
    explicit ExtraProvider(QObject *parent = nullptr);
    ~ExtraProvider() override = default;

    QString providerName() const override { return "ExtraProvider"; }

    QString time() const;
    QString date() const;

public slots:
    void updateTime();

signals:
    void timeChanged();
    void dateChanged();

private:
    // Formatted strings for QML display
    QString _timeStr;
    QString _dateStr;

    //Extras
    QTimer _timer;
};

#endif /* EXTRAPROVIDER_HPP */