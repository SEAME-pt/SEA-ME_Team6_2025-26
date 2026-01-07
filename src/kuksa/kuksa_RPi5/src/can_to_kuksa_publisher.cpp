// kuksa_can_publisher.cpp
//
// KUKSA publisher that reads CAN frames (SocketCAN) and publishes to KUKSA Databroker
//
// CAN IDs:
//  - 0x100 : speedx10      (uint16, little-endian) -> Vehicle.Speed (double)
//  - 0x111 : tempx10       (int16, little-endian)  -> Vehicle.Cabin.AirTemperature (double)
//  - 0x112 : heartbeat     (uint8, little-endian) -> Vehicle.Test.Heartbeat (int8)
//

#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <grpcpp/grpcpp.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

// Generated KUKSA headers
#include "kuksa/val/v2/val.grpc.pb.h"
#include "kuksa/val/v2/types.pb.h"

#include "../../can_frames.h"

using kuksa::val::v2::VAL;

// Configuration
static const char* KUKSA_ADDR = "127.0.0.1:55555";
static const char* CAN_IFACE  = "can1"; //david to ruben: change if needed

// KUKSA helpers
static std::unique_ptr<VAL::Stub> create_val_stub()
{
    auto channel = grpc::CreateChannel(KUKSA_ADDR,
                                       grpc::InsecureChannelCredentials());
    return VAL::NewStub(channel);
}

static void publish_double(VAL::Stub* stub,
                           const std::string& path,
                           double value)
{
    grpc::ClientContext ctx;
    kuksa::val::v2::PublishValueRequest req;
    kuksa::val::v2::PublishValueResponse resp;

    req.mutable_signal_id()->set_path(path);
    req.mutable_data_point()->mutable_value()->set_double_(value);

    grpc::Status st = stub->PublishValue(&ctx, req, &resp);
    if (!st.ok()) {
        std::cerr << "[KUKSA] PublishValue(" << path
                  << ") failed: " << st.error_message() << "\n";
    }
}

static void publish_int32(VAL::Stub* stub,
                          const std::string& path,
                          std::int32_t value)
{
    grpc::ClientContext ctx;
    kuksa::val::v2::PublishValueRequest req;
    kuksa::val::v2::PublishValueResponse resp;

    req.mutable_signal_id()->set_path(path);
    req.mutable_data_point()->mutable_value()->set_int32(value);

    grpc::Status st = stub->PublishValue(&ctx, req, &resp);
    if (!st.ok()) {
        std::cerr << "[KUKSA] PublishValue(" << path
                  << ") failed: " << st.error_message() << "\n";
    }
}

// CAN helpers (little-endian decoding)
static inline std::uint16_t u16_le(const std::uint8_t* d)
{
    return static_cast<std::uint16_t>(d[0]) |
           (static_cast<std::uint16_t>(d[1]) << 8);
}

//Same logic as before but casts to int16_t
static inline std::int16_t i16_le(const std::uint8_t* d)
{
    return static_cast<std::int16_t>(u16_le(d));
}

static inline std::uint8_t u8(const std::uint8_t* d)
{
    return d[0];
}

// CAN socket setup
static int open_can_socket()
{
    int s = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("socket(PF_CAN)");
        return -1;
    }

    struct ifreq ifr {};
    std::snprintf(ifr.ifr_name, IFNAMSIZ, "%s", CAN_IFACE);

    if (::ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl(SIOCGIFINDEX)");
        ::close(s);
        return -1;
    }

    struct sockaddr_can addr {};
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (::bind(s, reinterpret_cast<struct sockaddr*>(&addr),
               sizeof(addr)) < 0) {
        perror("bind(AF_CAN)");
        ::close(s);
        return -1;
    }

    return s;
}

// CAN frame handler
static void handle_can_frame(const struct can_frame& frame,
                             VAL::Stub* stub)
{
    const std::uint32_t id  = frame.can_id & CAN_EFF_MASK;
    const std::uint8_t  dlc = frame.can_dlc;

    switch (id) {
        case 0x100: { //speedx10
            if (dlc < 2) return;
            const std::uint16_t raw = u16_le(frame.data);
            const double speed = raw / 10.0;
            publish_double(stub, "Vehicle.Speed", speed);
            break;
        }
        case 0x111: { // tempx10
            if (dlc < 2) return;
            const std::int16_t raw = i16_le(frame.data);
            const double temp = raw / 10.0;
            publish_double(stub, "Vehicle.Exterior.AirTemperature", temp);
            break;
        }
        case 0x101: { // heartbeat
            if (dlc < 1) return;

            const std::uint8_t hb_u8 = u8(frame.data);
            publish_int32(stub, "Vehicle.SafetyCritical.Heartbeat", static_cast<std::int32_t>(hb_u8));

            break;
        }
        default:
            // Ignore other CAN IDs
            break;
    }
}

// main (orchestration only)
int main()
{
    // 1) Connect to KUKSA
    auto stub = create_val_stub();
    std::cout << "[KUKSA] Connected to " << KUKSA_ADDR << "\n";

    // 2) Open CAN socket
    int can_sock = open_can_socket();
    if (can_sock < 0) {
        std::cerr << "[CAN] Failed to open " << CAN_IFACE << "\n";
        return 1;
    }

    std::cout << "[CAN] Listening on " << CAN_IFACE << "\n";

    // 3) Read CAN frames forever
    while (true) {
        struct can_frame frame {};
        ssize_t n = ::read(can_sock, &frame, sizeof(frame));
        if (n < 0) {
            perror("read(can)");
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        if (static_cast<size_t>(n) != sizeof(frame)) {
            continue;
        }

        handle_can_frame(frame, stub.get());
    }

    ::close(can_sock);
    return 0;
}
