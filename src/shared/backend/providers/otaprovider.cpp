/**
 * @file otaprovider.cpp
 * @brief Implementation of OTAProvider class
 */

#include "providers/otaprovider.hpp"

OTAProvider::OTAProvider(QObject *parent)
    : BaseProvider(parent),
      _installedVersionValue(""),
      _pendingVersionValue(""),
      _isUpdateAvailable(false)
{
    qDebug() << "[OTAProvider] Initialized";
}

QString OTAProvider::installedVersion() const
{
    QMutexLocker locker(&_mutex);
    return _installedVersionValue;
}

QString OTAProvider::pendingVersion() const
{
    QMutexLocker locker(&_mutex);
    return _pendingVersionValue;
}

bool OTAProvider::isUpdateAvailable() const
{
    QMutexLocker locker(&_mutex);
    return _isUpdateAvailable;
}

void OTAProvider::updateInstalledVersion(QString installedVersion)
{
    QMutexLocker locker(&_mutex);

    if (installedVersion == _installedVersionValue)
        return;

    _installedVersionValue = installedVersion;

    locker.unlock();
    emit installedVersionChanged();
}

void OTAProvider::updatePendingVersion(QString pendingVersion)
{
    QMutexLocker locker(&_mutex);

    if (pendingVersion == _pendingVersionValue)
        return;

    _pendingVersionValue = pendingVersion;

    locker.unlock();
    emit pendingVersionChanged();
}

void OTAProvider::updateIsUpdateAvailable(bool updateAvailable)
{
    QMutexLocker locker(&_mutex);

    if (_isUpdateAvailable == updateAvailable) return;

    _isUpdateAvailable = updateAvailable;

    locker.unlock();
    emit isUpdateAvailableChanged();
}

void OTAProvider::triggerUpdate()
{
    qDebug() << "[OTAProvider] Triggering update...";
    emit requestWrite("Vehicle.OTA.TestUpdate", QVariant(true));
}