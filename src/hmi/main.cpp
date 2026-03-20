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
#include <csignal>
#include <execinfo.h>
#include <cstdio>

#include "core/reader.hpp"
#include "core/signalrouter.hpp"
#include "providers/powertrainprovider.hpp"
#include "providers/vehicleprovider.hpp"
#include "providers/exteriorprovider.hpp"
#include "providers/adasprovider.hpp"
#include "providers/currentlocationprovider.hpp"
#include "providers/chassisprovider.hpp"
#include "providers/extraprovider.hpp"
#include "providers/cameraprovider.hpp"

static void crashHandler(int sig) {
    void *array[20];
    int size = backtrace(array, 20);
    fprintf(stderr, "\n[CRASH] Signal %d received:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

int main(int argc, char *argv[])
{
    //qputenv("QSG_RENDER_LOOP", "basic");
    qputenv("QML_DISABLE_DISTANCEFIELD", "1");
    signal(SIGSEGV, crashHandler);
    signal(SIGABRT, crashHandler);
    signal(SIGFPE,  crashHandler);
    QGuiApplication app(argc, argv);

    std::vector<std::string> kuksaSignals = {
        "Vehicle.Speed",
        "Vehicle.Exterior.AirTemperature",
        "Vehicle.ADAS.ObstacleDetection.Front.Distance",
        "Vehicle.Powertrain.TractionBattery.CurrentVoltage",
        "Vehicle.Powertrain.ElectricMotor.Speed",
        "Vehicle.Powertrain.TractionBattery.IsCritical",
        "Vehicle.Powertrain.TractionBattery.IsLevelLow",
        "Vehicle.CurrentLocation.Heading",
        "Vehicle.Chassis.SteeringWheel.Angle"
    };

    qDebug() << "[Main] Initializing application with" << kuksaSignals.size() << "Kuksa signals";

    PowertrainProvider *powertrain = new PowertrainProvider(&app);
    VehicleProvider *vehicle = new VehicleProvider(&app);
    ExteriorProvider *exterior = new ExteriorProvider(&app);
    ADASProvider *adas = new ADASProvider(&app);
    CurrentLocationProvider *currentLocation = new CurrentLocationProvider(&app);
    ChassisProvider *chassis = new ChassisProvider(&app);
    CameraProvider *camera = new CameraProvider(&app);

    qDebug() << "[Main] Created all providers";

    SignalRouter *router = new SignalRouter(&app);
    router->registerPowertrainProvider(powertrain);
    router->registerVehicleProvider(vehicle);
    router->registerExteriorProvider(exterior);
    router->registerADASProvider(adas);
    router->registerCurrentLocationProvider(currentLocation);
    router->registerChassisProvider(chassis);

    qDebug() << "[Main] SignalRouter configured with all providers";

    Reader *reader = new Reader(kuksaSignals, router, &app);

    QObject::connect(reader, &Reader::connectionError,
                     &app, [](QString error) {
        qCritical() << "[Main] Kuksa Reader error:" << error;
    });

    QObject::connect(reader, &Reader::connected,
                     &app, []() {
        qDebug() << "[Main] Successfully connected to Kuksa!";
    });

    qDebug() << "[Main] Reader connected to SignalRouter";

    QQmlApplicationEngine engine;

    engine.addImportPath("qrc:/qml");
    qmlRegisterSingletonType(QUrl("qrc:/qml/themes/BaseTheme.qml"), 
                             "ClusterTheme", 1, 0, "BaseTheme");

    qmlRegisterType<ExtraProvider>("Cluster.Backend", 1, 0, "ExtraProvider");

    engine.rootContext()->setContextProperty("powertrain", powertrain);
    engine.rootContext()->setContextProperty("exterior", exterior);
    engine.rootContext()->setContextProperty("vehicle", vehicle);
    engine.rootContext()->setContextProperty("adas", adas);
    engine.rootContext()->setContextProperty("currentLocation", currentLocation);
    engine.rootContext()->setContextProperty("chassis", chassis);
    engine.rootContext()->setContextProperty("camera", camera);

    qDebug() << "[Main] All providers exposed to QML";

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, 
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    QObject::connect(&engine, &QQmlApplicationEngine::warnings,
                    &app, [](const QList<QQmlError> &warnings) {
        for (const QQmlError &w : warnings)
            qCritical() << "[QML ERROR]" << w.toString();
    });

    qDebug() << "[Main] About to load QML...";
    engine.load(url);
    qDebug() << "[Main] QML loaded!"; 

    qDebug() << "Qt version:" << QT_VERSION_STR;
    if (engine.rootObjects().isEmpty()) {
        qCritical() << "[Main] Failed to load QML!";
        return -1;
    }

    qDebug() << "[Main] Application started successfully!";
    return app.exec();
}
