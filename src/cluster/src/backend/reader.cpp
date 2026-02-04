#include "reader.hpp"

static std::string read_file(const std::string& path)
{
    std::ifstream f(path.c_str(), std::ios::in | std::ios::binary);
    if (!f.is_open())
        throw std::runtime_error("Failed to open file: " + path);

    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

static std::unique_ptr<VAL::Stub> create_val_stub(const std::string& host_port)
{
    grpc::SslCredentialsOptions ssl_opts;
    ssl_opts.pem_root_certs = read_file("/etc/kuksa/tls/ca.crt");

    auto channel = grpc::CreateChannel(host_port, grpc::SslCredentials(ssl_opts));
    return VAL::NewStub(channel);
}

ReaderWorker::ReaderWorker(const std::string &server)
    : QObject(nullptr), _server(server), _shouldStop(false)
{
    qDebug() << "[ReaderWorker] Created for server:" << QString::fromStdString(server);
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

        // Setup subscription request
        std::vector<std::string> paths;
        paths.push_back("Vehicle.Speed");
        paths.push_back("Vehicle.Exterior.AirTemperature");
        paths.push_back("Vehicle.ECU.SafetyCritical.Heartbeat");
        paths.push_back("Vehicle.ADAS.ObstacleDetection.Front.Distance");
        paths.push_back("Vehicle.Powertrain.TractionBattery.CurrentVoltage");
        //paths.push_back("Vehicle.TraveledDistance");
        paths.push_back("Vehicle.Powertrain.ElectricMotor.Speed");
        paths.push_back("Vehicle.CurrentLocation.Heading");
        paths.push_back("Vehicle.Powertrain.TractionBattery.IsCritical");
        paths.push_back("Vehicle.Powertrain.TractionBattery.IsLevelLow");

        kuksa::val::v2::SubscribeRequest req;
        for (size_t i = 0; i < paths.size(); ++i)
            req.add_signal_paths(paths[i]);

        // Create context and stream
        std::string jwt = read_file("/etc/kuksa/jwt/reader.jwt");

        grpc::ClientContext ctx;
        ctx.AddMetadata("authorization", "Bearer " + jwt);
        kuksa::val::v2::SubscribeResponse resp;

        std::unique_ptr<grpc::ClientReader<kuksa::val::v2::SubscribeResponse>> stream(
            stub->Subscribe(&ctx, req));

        // Read messages in loop (this runs in background thread)
        while (!_shouldStop && stream->Read(&resp))
        {
            // Process the response
            const ::google::protobuf::Map<std::string, kuksa::val::v2::Datapoint> &entries = resp.entries();

            for (auto it = entries.begin(); it != entries.end(); ++it)
            {
                const std::string &path = it->first;
                const kuksa::val::v2::Datapoint &dp = it->second;
                
                if (!dp.has_value())
                {
                    qDebug() << "[ReaderWorker]" << QString::fromStdString(path) << "= <no value>";
                    continue;
                }
                
                // Emit signals to main thread
                if (path == "Vehicle.Speed")
                {
                    double speed = dp.value().double_();
                    qDebug() << "[ReaderWorker] Speed:" << speed << "  | " 
                            << QString::fromStdString(value_to_string(dp.value()));
                    emit speedReceived(speed);
                }
                else if (path == "Vehicle.Exterior.AirTemperature")
                {
                    double temp = dp.value().double_();
                    qDebug() << "[ReaderWorker] Temperature:" << temp << "  | " 
                            << QString::fromStdString(value_to_string(dp.value()));
                    emit temperatureReceived(temp);
                } 
                else if (path == "Vehicle.ADAS.ObstacleDetection.Front.Distance") 
                {
                    double frontDistance = dp.value().float_();
                    qDebug() << "[ReaderWorker] Front Distance:" << frontDistance << "  | " 
                            << QString::fromStdString(value_to_string(dp.value()));
                    emit frontDistanceReceived(frontDistance);
                } else if (path == "Vehicle.Powertrain.TractionBattery.CurrentVoltage") {
                    double voltage = dp.value().float_();
                    qDebug() << "[ReaderWorker] Voltage:" << voltage << "  | " 
                            << QString::fromStdString(value_to_string(dp.value()));
                    emit voltageReceived(voltage);
                }
                else if (path == "Vehicle.Powertrain.ElectricMotor.Speed") 
                {
                    double wheelSpeed = static_cast<double>(dp.value().int32());
                    qDebug() << "[ReaderWorker] Wheel Speed:" << wheelSpeed << "  | " 
                            << QString::fromStdString(value_to_string(dp.value()));
                    emit wheelSpeedReceived(wheelSpeed);
                } else if (path == "Vehicle.CurrentLocation.Heading") {
                    double heading = dp.value().double_();
                    qDebug() << "[ReaderWorker] Heading:" << heading << "  | " 
                            << QString::fromStdString(value_to_string(dp.value()));
                    emit headingReceived(heading);
                } else if (path == "Vehicle.Powertrain.TractionBattery.IsLevelLow") {
                    bool status = dp.value().bool_();
                    qDebug() << "[ReaderWorker] IsLevelLow:" << status << "  | " 
                            << QString::fromStdString(value_to_string(dp.value()));
                    emit voltageLevelReceived(status, "low");
                } else if (path == "Vehicle.Powertrain.TractionBattery.IsCritical") {
                    bool status = dp.value().bool_();
                    qDebug() << "[ReaderWorker] IsCritical:" << status << "  | " 
                            << QString::fromStdString(value_to_string(dp.value()));
                    emit voltageLevelReceived(status, "critical");
                }
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

std::string ReaderWorker::value_to_string(const kuksa::val::v2::Value &v)
{
    switch (v.typed_value_case())
    {
    case kuksa::val::v2::Value::kDouble:
        return std::to_string(v.double_());
    case kuksa::val::v2::Value::kFloat:
        return std::to_string(v.float_());
    case kuksa::val::v2::Value::kInt32:
        return std::to_string(v.int32());
    case kuksa::val::v2::Value::kInt64:
        return std::to_string((long long)v.int64());
    case kuksa::val::v2::Value::kUint32:
        return std::to_string(v.uint32());
    case kuksa::val::v2::Value::kUint64:
        return std::to_string((unsigned long long)v.uint64());
    case kuksa::val::v2::Value::kBool:
        return v.bool_() ? "true" : "false";
    case kuksa::val::v2::Value::kString:
        return v.string();
    default:
        return "<unset>";
    }
}

// ============================================================================
// Reader Implementation (runs in main thread)
// ============================================================================

Reader::Reader(QObject *parent) : QObject(parent)
{
    qDebug() << "[Reader] Constructor called - setting up background thread";

    // Create worker thread
    _workerThread = new QThread(this);
    _worker = new ReaderWorker(_server);
    
    // Move worker to thread
    _worker->moveToThread(_workerThread);

    // Connect signals from worker to this object (forwarding to main thread)
    connect(_worker, &ReaderWorker::speedReceived, this, &Reader::speedReceived);
    connect(_worker, &ReaderWorker::temperatureReceived, this, &Reader::temperatureReceived);
    connect(_worker, &ReaderWorker::distanceReceived, this, &Reader::distanceReceived);
    connect(_worker, &ReaderWorker::frontDistanceReceived, this, &Reader::frontDistanceReceived);
    connect(_worker, &ReaderWorker::wheelSpeedReceived, this, &Reader::wheelSpeedReceived);
    connect(_worker, &ReaderWorker::voltageReceived, this, &Reader::voltageReceived);
    connect(_worker, &ReaderWorker::voltageLevelReceived, this, &Reader::voltageLevelReceived);
    connect(_worker, &ReaderWorker::headingReceived, this, &Reader::headingReceived);


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