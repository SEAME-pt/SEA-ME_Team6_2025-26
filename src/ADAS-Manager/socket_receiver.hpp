#pragma once

#include <cstdint>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

static constexpr int RECV_TIMEOUT_MS = 60;

// ── Lane ──────────────────────────────────────────────────────────────────────

#define MAX_WAYPOINTS 20
#define POLY_DEGREE    3   // cubic → 4 coefficients (a0 + a1*y + a2*y² + a3*y³)

struct __attribute__((packed)) Waypoint {
    float x;
    float y;
};

// Per-lane data for MPC — populated by inference, reserved for future use
struct __attribute__((packed)) LaneObject {
    uint8_t  waypoint_count;
    Waypoint waypoints[MAX_WAYPOINTS];
    float    polyfit[POLY_DEGREE + 1];
};

// lane_status encoding: 0=none, 1=left, 2=right, 3=both
// LaneObject fields are mock zeros until MPC integration
struct __attribute__((packed)) LaneFrame {
    float      lateral_deviation;
    uint8_t    lane_status;
    LaneObject lane_left;
    LaneObject lane_right;
};

// ── Object detection ──────────────────────────────────────────────────────────

enum SignClass : uint8_t {
    SIGN_UNKNOWN     = 0,
    SIGN_STOP        = 1,   // stop_sign
    SIGN_YIELD       = 2,   // traffic_priority
    SIGN_SPEED_30    = 3,
    SIGN_SPEED_50    = 4,   // speed_50
    SIGN_SPEED_80    = 5,   // speed_80
    SIGN_OBSTACLE    = 6,   // car, obstacle
    SIGN_PEDESTRIAN  = 7,   // crosswalk, pedestrians_crossing
    SIGN_TL_GREEN    = 8,   // traffic_lights_green
    SIGN_TL_RED      = 9,   // traffic_lights_red
    SIGN_TL_YELLOW   = 10,  // traffic_lights_yellow
};

#define MAX_OBJECTS 4

struct __attribute__((packed)) DetectedObject {
    uint8_t class_id;     // SignClass
    float   confidence;
    float   distance;     // metres
    float   theta_cam;    // horizontal angle deg: 0=straight, +=right, -=left (Vasco fills this)
};

struct __attribute__((packed)) ObjectFrame {
    uint8_t        count;
    DetectedObject objects[MAX_OBJECTS];
};

// ── V2I mobility scenarios ───────────────────────────────────────────────────

enum V2ITrafficLightState : uint8_t {
    V2I_TL_UNKNOWN = 0,
    V2I_TL_RED     = 1,
    V2I_TL_YELLOW  = 2,
    V2I_TL_GREEN   = 3,
};

enum V2IBarrierState : uint8_t {
    V2I_BARRIER_UNKNOWN = 0,
    V2I_BARRIER_OPEN    = 1,
    V2I_BARRIER_CLOSED  = 2,
    V2I_BARRIER_MOVING  = 3,
};

struct __attribute__((packed)) V2IFrame {
    uint8_t traffic_light_state;  // V2ITrafficLightState
    uint8_t barrier_state;        // V2IBarrierState
    uint8_t priority_active;      // 0=false, 1=true
    uint8_t reserved;             // future use (alignment/versioning)
};

// ── Socket receiver ───────────────────────────────────────────────────────────

class SocketReceiver {
public:
    explicit SocketReceiver(const char* path, int timeout_ms = RECV_TIMEOUT_MS)
        : path_(path), timeout_ms_(timeout_ms) {}

    int init() {
        fd_ = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (fd_ < 0) return -1;

        unlink(path_);

        struct sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, path_, sizeof(addr.sun_path) - 1);

        if (bind(fd_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
            close(fd_);
            fd_ = -1;
            return -1;
        }

        struct timeval tv{ 0, timeout_ms_ * 1000 };
        setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        return fd_;
    }

    template <typename Frame>
    bool receiveLatest(Frame& out) {
        Frame   buf{};
        ssize_t n = -1;

        while (true) {
            ssize_t r = recv(fd_, &buf, sizeof(buf), MSG_DONTWAIT);
            if (r == static_cast<ssize_t>(sizeof(buf))) {
                n = r;
            } else {
                break;
            }
        }

        if (n == static_cast<ssize_t>(sizeof(buf))) {
            out = buf;
            return true;
        }

        n = recv(fd_, &buf, sizeof(buf), 0);
        if (n == static_cast<ssize_t>(sizeof(buf))) {
            out = buf;
            return true;
        }

        return false;
    }

    void close_fd() {
        if (fd_ >= 0) {
            close(fd_);
            fd_ = -1;
        }
    }

private:
    const char* path_;
    int         timeout_ms_;
    int         fd_ = -1;
};
