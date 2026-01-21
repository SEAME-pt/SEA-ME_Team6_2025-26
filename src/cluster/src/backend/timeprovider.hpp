#include <QObject>
#include <QTimer>
#include <QDateTime>

class TimeProvider: public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currTime READ currTime NOTIFY timeChanged)
    Q_PROPERTY(QString currDate READ currDate NOTIFY dateChanged)

    public:
        explicit TimeProvider(QObject *parent = nullptr);

        QString currTime() const;
        QString currDate() const;

    signals:
        void timeChanged();
        void dateChanged();

    public slots:
        void updateTime();

    private:
        QString _currTime;
        QString _currDate;
        QTimer  _timer;
};
