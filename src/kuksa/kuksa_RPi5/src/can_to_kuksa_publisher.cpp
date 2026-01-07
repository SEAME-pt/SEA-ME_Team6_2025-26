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
#include "../inc/can_decode.hpp"

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
    const uint32_t id = frame.can_id & CAN_SFF_MASK; //Changed the mask to standard frame format
    const std::uint8_t  dlc = frame.can_dlc;

    switch (id) {
        case CAN_ID_WHEEL_SPEED: { //speedx10
            if (dlc < 8) return;

            const std::int16_t  rpm          = i16_le(&frame.data[0]);     // bytes 0-1
            const std::uint32_t total_pulses = u32_le(&frame.data[2]);     // bytes 2-5
            const std::uint16_t raw_speed    = u16_le(&frame.data[6]);     // bytes 6-7

            const double speed = raw_speed / 10.0;
            publish_double(stub, "Vehicle.Speed", speed);
            break;
        }
        case CAN_ID_ENVIRONMENT: { // tempx10
            if (dlc < 8) return;

            const std::int16_t raw_temp = i16_le(&frame.data[0]);
            const std::uint8_t humidity = u8(&frame.data[2]);
            const std::uint8_t reserved = u8(&frame.data[3]);
            const std::uint32_t pressure = (static_cast<std::uint32_t>(frame.data[4]) << 16) |
                                                (static_cast<std::uint32_t>(frame.data[5]) << 8)  |
                                                 static_cast<std::uint32_t>(frame.data[6]);
            const std::uint8_t status   = u8(&frame.data[7]);

            const double temp = raw_temp / 10.0;
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
