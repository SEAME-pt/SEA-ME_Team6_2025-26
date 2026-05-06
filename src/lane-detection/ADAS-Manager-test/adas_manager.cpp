#include "socket_receiver.hpp"
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <mutex>
#include <thread>

static const char* LANE_SOCKET   = "/tmp/adas_lane.sock";
static const char* OBJECT_SOCKET = "/tmp/adas_objects.sock";

static const char* LANE_STATUS_STR[] = {"none", "left", "right", "both"};

static std::atomic<bool> running{true};

static void on_signal(int) { running = false; }

// ── Shared state ──────────────────────────────────────────────────────────────
struct SharedState {
    std::mutex mtx;

    LaneFrame lane{};
    bool      lane_valid = false;

    ObjectFrame object{};
    bool        object_valid = false;
};

// ── Threads ───────────────────────────────────────────────────────────────────
void lane_thread(SharedState& state) {
    SocketReceiver rx(LANE_SOCKET);
    if (rx.init() < 0) {
        fprintf(stderr, "[LANE] Failed to init socket\n");
        return;
    }
    printf("[LANE] Listening on %s\n", LANE_SOCKET);

    LaneFrame frame{};
    while (running) {
        bool ok = rx.receiveLatest(frame);
        std::lock_guard<std::mutex> lk(state.mtx);
        state.lane_valid = ok;
        if (ok) state.lane = frame;
    }
    rx.close_fd();
}

void object_thread(SharedState& state) {
    SocketReceiver rx(OBJECT_SOCKET);
    if (rx.init() < 0) {
        fprintf(stderr, "[OBJ] Failed to init socket\n");
        return;
    }
    printf("[OBJ] Listening on %s\n", OBJECT_SOCKET);

    ObjectFrame frame{};
    while (running) {
        bool ok = rx.receiveLatest(frame);
        std::lock_guard<std::mutex> lk(state.mtx);
        state.object_valid = ok;
        if (ok) state.object = frame;
    }
    rx.close_fd();
}

// ── Main loop ─────────────────────────────────────────────────────────────────
int main() {
    signal(SIGINT,  on_signal);
    signal(SIGTERM, on_signal);

    SharedState state;

    std::thread t_lane(lane_thread,  std::ref(state));
    std::thread t_obj (object_thread, std::ref(state));

    printf("[ADAS] Manager running. Ctrl+C to stop.\n");

    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        LaneFrame   lane{};
        ObjectFrame obj{};
        bool lane_valid, obj_valid;

        {
            std::lock_guard<std::mutex> lk(state.mtx);
            lane       = state.lane;
            lane_valid = state.lane_valid;
            obj        = state.object;
            obj_valid  = state.object_valid;
        }

        const char* lane_str = (lane.lane_status < 4)
                               ? LANE_STATUS_STR[lane.lane_status] : "?";

        if (lane_valid)
            printf("[ADAS] lane=%-5s  dev=%+.2fcm", lane_str, lane.lateral_deviation);
        else
            printf("[ADAS] lane=DEGRADED          ");

        if (obj_valid) {
            printf("  | obj=%u sign(s)", obj.count);
            for (uint8_t i = 0; i < obj.count && i < MAX_OBJECTS; ++i)
                printf("  [class=%u conf=%.2f dist=%.2fm]",
                       obj.objects[i].class_id,
                       obj.objects[i].confidence,
                       obj.objects[i].distance);
            printf("\n");
        } else {
            printf("  | obj=DEGRADED\n");
        }
    }

    t_lane.join();
    t_obj.join();
    printf("[ADAS] Shutdown.\n");
    return 0;
}
