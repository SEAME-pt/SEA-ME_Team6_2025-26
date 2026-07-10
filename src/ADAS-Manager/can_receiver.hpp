#pragma once

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include "can_id.h"
#include "can_protocol.h"   // CtrlStatus_t, CAN_ID_CTRL_STATUS

class CanReceiver {
public:
    explicit CanReceiver(const char* channel = "can1") : channel_(channel) {}

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

        // Accept only CtrlStatus (0x213) — contains current_speed_cms + gap_cm (SRF08)
        struct can_filter filter[1];
        filter[0].can_id   = CAN_ID_CTRL_STATUS;
        filter[0].can_mask = CAN_SFF_MASK;
        setsockopt(fd_, SOL_CAN_RAW, CAN_RAW_FILTER, filter, sizeof(filter));

        // 100 ms receive timeout so the thread can check running flag
        struct timeval tv{ 0, 100000 };
        setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        return fd_;
    }

    // Returns true if a valid CtrlStatus frame (0x213) was received.
    // Contains current_speed_cms (Hall encoder) and gap_cm (SRF08).
    bool read_ctrl_status(CtrlStatus_t& status) {
        if (fd_ < 0) return false;

        struct can_frame frame{};
        if (read(fd_, &frame, sizeof(frame)) != sizeof(frame)) return false;
        if (frame.can_id  != CAN_ID_CTRL_STATUS)               return false;
        if (frame.can_dlc <  sizeof(CtrlStatus_t))              return false;

        memcpy(&status, frame.data, sizeof(CtrlStatus_t));
        return true;
    }

    void close_fd() {
        if (fd_ >= 0) { close(fd_); fd_ = -1; }
    }

private:
    const char* channel_;
    int         fd_ = -1;
};
