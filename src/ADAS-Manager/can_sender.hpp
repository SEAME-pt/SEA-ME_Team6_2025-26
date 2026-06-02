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

#include "can_protocol.h"   // CtrlCmd_t, CtrlMode_t, CAN_ID_CTRL_CMD, HEADWAY_*

static constexpr const char* CAN_CHANNEL  = "can1";
static constexpr uint32_t    CAN_ID_ESTOP = 0x001;

// CRC-8 (poly=0x07, init=0x00) — matches STM32 calculate_crc8()
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

    // CAN 0x202 — CtrlCmd_t — unified longitudinal+lateral control
    bool send_ctrl_cmd(CtrlMode_t mode,
                       int8_t   steering,
                       int8_t   throttle,
                       uint16_t target_speed_cms = 0,
                       uint8_t  headway          = HEADWAY_MEDIUM,
                       bool     aeb_request      = false) {
        CtrlCmd_t cmd{};
        cmd.mode             = static_cast<uint8_t>(mode);
        cmd.steering         = steering;
        cmd.throttle         = throttle;
        cmd.target_speed_cms = target_speed_cms;
        cmd.headway          = headway & 0x0F;
        cmd.aeb_request      = aeb_request ? 1 : 0;
        cmd.reserved         = 0;
        cmd.counter          = counter_++ % 15;
        cmd.crc              = _crc8(reinterpret_cast<const uint8_t*>(&cmd),
                                     sizeof(cmd) - 1);

        struct can_frame frame{};
        frame.can_id  = CAN_ID_CTRL_CMD;
        frame.can_dlc = 8;
        memcpy(frame.data, &cmd, sizeof(cmd));
        bool ok = _send(frame);

        if (!ok || (counter_ % 25) == 0) {
            printf("[CAN] 0x%03X mode=%u steer=%+d thr=%+d%s %s\n",
                   CAN_ID_CTRL_CMD, static_cast<unsigned>(mode), steering, throttle,
                   aeb_request ? " AEB!" : "", ok ? "OK" : "FAIL");
        }
        return ok;
    }

    // CAN 0x001 — EmergencyStop_t — one-shot pulse
    bool send_estop(uint8_t active, uint8_t reason = 0x10) {
        uint8_t body[7] = {
            active,
            2,        // source = AGL
            0, 0,     // distance_mm
            reason,
            0, 0,
        };
        struct can_frame frame{};
        frame.can_id  = CAN_ID_ESTOP;
        frame.can_dlc = 8;
        memcpy(frame.data, body, 7);
        frame.data[7] = _crc8(body, 7);
        return _send(frame);
    }

    void close_fd() { if (fd_ >= 0) { close(fd_); fd_ = -1; } }

private:
    bool _send(const struct can_frame& frame) {
        if (fd_ < 0) return false;
        return write(fd_, &frame, sizeof(frame)) == sizeof(frame);
    }

    const char* channel_;
    int         fd_      = -1;
    uint8_t     counter_ = 0;
};
