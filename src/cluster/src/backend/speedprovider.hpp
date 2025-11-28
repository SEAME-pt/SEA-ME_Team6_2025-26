#include <QObject>
#include <QTimer>

class SpeedProvider : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currSpeed READ currSpeed NOTIFY speedChanged)

public:
    explicit SpeedProvider(QObject *parent = nullptr);

    QString currSpeed() const;

signals:
    void speedChanged();

private slots:
    void updateSpeed();

private:
    QString _currSpeed;
    QTimer  _timer;
    double _speedValue = 0.0;
    double _increment  = 0.5;
};
