#ifndef WRITER_HPP
#define WRITER_HPP

#include <QObject>
#include <QVariant>
#include <QString>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "kuksa/val.grpc.pb.h"

using kuksa::val::v2::VAL;

class Writer : public QObject
{
    Q_OBJECT

public:
    explicit Writer(QObject *parent = nullptr);

public slots:
    void writeSignal(const QString &path, const QVariant &value);

signals:
    void writeError(QString path, QString error);
    void writeSuccess(QString path);

private:
    std::string _server = "10.21.220.191:55555";

    static std::string read_file(const std::string &path);
    static std::unique_ptr<VAL::Stub> create_val_stub(const std::string &host_port);
    static kuksa::val::v2::Value variant_to_value(const QVariant &value);
};

#endif /* WRITER_HPP */