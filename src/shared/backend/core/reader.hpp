/**
 * @file reader.hpp
 * @brief Generic Kuksa signal reader
 * @desc Receives signals from Kuksa and emits generic path+value pairs
 */

#ifndef READER_HPP
#define READER_HPP

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QVariant>
#include <atomic>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <grpcpp/grpcpp.h>
#include "kuksa/val.grpc.pb.h"
#include "kuksa/types.pb.h"
#include "signalrouter.hpp"

using kuksa::val::v2::VAL;

// Worker class that runs in a separate thread
class ReaderWorker : public QObject
{
    Q_OBJECT

public:
    explicit ReaderWorker(const std::string &server, const std::vector<std::string> &signalPaths);
    ~ReaderWorker();

public slots:
    void startReading();
    void stopReading();

signals:
    void signalReceived(QString path, QVariant value);
    
    void connectionError(QString error);
    void connected();

private:
    std::string _server;
    std::vector<std::string> _signalPaths;
    std::atomic<bool> _shouldStop;
    
    // Helper functions
    static std::string read_file(const std::string& path);
    static std::unique_ptr<VAL::Stub> create_val_stub(const std::string& host_port);
    static QVariant datapoint_to_variant(const kuksa::val::v2::Datapoint &dp);
};

// Main Reader class that manages the worker thread
class Reader : public QObject
{
    Q_OBJECT

public:
    explicit Reader(const std::vector<std::string> &signalPaths, SignalRouter *router, QObject *parent = nullptr);
    ~Reader();

signals:
    void signalReceived(QString path, QVariant value);
    void connectionError(QString error);
    void connected();

private:
    QThread *_workerThread;
    ReaderWorker *_worker;
    std::string _server = "10.21.220.191:55555";
};

#endif /* READER_HPP */