#pragma once

#include <cstdint>
#include <cstring>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdio>

static constexpr const char* CAN_CHANNEL     = "can1";
static constexpr uint32_t    CAN_ID_MOTOR    = 0x200;
static constexpr uint32_t    CAN_ID_JOYSTICK = 0x500;
static constexpr uint32_t    CAN_ID_ESTOP    = 0x001;

// CRC-8 (poly=0x07, init=0x00) — matches lka_safety_monitor.py
static uint8_t _crc8(const uint8_t* data, size_t len) {
    uint8_t crc = 0x00;
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (int j = 0; j < 8; ++j)
            crc = (crc & 0x80) ? ((crc << 1) ^ 0x07) : (crc << 1);
    }
    return crc;
}

class CanSender {
public:
    explicit CanSender(const char* channel = CAN_CHANNEL) : channel_(channel) {}

    int init() {
        fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (fd_ < 0) return -1;

        struct ifreq ifr{};
        strncpy(ifr.ifr_name, channel_, IFNAMSIZ - 1);
        if (ioctl(fd_, SIOCGIFINDEX, &ifr) < 0) {
            close(fd_); fd_ = -1; return -1;
        }

        struct sockaddr_can addr{};
        addr.can_family  = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        if (bind(fd_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
            close(fd_); fd_ = -1; return -1;
        }
        return fd_;
    }

    // CAN 0x200 — MotorCmd_t — matches STM32 handle_motor_cmd()
    bool send_motor_cmd(int8_t steering, int8_t throttle,
                        uint8_t mode, uint8_t flags = 0) {
        uint8_t body[7];
        body[0] = static_cast<uint8_t>(throttle);
        body[1] = static_cast<uint8_t>(steering);
        body[2] = flags;
        body[3] = mode;
        body[4] = 0;           // reserved
        body[5] = 0;           // reserved
        body[6] = counter_++ % 15;

        struct can_frame frame{};
        frame.can_id  = CAN_ID_MOTOR;
        frame.can_dlc = 8;
        memcpy(frame.data, body, 7);
        frame.data[7] = _crc8(body, 7);
        bool ok = _send(frame);
        printf("[CAN] 0x%03X steer=%+d throttle=%d mode=%u %s\n",
               CAN_ID_MOTOR, steering, throttle, mode, ok ? "OK" : "FAIL");
        return ok;
    }

    // CAN 0x500 — legacy joystick format (int16 LE) — kept for compatibility
    bool send_control(int16_t steering, int16_t throttle) {
        struct can_frame frame{};
        frame.can_id  = CAN_ID_JOYSTICK;
        frame.can_dlc = 4;
        memcpy(frame.data,     &steering, 2);
        memcpy(frame.data + 2, &throttle, 2);
        return _send(frame);
    }

    // CAN 0x001 — EmergencyStop_t — matches lka_safety_monitor.py
    // struct.pack('<BBHBxx', active, source=2, distance_mm=0, reason=0x10) + crc8
    bool send_estop(uint8_t active) {
        uint8_t body[7] = {
            active,
            2,      // source = AGL
            0, 0,   // distance_mm (uint16 LE)
            0x10,   // reason = LANE_LOSS
            0, 0,   // reserved
        };
        struct can_frame frame{};
        frame.can_id  = CAN_ID_ESTOP;
        frame.can_dlc = 8;
        memcpy(frame.data, body, 7);
        frame.data[7] = _crc8(body, 7);
        return _send(frame);
    }

    void close_fd() {
        if (fd_ >= 0) { close(fd_); fd_ = -1; }
    }

private:
    bool _send(const struct can_frame& frame) {
        if (fd_ < 0) return false;
        return write(fd_, &frame, sizeof(frame)) == sizeof(frame);
    }

    const char* channel_;
    int         fd_       = -1;
    uint8_t     counter_  = 0;
};
