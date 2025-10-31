#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QLabel label("SEA:ME");
    label.setWindowTitle("Hello Qt6");
    label.setAlignment(Qt::AlignCenter);

    label.showFullScreen();

    return app.exec();
}
