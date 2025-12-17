/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @create date 2025-12-16 14:43:06
 * @modify date 2025-12-16 15:39:56
 * @desc Class that contains the ICP (Internet Computer Protocol) functions and connection to CAN logic
 */

#include <QObject>
#include <QTcpSocket>
#include <QStringList>

class IpcClient : public QObject
{
    Q_OBJECT

public:
    explicit IpcClient(QObject *parent = nullptr);

signals:
    void speedReceived(double speed);
    void temperatureReceived(double temperature);

private slots:
    void onConnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket _socket;
};
