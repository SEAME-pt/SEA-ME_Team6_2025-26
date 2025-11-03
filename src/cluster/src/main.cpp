#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QDebug>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:qml/main.qml"));
    auto *window = qobject_cast<QQuickWindow*>(engine.rootObjects().first());
    if (window) {
        window->setHeight(400);
        window->setWidth(1028);
    }

    return app.exec();
}
