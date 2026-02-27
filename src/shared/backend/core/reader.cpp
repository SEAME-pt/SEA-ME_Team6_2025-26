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

// ✨ Convert Kuksa datapoint to QVariant (type-agnostic)
QVariant ReaderWorker::datapoint_to_variant(const kuksa::val::v2::Datapoint &dp)
{
    if (!dp.has_value())
        return QVariant(); // Invalid/null value

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
            return QVariant(); // Unsupported type
    }
}

// ============================================================================
// ReaderWorker Implementation (Background Thread)
// ============================================================================

ReaderWorker::ReaderWorker(const std::string &server, const std::vector<std::string> &signalPaths)
    : QObject(nullptr), _server(server), _signalPaths(signalPaths), _shouldStop(false)
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
        // Create stub
        auto stub = create_val_stub(_server);
        qDebug() << "[ReaderWorker] Connected to" << QString::fromStdString(_server);
        emit connected();

        // Setup subscription request with provided signal paths
        kuksa::val::v2::SubscribeRequest req;
        for (const auto &path : _signalPaths) {
            req.add_signal_paths(path);
            qDebug() << "[ReaderWorker]   Subscribed to:" << QString::fromStdString(path);
        }

        // Create context and stream
        std::string jwt = read_file("/etc/kuksa/jwt/reader.jwt");

        grpc::ClientContext ctx;
        ctx.AddMetadata("authorization", "Bearer " + jwt);
        kuksa::val::v2::SubscribeResponse resp;

        std::unique_ptr<grpc::ClientReader<kuksa::val::v2::SubscribeResponse>> stream(
            stub->Subscribe(&ctx, req));

        while (!_shouldStop && stream->Read(&resp))
        {
            const ::google::protobuf::Map<std::string, kuksa::val::v2::Datapoint> &entries = resp.entries();

            for (auto it = entries.begin(); it != entries.end(); ++it)
            {
                const std::string &path = it->first;
                const kuksa::val::v2::Datapoint &dp = it->second;
                
                QVariant value = datapoint_to_variant(dp);
                
                if (!value.isValid()) {
                    qDebug() << "[ReaderWorker]" << QString::fromStdString(path) << "= <no value>";
                    continue;
                }
                qDebug() << "[ReaderWorker] Signal:" << QString::fromStdString(path) 
                         << "=" << value;
                emit signalReceived(QString::fromStdString(path), value);
            }
        }

        // Stream ended
        grpc::Status st = stream->Finish();
        if (!st.ok())
        {
            QString errorMsg = QString::fromStdString(st.error_message());
            qDebug() << "[ReaderWorker] Subscribe stream ended with error:" << errorMsg;
            emit connectionError(errorMsg);
        }
        else
        {
            qDebug() << "[ReaderWorker] Subscribe stream ended cleanly";
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
}

// ============================================================================
// Reader Implementation (Main Thread)
// ============================================================================

Reader::Reader(const std::vector<std::string> &signalPaths, SignalRouter *router, QObject *parent) 
    : QObject(parent)
{
    qDebug() << "[Reader] Constructor called - setting up background thread";

    // Create worker thread
    _workerThread = new QThread(this);
    _worker = new ReaderWorker(_server, signalPaths);
    
    // Move worker to thread
    _worker->moveToThread(_workerThread);

    // ✨ Simple forwarding - no specific signal knowledge!
    connect(_worker, &ReaderWorker::signalReceived, 
            this, &Reader::signalReceived);
    connect(_worker, &ReaderWorker::connectionError, 
            this, &Reader::connectionError);
    connect(_worker, &ReaderWorker::connected, 
            this, &Reader::connected);

    // Start reading when thread starts
    connect(_workerThread, &QThread::started, 
            _worker, &ReaderWorker::startReading);
    
    // Cleanup when thread finishes
    connect(_workerThread, &QThread::finished, 
            _worker, &QObject::deleteLater);

    if (router) {
        connect(this, &Reader::signalReceived,
                router, &SignalRouter::routeSignal);
    }

    // Start the thread
    _workerThread->start();
    
    qDebug() << "[Reader] Background thread started";
}

Reader::~Reader()
{
    qDebug() << "[Reader] Destructor called - stopping background thread";
    
    // Stop the worker
    _worker->stopReading();
    
    // Stop thread and wait
    _workerThread->quit();
    _workerThread->wait(5000); // Wait up to 5 seconds
    
    qDebug() << "[Reader] Background thread stopped";
}