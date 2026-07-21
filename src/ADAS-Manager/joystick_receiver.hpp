#pragma once

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

static constexpr const char* JOYSTICK_SOCKET    = "/tmp/adas_joystick.sock";
static constexpr int         JOYSTICK_TIMEOUT_MS = 60;

struct JoystickMsg {
    enum class Type : uint8_t { NONE, J, T, FORCE_MANUAL, FORCE_AUTO, EMERGENCY_TOGGLE } type = Type::NONE;
    int8_t steering = 0;
    int8_t throttle = 0;
};

class JoystickReceiver {
public:
    explicit JoystickReceiver(const char* path       = JOYSTICK_SOCKET,
                              int         timeout_ms = JOYSTICK_TIMEOUT_MS)
        : path_(path), timeout_ms_(timeout_ms) {}

    int init() {
        fd_ = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (fd_ < 0) return -1;

        unlink(path_);

        struct sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, path_, sizeof(addr.sun_path) - 1);

        if (bind(fd_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
            close(fd_); fd_ = -1; return -1;
        }

        struct timeval tv{ 0, timeout_ms_ * 1000 };
        setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        return fd_;
    }

    JoystickMsg receive() {
        char    buf[64];
        ssize_t n = recv(fd_, buf, sizeof(buf) - 1, 0);
        if (n <= 0) return {};
        buf[n] = '\0';
        return _parse(buf);
    }

    void close_fd() {
        if (fd_ >= 0) { close(fd_); fd_ = -1; }
    }

private:
    static JoystickMsg _parse(const char* s) {
        JoystickMsg msg{};
        if (s[0] == 'T') {
            msg.type = JoystickMsg::Type::T;
        } else if (s[0] == 'M') {
            msg.type = JoystickMsg::Type::FORCE_MANUAL;
        } else if (s[0] == 'A') {
            msg.type = JoystickMsg::Type::FORCE_AUTO;
        } else if (s[0] == 'E') {
            msg.type = JoystickMsg::Type::EMERGENCY_TOGGLE;
        } else if (s[0] == 'J') {
            int st = 0, th = 0;
            if (sscanf(s, "J %d %d", &st, &th) == 2) {
                msg.type     = JoystickMsg::Type::J;
                msg.steering = static_cast<int8_t>(st);
                msg.throttle = static_cast<int8_t>(th);
            }
        }
        return msg;
    }

    const char* path_;
    int         timeout_ms_;
    int         fd_ = -1;
};
