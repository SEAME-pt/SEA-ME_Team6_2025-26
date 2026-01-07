# How the KUKSA reader/subscriber works

1. First of all you need to have the `generated`folder in your folder structure
	1. You can find it in the branch `feature/kuksa/implementation`
	2. Include the header files:
	   ```
	   #include "kuksa/val/v2/val.grpc.pb.h"
		#include "kuksa/val/v2/types.pb.h"
	   ```
	   
2. Create stub object that binds to the channel
```static std::unique_ptr<VAL::Stub> create_val_stub(const std::string& host_port)

{

auto channel = grpc::CreateChannel(host_port, grpc::InsecureChannelCredentials());

return VAL::NewStub(channel);

}
// in this case 'host_port' is the RPi5 ip that is running the KUKSA
```

2. Helper function - value_to_string - to convert to string for Qt
```
static std::string value_to_string(const kuksa::val::v2::Value& v)

{

switch (v.typed_value_case()) {

case kuksa::val::v2::Value::kDouble: return std::to_string(v.double_());

case kuksa::val::v2::Value::kFloat: return std::to_string(v.float_());

case kuksa::val::v2::Value::kInt32: return std::to_string(v.int32());

case kuksa::val::v2::Value::kInt64: return std::to_string((long long)v.int64());

case kuksa::val::v2::Value::kUint32: return std::to_string(v.uint32());

case kuksa::val::v2::Value::kUint64: return std::to_string((unsigned long long)v.uint64());

case kuksa::val::v2::Value::kBool: return v.bool_() ? "true" : "false";

case kuksa::val::v2::Value::kString: return v.string();

default: return "<unset>";

}

}
```

3. In main (or the Qt app) we need to:
	1. Create the stub to call KUKSA methods
	   `auto stub = create_val_stub(server);`
	2. Select the paths we want to subscribe
```
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
```

3. And now, the real magic:
**The loop that is going to read from KUKSA continuously**

```
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
