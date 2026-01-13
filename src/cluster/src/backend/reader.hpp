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
#include "../../../kuksa/kuksa_RPi4_display/generated/kuksa/val/v2/val.grpc.pb.h"
#include "../../../kuksa/kuksa_RPi4_display/generated/kuksa/val/v2/types.pb.h"
#include "../../../kuksa/kuksa_RPi4_display/inc/can_id.h"

using kuksa::val::v2::VAL;

class Reader: public QObject {
    Q_OBJECT

    public:
        explicit Reader(QObject *parent = nullptr);

    signals:
        void speedReceived(double speed);
        void temperatureReceived(double temperature);

    private slots:
        static create_val_stub(const std::string& host_port);
        static value_to_string(const kuksa::val::v2::Value& v);

    private:
        std::string _server = "172.20.10.3:5555";
}

#endif /* READER_HPP */
