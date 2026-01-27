#ifndef READER_HPP
#define READER_HPP

#include <QObject>
#include <QTcpSocket>
#include <QStringList>
#include <QDebug>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <grpcpp/grpcpp.h>
#include "kuksa/val.grpc.pb.h"
#include "kuksa/types.pb.h"
#include "kuksa/can_id.h"

using kuksa::val::v2::VAL;

class Reader : public QObject
{
    Q_OBJECT

public:
    explicit Reader(QObject *parent = nullptr);

signals:
    void speedReceived(double speed);
    void temperatureReceived(double temperature);

private:
    std::string _server = "10.21.220.191:5555";

    static std::unique_ptr<VAL::Stub> create_val_stub(const std::string &host_port);
    static std::string value_to_string(const kuksa::val::v2::Value &v);
};

#endif /* READER_HPP */