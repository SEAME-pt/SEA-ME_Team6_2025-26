#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <grpcpp/grpcpp.h>

#include "kuksa/val/v2/val.grpc.pb.h"
#include "kuksa/val/v2/types.pb.h"

#include "../../can_frames.h"

using kuksa::val::v2::VAL;

// Create a stub object that binds to the channel
static std::unique_ptr<VAL::Stub> create_val_stub(const std::string& host_port)
{
    auto channel = grpc::CreateChannel(host_port, grpc::InsecureChannelCredentials());
    return VAL::NewStub(channel);
}

static std::string value_to_string(const kuksa::val::v2::Value& v)
{
    // The generated 'Value' has oneof typed_value.
    // Switch to the correct option to 
    switch (v.typed_value_case()) {
        case kuksa::val::v2::Value::kDouble:
            return std::to_string(v.double_()); // note: double_() not "double"
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

// Poll one path using GetValue and print it.
// IMPORTANT: the proto must have GetValue(GetValueRequest)->GetValueResponse.
static bool get_and_print(VAL::Stub* stub, const std::string& path)
{
    grpc::ClientContext ctx;

    kuksa::val::v2::GetValueRequest req;
    kuksa::val::v2::GetValueResponse resp;

    // PublishValueRequest used signal_id + data_point.
    // GetValueRequest also uses signal_id.
    req.mutable_signal_id()->set_path(path);

    grpc::Status st = stub->GetValue(&ctx, req, &resp);
    if (!st.ok()) {
        std::cerr << "[KUKSA] GetValue(" << path << ") failed: " << st.error_message() << "\n";
        return false;
    }

    // Response contains data_point
    if (!resp.has_data_point()) {
        std::cout << "[KUKSA] " << path << " = <no datapoint>\n";
        return true;
    }

    // Confirm if is has a valid value
    const auto& dp = resp.data_point();
    if (!dp.has_value()) {
        std::cout << "[KUKSA] " << path << " = <no value>\n";
        return true;
    }

    std::cout << "[KUKSA] " << path << " = " << value_to_string(dp.value()) << "\n";
    return true;
}

int main(int argc, char** argv)
{
    const std::string server = (argc >= 2) ? argv[1] : "127.0.0.1:55555";

    auto stub = create_val_stub(server);
    std::cout << "[KUKSA] Reader connected to " << server << "\n";

    // Paths to read (the ones loaded via --vss)
    std::vector<std::string> paths;
    paths.push_back("Vehicle.Speed");
    paths.push_back("Vehicle.Cabin.AirTemperature");
    paths.push_back("Vehicle.Test.Heartbeat");

    while (true) {
        for (size_t i = 0; i < paths.size(); ++i) {
            get_and_print(stub.get(), paths[i]);
        }
        std::cout << "----\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
