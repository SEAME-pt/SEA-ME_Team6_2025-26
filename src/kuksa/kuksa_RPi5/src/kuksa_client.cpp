#include "kuksa_client.hpp"

#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "../generated/kuksa/val/v2/val.grpc.pb.h"
#include "../generated/kuksa/val/v2/types.pb.h"

using kuksa::val::v2::VAL;

static std::string read_file(const std::string& path)
{
    std::ifstream f(path.c_str(), std::ios::in | std::ios::binary);
    if (!f.is_open())
        throw std::runtime_error("Failed to open file: " + path);

    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

struct KuksaClient::Impl {
    std::shared_ptr<grpc::Channel> channel;
    std::unique_ptr<VAL::Stub> stub;
    std::string jwt;

    Impl(const std::string& addr, const std::string& ca_cert_path)
    {
        grpc::SslCredentialsOptions ssl_opts; 
        ssl_opts.pem_root_certs = read_file(ca_cert_path); // Load CA cert
        jwt = read_file("/etc/kuksa/jwt/publisher.jwt"); // Load JWT from file
        channel = grpc::CreateChannel(addr, grpc::SslCredentials(ssl_opts));
        stub = VAL::NewStub(channel);
    }
};

KuksaClient::KuksaClient(const std::string& addr)
: impl_(new Impl(addr, "/etc/kuksa/tls/ca.crt"))
{
}

KuksaClient::~KuksaClient() = default;

void KuksaClient::publishDouble(const std::string& path, double value)
{
    grpc::ClientContext ctx;
    add_auth(ctx, impl_->jwt); // Add JWT for authentication
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
    ctx.AddMetadata("authorization", std::string("Bearer ") + impl_->jwt); // Add JWT for authentication
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

void KuksaClient::publishBool(const std::string& path, bool value)
{
    grpc::ClientContext ctx;
    ctx.AddMetadata("authorization", std::string("Bearer ") + impl_->jwt); // Add JWT for authentication
    kuksa::val::v2::PublishValueRequest req;
    kuksa::val::v2::PublishValueResponse resp;

    req.mutable_signal_id()->set_path(path);
    req.mutable_data_point()->mutable_value()->set_bool_(value);

    grpc::Status st = impl_->stub->PublishValue(&ctx, req, &resp);
    if (!st.ok()) {
        std::cerr << "[KUKSA] PublishValue(" << path
                  << ") failed: " << st.error_message() << "\n";
    }
}

void KuksaClient::publishFloat(const std::string& path, float value)
{
    grpc::ClientContext ctx;
    ctx.AddMetadata("authorization", std::string("Bearer ") + impl_->jwt); // Add JWT for authentication
    kuksa::val::v2::PublishValueRequest req;
    kuksa::val::v2::PublishValueResponse resp;

    req.mutable_signal_id()->set_path(path);
    req.mutable_data_point()->mutable_value()->set_float_(value);

    grpc::Status st = impl_->stub->PublishValue(&ctx, req, &resp);
    if (!st.ok()) {
        std::cerr << "[KUKSA] PublishValue(" << path
                  << ") failed: " << st.error_message() << "\n";
    }
}

void KuksaClient::publishUint32(const std::string& path, std::uint32_t value)
{
    grpc::ClientContext ctx;
    ctx.AddMetadata("authorization", std::string("Bearer ") + impl_->jwt); // Add JWT for authentication
    kuksa::val::v2::PublishValueRequest req;
    kuksa::val::v2::PublishValueResponse resp;

    req.mutable_signal_id()->set_path(path);
    req.mutable_data_point()->mutable_value()->set_uint32(value);

    grpc::Status st = impl_->stub->PublishValue(&ctx, req, &resp);
    if (!st.ok()) {
        std::cerr << "[KUKSA] PublishValue(" << path
                  << ") failed: " << st.error_message() << "\n";
    }
}

