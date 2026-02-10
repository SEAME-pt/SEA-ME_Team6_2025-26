#include <QObject>
#include <QTimer>
#include <QFile>
#include <QTextStream>

class SystemStatus: public QObject {
    Q_OBJECT
    Q_PROPERTY(int currBatteryLvl READ currBatteryLvl NOTIFY batteryChanged)
    Q_PROPERTY(QString batteryIconSource READ batteryIconSource NOTIFY batteryChanged)

    public:
        explicit SystemStatus(QObject *parent = nullptr);

        int currBatteryLvl() const;
        QString batteryIconSource() const;

    signals:
        void batteryChanged();

    private slots:
        void updateBatteryLevel();

    private:
        int _currBatteryLvl;
        QTimer _timer;

        int readBatteryLevelFromSystem() const;
};
