#include "kuksa_client.hpp"

#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <string>

#include "../generated/kuksa/val/v2/val.grpc.pb.h"
#include "../generated/kuksa/val/v2/types.pb.h"

using kuksa::val::v2::VAL;

struct KuksaClient::Impl {
    std::shared_ptr<grpc::Channel> channel;
    std::unique_ptr<VAL::Stub> stub;

    explicit Impl(const std::string& addr)
    {
        channel = grpc::CreateChannel(addr, grpc::InsecureChannelCredentials());
        stub = VAL::NewStub(channel);
    }
};

KuksaClient::KuksaClient(const std::string& addr)
: impl_(new Impl(addr))
{
}

KuksaClient::~KuksaClient() = default;

void KuksaClient::publishDouble(const std::string& path, double value)
{
    grpc::ClientContext ctx;
    kuksa::val::v2::PublishValueRequest req;
    kuksa::val::v2::PublishValueResponse resp;

    req.mutable_signal_id()->set_path(path);
    req.mutable_data_point()->mutable_value()->set_double_(value);

    grpc::Status st = impl_->stub->PublishValue(&ctx, req, &resp);
    if (!st.ok()) {
        std::cerr << "[KUKSA] PublishValue(" << path
                  << ") failed: " << st.error_message() << "\n";
    }
}

void KuksaClient::publishInt32(const std::string& path, std::int32_t value)
{
    grpc::ClientContext ctx;
    kuksa::val::v2::PublishValueRequest req;
    kuksa::val::v2::PublishValueResponse resp;

    req.mutable_signal_id()->set_path(path);
    req.mutable_data_point()->mutable_value()->set_int32(value);

    grpc::Status st = impl_->stub->PublishValue(&ctx, req, &resp);
    if (!st.ok()) {
        std::cerr << "[KUKSA] PublishValue(" << path
                  << ") failed: " << st.error_message() << "\n";
    }
}
