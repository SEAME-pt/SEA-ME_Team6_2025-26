#include "writer.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <grpcpp/grpcpp.h>

Writer::Writer(QObject *parent) : QObject(parent)
{
    qDebug() << "[Writer] Initialized";
}

std::string Writer::read_file(const std::string &path)
{
    std::ifstream f(path.c_str(), std::ios::in | std::ios::binary);
    if (!f.is_open())
        throw std::runtime_error("Failed to open file: " + path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

std::unique_ptr<VAL::Stub> Writer::create_val_stub(const std::string &host_port)
{
    grpc::SslCredentialsOptions ssl_opts;
    ssl_opts.pem_root_certs = read_file("/etc/kuksa/tls/ca.crt");
    auto channel = grpc::CreateChannel(host_port, grpc::SslCredentials(ssl_opts));
    return VAL::NewStub(channel);
}

kuksa::val::v2::Value Writer::variant_to_value(const QVariant &value)
{
    kuksa::val::v2::Value v;
    switch (value.typeId()) {
        case QMetaType::Double:
        case QMetaType::Float:
            v.set_double_(value.toDouble()); break;
        case QMetaType::Bool:
            v.set_bool_(value.toBool()); break;
        case QMetaType::Int:
            v.set_int32(value.toInt()); break;
        case QMetaType::QString:
            v.set_string(value.toString().toStdString()); break;
        default:
            qWarning() << "[Writer] Unsupported QVariant type:" << value.typeName();
    }
    return v;
}

void Writer::writeSignal(const QString &path, const QVariant &value)
{
    try {
        auto stub = create_val_stub(_server);

        kuksa::val::v2::PublishValueRequest req;

        req.mutable_signal_id()->set_path(path.toStdString());
        req.mutable_data_point()->mutable_value()->set_bool_(value.toBool());

        grpc::ClientContext ctx;
        std::string jwt = read_file("/etc/kuksa/jwt/publisher.jwt");
        ctx.AddMetadata("authorization", "Bearer " + jwt);

        kuksa::val::v2::PublishValueResponse resp;
        grpc::Status status = stub->PublishValue(&ctx, req, &resp);

        if (!status.ok())
            emit writeError(path, QString::fromStdString(status.error_message()));
        else
            emit writeSuccess(path);

    } catch (const std::exception &e) {
        emit writeError(path, QString("Exception: %1").arg(e.what()));
    }
}