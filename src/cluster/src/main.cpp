#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QDebug>
#include "backend/timeprovider.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    engine.addImportPath("qrc:/qml");
    qmlRegisterSingletonType(QUrl("qrc:/qml/themes/BaseTheme.qml"), "ClusterTheme", 1, 0, "BaseTheme");
    qmlRegisterType<TimeProvider>("Cluster.Backend", 1, 0, "TimeProvider");
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
