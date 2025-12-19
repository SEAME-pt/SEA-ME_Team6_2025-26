/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @create date 2025-12-16 14:47:58
 * @modify date 2025-12-16 15:46:32
 * @desc IpcClient Class functions
 */

#include "ipcclient.hpp"
#include <QDebug>

/**
 * @brief: Constructs an IpcClient object.
 *
 * Initializes the socket, sets up connections for signals such as connect, disconnected, readyRead and errorOccurred, and attempts to connect to the daemon on 127.0.0.1:5555.
 *
 * @param parent: (QObject *): Pointer to the parent QOject.
 * @return: void
 * @signals: None
 */
IpcClient::IpcClient(QObject *parent)
    : QObject(parent)
{
    qDebug() << "[IpcClient] Constructor called";

    connect(&_socket, &QTcpSocket::connected,
            this, &IpcClient::onConnected);

    connect(&_socket, &QTcpSocket::disconnected, []()
            { qDebug() << "[IpcClient] Disconnected from daemon"; });

    connect(&_socket, &QTcpSocket::errorOccurred,
            this, &IpcClient::onSocketError);

    connect(&_socket, &QTcpSocket::readyRead,
            this, &IpcClient::onReadyRead);

    qDebug() << "[IpcClient] Connecting to daemon...";
    _socket.connectToHost("127.0.0.1", 5555);
}

/**
 * @brief: Handle socket errors.
 *
 * Logs the occurred socket error code to the debug output.
 *
 * @param socketError: (QAbstractSocket::SocketError): The socket error that occured.
 * @return: void
 * @signals: None
 */
void IpcClient::onSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "[IpcClient] Socket error:" << socketError;
}

/**
 * @brief: Slot called when the client connects to the daemon.
 *
 * Sends an initial "GET" request to the daemon to start receiving data.
 *
 * @param None:
 * @return: void
 * @signals: None
 */
void IpcClient::onConnected()
{
    qDebug() << "[IpcClient] Connected to daemon";
    _socket.write("GET\n");
}

/**
 * @brief: Sot called when the socket has data available to read.
 *
 * Reads all available data, parses it and emits the signal if a valid value is found.
 * Afterwards, it sends another "GET" request to continue polling data.
 *
 * @param None:
 * @return: void
 * @signals: speedReceived(double speed): Emitted when a valid speed value is extracted.
 */
void IpcClient::onReadyRead()
{
    QByteArray data = _socket.readAll();

    QString msg = QString::fromUtf8(data).trimmed();

    QStringList tokens = msg.split('|');

    if (tokens.size() < 3)
        return;

    bool ok = false;
    QString heartbeat = tokens[0].trimmed();
    // ---------------------
    double temp = tokens[1].trimmed().toDouble(&ok);
    if (ok) {
        emit temperatureReceived(temp);
        ok = false;
    }
    // ---------------------
    double speed = tokens[2].trimmed().toDouble(&ok);
    if (ok) {
        emit speedReceived(speed);
        ok = false;
    }
    _socket.write("GET\n");
}
