/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @desc Root Source Code for the QT App
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QDebug>
#include <QQmlContext>

#include "backend/timeprovider.hpp"
#include "backend/systemstatus.hpp" 
#include "backend/speedprovider.hpp"
#include "backend/temperatureprovider.hpp"
#include "backend/distanceprovider.hpp"
#include "backend/frontdistanceprovider.hpp"
#include "backend/wheelspeedprovider.hpp"
#include "backend/voltageprovider.hpp"
#include "backend/ipcclient.hpp"
#include "backend/reader.hpp"

void ipcConnections(QQmlApplicationEngine *engine)
{
    auto speedProvider = new SpeedProvider;
    auto temperatureProvider = new TemperatureProvider;
    auto distanceProvider = new DistanceProvider;
    auto frontDistanceProvider = new FrontDistanceProvider;
    auto wheelSpeedProvider = new WheelSpeedProvider;
    auto voltageProvider = new VoltageProvider;
    auto ipcClient = new IpcClient;
    auto readerC = new Reader;

    QObject::connect(
        readerC,
        &Reader::speedReceived,
        speedProvider,
        &SpeedProvider::setSpeed);

    engine->rootContext()->setContextProperty(
        "speedProvider",
        speedProvider);

    QObject::connect(
        readerC,
        &Reader::temperatureReceived,
        temperatureProvider,
        &TemperatureProvider::setTemperature);

    engine->rootContext()->setContextProperty(
        "temperatureProvider",
        temperatureProvider);

    QObject::connect(
        readerC,
        &Reader::frontDistanceReceived,
        frontDistanceProvider,
        &FrontDistanceProvider::setFrontDistance);

    engine->rootContext()->setContextProperty(
        "frontDistanceProvider",
        frontDistanceProvider);

    QObject::connect(
        readerC,
        &Reader::voltageReceived,
        voltageProvider,
        &VoltageProvider::setVoltage);

    engine->rootContext()->setContextProperty(
        "voltageProvider",
        voltageProvider);

    // QObject::connect(
    //     readerC,
    //     &Reader::distanceReceived,
    //     distanceProvider,
    //     &DistanceProvider::setDistance);

    // engine->rootContext()->setContextProperty(
    //     "distanceProvider",
    //     distanceProvider);

    QObject::connect(
        readerC,
        &Reader::wheelSpeedReceived,
        wheelSpeedProvider,
        &WheelSpeedProvider::setWheelSpeed);

    engine->rootContext()->setContextProperty(
        "wheelSpeedProvider",
        wheelSpeedProvider);
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    engine.addImportPath("qrc:/qml");
    qmlRegisterSingletonType(QUrl("qrc:/qml/themes/BaseTheme.qml"), "ClusterTheme", 1, 0, "BaseTheme");

    qmlRegisterType<TimeProvider>("Cluster.Backend", 1, 0, "TimeProvider");
    qmlRegisterType<SystemStatus>("Cluster.Backend", 1, 0, "SystemStatus");

    ipcConnections(&engine);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl)
                     {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    engine.load(url);

    qDebug() << "Qt version:" << QT_VERSION_STR;
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
