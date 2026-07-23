/**
 * @file signalrouter.hpp
 * @brief Routes Kuksa signals to appropriate providers
 * @desc Central dispatcher that maps signal paths to provider methods
 */

#ifndef SIGNALROUTER_HPP
#define SIGNALROUTER_HPP

#include <QObject>
#include <QDebug>
#include <QString>
#include <QVariant>
#include <QMap>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStringList>
#include <functional>

class ADASProvider;
class ExteriorProvider;
class PowertrainProvider;
class VehicleProvider;
class CurrentLocationProvider;
class ChassisProvider;
class OTAProvider;

/**
 * @class SignalRouter
 * @brief Routes vehicle signals from Reader to appropriate providers
 * 
 * Receives generic signals (path + value) from Reader and dispatches
 * them to the correct provider based on the signal path.
 */
class SignalRouter : public QObject
{
    Q_OBJECT

public:
    explicit SignalRouter(QObject *parent = nullptr);
    ~SignalRouter() = default;

    /**
     * @brief Register providers with the router
     */
    void registerPowertrainProvider(PowertrainProvider *provider);
    void registerVehicleProvider(VehicleProvider *provider);
    void registerExteriorProvider(ExteriorProvider *provider);
    void registerADASProvider(ADASProvider *provider);
    void registerCurrentLocationProvider(CurrentLocationProvider *provider);
    void registerChassisProvider(ChassisProvider *provider);
    void registerOTAProvider(OTAProvider *provider);

public slots:
    /**
     * @brief Single entry point for all signals from Reader
     * Routes based on path string to appropriate provider
     */
    void routeSignal(const QString &path, const QVariant &value);

signals:
    /**
     * @brief Emitted when routing encounters an error
     */
    void routingError(QString error);

private:
    PowertrainProvider *_powertrainProvider;
    VehicleProvider *_vehicleProvider;
    ExteriorProvider *_exteriorProvider;
    ADASProvider *_adasProvider;
    CurrentLocationProvider *_currentLocationProvider;
    ChassisProvider *_chassisProvider;
    OTAProvider *_otaProvider;

    /**
     * @brief Route handlers for each signal domain
     */
    void routePowertrainSignal(const QString &path, const QVariant &value);
    void routeExteriorSignal(const QString &path, const QVariant &value);
    void routeVehicleSignal(const QString &path, const QVariant &value);
    void routeADASSignal(const QString &path, const QVariant &value);
    void routeCurrentLocationSignal(const QString &path, const QVariant &value);
    void routeChassisSignal(const QString &path, const QVariant &value);
    void routeOTASignal(const QString &path, const QVariant &value);

    /**
     * @brief Helper to check if provider is registered
     */
    template<typename T>
    bool isProviderRegistered(T* provider, const QString &providerName);
    QStringList parseStringArray(const QVariant &value);
};

#endif /* SIGNALROUTER_HPP */