/**
 * @file otaprovider.hpp
 * @brief Provider for Vehicle OTA signals
 * @desc Manages system updates
 */

#ifndef OTAPROVIDER_HPP
#define OTAPROVIDER_HPP

#include "core/baseprovider.hpp"
#include <QString>
#include <QMutex>
#include <QVariant>

/**
 * @class OTAProvider
 * @brief Handles all ota-related vehicle signals
 *
 * Manages:
 * - Instaled Version Track (Vehicle.OTA.InstalledVersion)
 * - Pending Version (Vehicle.OTA.PendingVersion)
 * - Update Available on system (Vehicle.OTA.UpdateAvailable)
 *
 */
class OTAProvider : public BaseProvider
{
    Q_OBJECT

    Q_PROPERTY(QString installedVersion READ installedVersion NOTIFY installedVersionChanged)
    Q_PROPERTY(QString pendingVersion READ pendingVersion NOTIFY pendingVersionChanged)
    Q_PROPERTY(bool isUpdateAvailable READ isUpdateAvailable NOTIFY isUpdateAvailableChanged)

public:
    explicit OTAProvider(QObject *parent = nullptr);
    ~OTAProvider() override = default;

    QString providerName() const override { return "OTAProvider"; }

    QString installedVersion() const;
    QString pendingVersion() const;
    bool isUpdateAvailable() const;

public slots:
    void updateInstalledVersion(QString installedVersion);
    void updatePendingVersion(QString pendingVersion);
    void updateIsUpdateAvailable(bool updateAvailable);
    Q_INVOKABLE void triggerUpdate();

signals:
    void installedVersionChanged();
    void pendingVersionChanged();
    void isUpdateAvailableChanged();
    void requestWrite(QString path, QVariant value);

private:
    // Raw values
    QString _installedVersionValue;
    QString _pendingVersionValue;
    bool _isUpdateAvailable;

    // Formatted strings for QML display

    // Helper Functions
};

#endif /* OTAPROVIDER_HPP */