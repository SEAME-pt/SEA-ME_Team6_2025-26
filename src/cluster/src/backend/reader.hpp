#ifndef READER_HPP
#define READER_HPP

#include <QObject>
#include <QThread>
#include <QDebug>
#include <atomic>
#include <memory>
#include <string>
#include <vector>
#include <grpcpp/grpcpp.h>
#include "kuksa/val.grpc.pb.h"
#include "kuksa/types.pb.h"

using kuksa::val::v2::VAL;

// Worker class that runs in a separate thread
class ReaderWorker : public QObject
{
    Q_OBJECT

public:
    explicit ReaderWorker(const std::string &server);
    ~ReaderWorker();

public slots:
    void startReading();
    void stopReading();

signals:
    void speedReceived(double speed);
    void temperatureReceived(double temperature);
    void distanceReceived(double distance);
    void frontDistanceReceived(double frontDistance);
    void wheelSpeedReceived(double wheelSpeed);
    void voltageReceived(double voltage);
    void headingReceived(double heading);
    void connectionError(QString error);
    void connected();

private:
    std::string _server;
    std::atomic<bool> _shouldStop;
    
    std::unique_ptr<VAL::Stub> create_val_stub(const std::string &host_port);
    static std::string value_to_string(const kuksa::val::v2::Value &v);
};

// Main Reader class that manages the worker thread
class Reader : public QObject
{
    Q_OBJECT

public:
    explicit Reader(QObject *parent = nullptr);
    ~Reader();

signals:
    void speedReceived(double speed);
    void temperatureReceived(double temperature);
    void distanceReceived(double distance);
    void frontDistanceReceived(double frontDistance);
    void wheelSpeedReceived(double wheelSpeed);
    void voltageReceived(double voltage);
    void headingReceived(double heading);
    void connectionError(QString error);
    void connected();

private:
    QThread *_workerThread;
    ReaderWorker *_worker;
    std::string _server = "10.21.220.191:55555";
};

#endif /* READER_HPP */