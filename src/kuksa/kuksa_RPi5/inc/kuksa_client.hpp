#pragma once
#include <cstdint>
#include <memory>
#include <string>

#include "interface_kuksa_client.hpp"

class KuksaClient : public IKuksaClient {
public:
    explicit KuksaClient(const std::string& addr);
    ~KuksaClient();

    bool ok() const;

    void publishDouble(const std::string& path, double value) override;
    void publishInt32 (const std::string& path, std::int32_t value) override;
    void publishBool  (const std::string& path, bool value) override;
    void publishFloat (const std::string& path, float value) override;
    void publishUint32(const std::string& path, std::uint32_t value) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

