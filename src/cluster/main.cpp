#include "headers/cluster.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    engine.load(QUrl("qrc:/main.qml"));
    auto *window = qobject_cast<QQuickWindow*>(engine.rootObjects().first());
    if (window) {
        window->setHeight(_windowHeight_);
        window->setWidth(_windowWidth_);
    }

    return app.exec();
}

