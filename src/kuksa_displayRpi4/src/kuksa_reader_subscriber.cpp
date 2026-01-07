#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <grpcpp/grpcpp.h>
#include "kuksa/val/v2/val.grpc.pb.h"
#include "kuksa/val/v2/types.pb.h"

using kuksa::val::v2::VAL;

// Create a stub object that binds to the channel
// the stub is like the wrapper that allows to call remote methods
static std::unique_ptr<VAL::Stub> create_val_stub(const std::string& host_port)
{
    auto channel = grpc::CreateChannel(host_port, grpc::InsecureChannelCredentials());
    return VAL::NewStub(channel);
}

// Helper to convert Value to string for printing
static std::string value_to_string(const kuksa::val::v2::Value& v)
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

static void print_datapoint(const std::string& path, const kuksa::val::v2::Datapoint& dp)
{
    if (!dp.has_value()) {
        std::cout << "[KUKSA] " << path << " = <no value>\n";
        return;
    }
    std::cout << "[KUKSA] " << path << " = " << value_to_string(dp.value()) << "\n";
}

int main(int argc, char** argv)
{
    const std::string server = (argc >= 2) ? argv[1] : "127.0.0.1:55555";
    auto stub = create_val_stub(server);

    std::cout << "[KUKSA] Reader connected to " << server << "\n";

    // Paths to subscribe (the ones loaded via --vss)
    std::vector<std::string> paths;
    paths.push_back("Vehicle.Speed");
    paths.push_back("Vehicle.Exterior.AirTemperature");
    paths.push_back("Vehicle.SafetyCritical.Heartbeat");

    // Build subscription request
    // Entries are the paths to subscribe to
    kuksa::val::v2::SubscribeRequest req;
    for (size_t i = 0; i < paths.size(); ++i) {
        req.add_signal_paths(paths[i]);
    }

    grpc::ClientContext ctx;

    // Start server->client streaming subscription
    std::unique_ptr<grpc::ClientReader<kuksa::val::v2::SubscribeResponse> > stream(
        stub->Subscribe(&ctx, req)
    );

    kuksa::val::v2::SubscribeResponse resp;

    // Read updates forever (until server closes stream or connection drops)
    // Read() is going to fill 'resp' with each incoming message
    // which may contain multiple updates
    while (stream->Read(&resp)) {
        // Our proto: map<string, Datapoint> entries
        const ::google::protobuf::Map<std::string, kuksa::val::v2::Datapoint>& entries = resp.entries();

        for (::google::protobuf::Map<std::string, kuksa::val::v2::Datapoint>::const_iterator it = entries.begin();
             it != entries.end(); ++it)
        {
            const std::string& path = it->first;
            const kuksa::val::v2::Datapoint& dp = it->second;
            print_datapoint(path, dp);
        }

        std::cout << "----\n";
    }

    grpc::Status st = stream->Finish();
    if (!st.ok()) {
        std::cerr << "[KUKSA] Subscribe stream ended with error: " << st.error_message() << "\n";
        return 1;
    }

    std::cout << "[KUKSA] Subscribe stream ended cleanly.\n";
    return 0;
}