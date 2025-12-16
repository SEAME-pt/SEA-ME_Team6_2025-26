/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @create date 2025-12-16 15:40:02
 * @modify date 2025-12-16 15:40:24
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
#include "backend/ipcclient.hpp"

void ipcConnections(QQmlApplicationEngine *engine)
{
    auto speedProvider = new SpeedProvider;
    auto temperatureProvider = new TemperatureProvider;
    auto ipcClient = new IpcClient;

    QObject::connect(
        ipcClient,
        &IpcClient::speedReceived,
        speedProvider,
        &SpeedProvider::setSpeed);

    engine->rootContext()->setContextProperty(
        "speedProvider",
        speedProvider);

    QObject::connect(
        ipcClient,
        &IpcClient::temperatureReceived,
        temperatureProvider,
        &TemperatureProvider::setTemperature);

    engine->rootContext()->setContextProperty(
        "temperatureProvider",
        temperatureProvider);
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
