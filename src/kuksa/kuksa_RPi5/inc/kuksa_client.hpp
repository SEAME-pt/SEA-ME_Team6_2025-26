#pragma once
#include <cstdint>
#include <memory>
#include <string>

class KuksaClient {
public:
    explicit KuksaClient(const std::string& addr);
    ~KuksaClient();

    bool ok() const;

    void publishDouble(const std::string& path, double value);
    void publishInt32(const std::string& path, std::int32_t value);

private:
    struct Impl;                  
    std::unique_ptr<Impl> impl_;
};
