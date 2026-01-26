#include "reader.hpp"

Reader::Reader(QObject *parent): QObject(parent) {
    qDebug() << "[Reader] Constructor called";

    auto stub = create_val_stub(this._server);
    qDebug() << "[Reader] Connected to " << this._server << "\n";

    std::vector<std::string> paths;
    paths.push_back("Vehicle.Speed");
    paths.push_back("Vehicle.Exterior.AirTemperature");
    paths.push_back("Vehicle.ECU.SafetyCritical.Heartbeat");

    kuksa::val::v2::SubscribeRequest req;
    for (size_t i = 0; i < paths.size(); ++i) {
        req.add_signal_paths(paths[i]);
    }

    grpc::ClientContext ctx;

    std::unique_ptr<grpc::ClientReader<kuksa::val::v2::SubscribeResponse> > stream(
        stub->Subscribe(&ctx, req)
    );

    while (stream->Read(&resp)) {
        // Our proto: map<string, Datapoint> entries
        const ::google::protobuf::Map<std::string, kuksa::val::v2::Datapoint>& entries = resp.entries();

        for (::google::protobuf::Map<std::string, kuksa::val::v2::Datapoint>::const_iterator it = entries.begin();
             it != entries.end(); ++it)
        {
            const std::string& path = it->first;
            const kuksa::val::v2::Datapoint& dp = it->second;
            if (!dp.has_value()) {
                qDebug() << "[READER] " << path << " = <no value>\n";
                return;
            }
            if (path == 'Vehicle.Speed')
                emit speedReceived(dp.value())
            else if (path == 'Vehicle.Exterior.AirTemperature')
                emit temperatureReceived(dp.value())
        }

        qDebug() << "----\n";
    }

    grpc::Status st = stream->Finish();
    if (!st.ok()) {
        qDebug() << "[KUKSA] Subscribe stream ended with error: " << st.error_message() << "\n";
        return 1;
    }

    qDebug() << "[KUKSA] Subscribe stream ended cleanly.\n";

    kuksa::val::v2::SubscribeResponse resp;
}

// Create a stub object that binds to the channel
// the stub is like the wrapper that allows to call remote methods
static std::unique_ptr<VAL::Stub> Reader::create_val_stub(const std::string& host_port)
{
    auto channel = grpc::CreateChannel(host_port, grpc::InsecureChannelCredentials());
    return VAL::NewStub(channel);
}

// Helper to convert Value to string for printing
static std::string Reader::value_to_string(const kuksa::val::v2::Value& v)
{
    switch (v.typed_value_case()) {
        case kuksa::val::v2::Value::kDouble:  return std::to_string(v.double_());
        case kuksa::val::v2::Value::kFloat:   return std::to_string(v.float_());
        case kuksa::val::v2::Value::kInt32:   return std::to_string(v.int32());
        case kuksa::val::v2::Value::kInt64:   return std::to_string((long long)v.int64());
        case kuksa::val::v2::Value::kUint32:  return std::to_string(v.uint32());
        case kuksa::val::v2::Value::kUint64:  return std::to_string((unsigned long long)v.uint64());
        case kuksa::val::v2::Value::kBool:    return v.bool_() ? "true" : "false";
        case kuksa::val::v2::Value::kString:  return v.string();
        default:                              return "<unset>";
    }
}