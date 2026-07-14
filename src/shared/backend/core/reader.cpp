/**
 * @file reader.cpp
 * @brief Implementation of generic Kuksa signal reader
 */

#include "reader.hpp"

// ============================================================================
// Helper Functions
// ============================================================================

std::string ReaderWorker::read_file(const std::string& path)
{
    std::ifstream f(path.c_str(), std::ios::in | std::ios::binary);
    if (!f.is_open())
        throw std::runtime_error("Failed to open file: " + path);

    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

std::unique_ptr<VAL::Stub> ReaderWorker::create_val_stub(const std::string& host_port)
{
    grpc::SslCredentialsOptions ssl_opts;
    ssl_opts.pem_root_certs = read_file("/etc/kuksa/tls/ca.crt");

    auto channel = grpc::CreateChannel(host_port, grpc::SslCredentials(ssl_opts));
    return VAL::NewStub(channel);
}

QVariant ReaderWorker::datapoint_to_variant(const kuksa::val::v2::Datapoint &dp)
{
    if (!dp.has_value())
        return QVariant();

    const kuksa::val::v2::Value &v = dp.value();

    switch (v.typed_value_case())
    {
        case kuksa::val::v2::Value::kDouble:
            return QVariant(v.double_());
        case kuksa::val::v2::Value::kFloat:
            return QVariant(static_cast<double>(v.float_()));
        case kuksa::val::v2::Value::kInt32:
            return QVariant(v.int32());
        case kuksa::val::v2::Value::kInt64:
            return QVariant(static_cast<qint64>(v.int64()));
        case kuksa::val::v2::Value::kUint32:
            return QVariant(v.uint32());
        case kuksa::val::v2::Value::kUint64:
            return QVariant(static_cast<quint64>(v.uint64()));
        case kuksa::val::v2::Value::kBool:
            return QVariant(v.bool_());
        case kuksa::val::v2::Value::kString:
            return QVariant(QString::fromStdString(v.string()));
        default:
            return QVariant();
    }
}

// ============================================================================
// ReaderWorker Implementation (Background Thread)
// ============================================================================

ReaderWorker::ReaderWorker(const std::string &server, const std::vector<std::string> &signalPaths)
    : QObject(nullptr), _server(server), _signalPaths(signalPaths),
      _shouldStop(false), _activeContext(nullptr)
{
    qDebug() << "[ReaderWorker] Created for server:" << QString::fromStdString(server);
    qDebug() << "[ReaderWorker] Subscribing to" << signalPaths.size() << "signals";
}

ReaderWorker::~ReaderWorker()
{
    _shouldStop = true;
    qDebug() << "[ReaderWorker] Destroyed!";
}

void ReaderWorker::startReading()
{
    qDebug() << "[ReaderWorker] Starting to read from Kuksa...";

    try {
        auto stub = create_val_stub(_server);
        qDebug() << "[ReaderWorker] Connected to" << QString::fromStdString(_server);
        emit connected();

        kuksa::val::v2::SubscribeRequest req;
        for (const auto &path : _signalPaths) {
            req.add_signal_paths(path);
            qDebug() << "[ReaderWorker]   Subscribed to:" << QString::fromStdString(path);
        }

        std::string jwt = read_file("/etc/kuksa/jwt/publisher.jwt");

        auto ctx = std::make_unique<grpc::ClientContext>();
        ctx->AddMetadata("authorization", "Bearer " + jwt);
        _activeContext.store(ctx.get());
        _contextReady.store(true);

        kuksa::val::v2::SubscribeResponse resp;
        std::unique_ptr<grpc::ClientReader<kuksa::val::v2::SubscribeResponse>> stream(
            stub->Subscribe(ctx.get(), req));

        qDebug() << "[ReaderWorker] About to enter Read() loop...";

        while (!_shouldStop && stream->Read(&resp))
        {
            if (_shouldStop) break;

            const auto &entries = resp.entries();

            // Only write to the cache - never emit signals directly.
            // The dispatch timer on the main thread will read this cache
            // at a fixed rate, so the Qt event queue never grows.
            {
                QMutexLocker locker(&_cacheMutex);
                for (auto it = entries.begin(); it != entries.end(); ++it)
                {
                    QVariant value = datapoint_to_variant(it->second);
                    if (!value.isValid()) continue;

                    QString path = QString::fromStdString(it->first);
                    _latestValues[path] = value;
                    _pendingEmit[path]  = true;
                }
            }
        }

        qDebug() << "[ReaderWorker] Exited Read() loop!";
        _contextReady.store(false);
        _activeContext.store(nullptr);

        grpc::Status st = stream->Finish();
        if (!st.ok()) {
            QString errorMsg = QString::fromStdString(st.error_message());
            qDebug() << "[ReaderWorker] Stream ended with error:" << errorMsg;
            emit connectionError(errorMsg);
        } else {
            qDebug() << "[ReaderWorker] Stream ended cleanly";
        }

    } catch (const std::exception &e) {
        QString errorMsg = QString("Exception: %1").arg(e.what());
        qDebug() << "[ReaderWorker]" << errorMsg;
        emit connectionError(errorMsg);
    }
}

void ReaderWorker::stopReading()
{
    qDebug() << "[ReaderWorker] Stop requested";
    _shouldStop = true;

    if (_contextReady.load()) {
        grpc::ClientContext* ctx = _activeContext.load();
        if (ctx)
            ctx->TryCancel();
    }
}

// ============================================================================
// Reader Implementation (Main Thread)
// ============================================================================

Reader::Reader(const std::vector<std::string> &signalPaths, SignalRouter *router, QObject *parent)
    : QObject(parent), _router(router)
{
    qDebug() << "[Reader] Constructor called - setting up background thread";

    _workerThread = new QThread(this);
    _worker = new ReaderWorker(_server, signalPaths);
    _worker->moveToThread(_workerThread);

    // Forward connection-level signals normally (low frequency, fine with QueuedConnection)
    connect(_worker, &ReaderWorker::connectionError,
            this, &Reader::connectionError, Qt::QueuedConnection);
    connect(_worker, &ReaderWorker::connected,
            this, &Reader::connected, Qt::QueuedConnection);

    connect(_workerThread, &QThread::started,
            _worker, &ReaderWorker::startReading);
    connect(_workerThread, &QThread::finished,
            _worker, &QObject::deleteLater);

    _dispatchTimer = new QTimer(this);
    _dispatchTimer->setInterval(50);
    _dispatchTimer->setTimerType(Qt::CoarseTimer);
    connect(_dispatchTimer, &QTimer::timeout, this, &Reader::dispatchPendingSignals);
    _dispatchTimer->start();

    _workerThread->start();
    qDebug() << "[Reader] Background thread started";
}

Reader::~Reader()
{
    qDebug() << "[Reader] Destructor called - stopping background thread";

    _dispatchTimer->stop();
    _worker->stopReading();
    _workerThread->quit();

    if (!_workerThread->wait(3000)) {
        qWarning() << "[Reader] Thread didn't stop after 3s, forcing";
        _workerThread->terminate();
        if (!_workerThread->wait(2000)) {
            qCritical() << "[Reader] Thread could not be terminated!";
        }
    }

    qDebug() << "[Reader] Background thread stopped";
}

void Reader::dispatchPendingSignals()
{
    // Runs on the main thread at 50ms intervals.
    // Grabs only the latest value per signal path and routes it.
    // No matter how fast the worker writes, we only process 9 signals max per tick.
    if (!_router) return;

    QHash<QString, QVariant> snapshot;

    {
        QMutexLocker locker(&_worker->_cacheMutex);
        for (auto it = _worker->_pendingEmit.begin(); it != _worker->_pendingEmit.end(); ++it) {
            if (it.value()) {
                snapshot[it.key()] = _worker->_latestValues[it.key()];
                it.value() = false;
            }
        }
    }

    for (auto it = snapshot.begin(); it != snapshot.end(); ++it) {
        _router->routeSignal(it.key(), it.value());
    }
}