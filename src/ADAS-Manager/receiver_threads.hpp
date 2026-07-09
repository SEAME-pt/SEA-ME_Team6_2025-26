#pragma once

#include "shared_state.hpp"
#include "socket_receiver.hpp"
#include "joystick_receiver.hpp"
#include "can_receiver.hpp"
#include "can_protocol.h"
#include <atomic>
#include <chrono>
#include <cstdio>
#include <mutex>
#include <thread>

// Defined in adas_manager.cpp — signals all receiver threads to stop.
extern std::atomic<bool> running;

static const char* LANE_SOCKET   = "/tmp/adas_lane.sock";
static const char* OBJECT_SOCKET = "/tmp/adas_objects.sock";
static const char* V2I_SOCKET    = "/tmp/adas_v2i.sock";

// ── Receiver threads ──────────────────────────────────────────────────────────
void status_thread(SharedState& state) {
    CanReceiver rx;
    if (rx.init() < 0) {
        fprintf(stderr, "[STATUS] Failed to init CAN receiver — speed feedback disabled\n");
        return;
    }
    printf("[STATUS] Listening on CAN (0x213)\n");

    while (running) {
        CtrlStatus_t st{};
        if (rx.read_ctrl_status(st)) {
            std::lock_guard<std::mutex> lk(state.mtx);
            state.current_speed_cms = st.current_speed_cms;
            state.gap_cm            = st.gap_cm;
            state.status_valid      = true;
        }
    }
    rx.close_fd();
}

void lane_thread(SharedState& state) {
    SocketReceiver rx(LANE_SOCKET);
    if (rx.init() < 0) { fprintf(stderr, "[LANE] Failed to init socket\n"); return; }
    printf("[LANE] Listening on %s\n", LANE_SOCKET);

    LaneFrame frame{};
    while (running) {
        bool ok = rx.receiveLatest(frame);
        // Timeout de socket ≠ sensor morto: a 10.5 fps o período entre frames
        // (~95ms) excede o timeout do receiver — escrever false aqui faz a
        // validade piscar. A expiração é do watchdog (lane_timeout_ms).
        if (!ok) continue;
        std::lock_guard<std::mutex> lk(state.mtx);
        state.lane         = frame;
        state.lane_valid   = true;
        state.last_lane_ts = std::chrono::steady_clock::now();
    }
    rx.close_fd();
}

void object_thread(SharedState& state) {
    SocketReceiver rx(OBJECT_SOCKET);
    if (rx.init() < 0) { fprintf(stderr, "[OBJ] Failed to init socket\n"); return; }
    printf("[OBJ] Listening on %s\n", OBJECT_SOCKET);

    ObjectFrame frame{};
    while (running) {
        bool ok = rx.receiveLatest(frame);
        // Igual à lane: false no timeout fazia o hold de STOP/vermelho
        // (throttle_limit=0) pulsar a ~10.5 Hz. Expira via obj_timeout_ms.
        if (!ok) continue;
        std::lock_guard<std::mutex> lk(state.mtx);
        state.object       = frame;
        state.object_valid = true;
        state.last_obj_ts  = std::chrono::steady_clock::now();
    }
    rx.close_fd();
}

void v2i_thread(SharedState& state) {
    SocketReceiver rx(V2I_SOCKET);
    if (rx.init() < 0) { fprintf(stderr, "[V2I] Failed to init socket\n"); return; }
    printf("[V2I] Listening on %s\n", V2I_SOCKET);

    V2IFrame frame{};
    while (running) {
        bool ok = rx.receiveLatest(frame);
        // Mesmo padrão de lane/object: timeout de socket não é V2I morto.
        if (!ok) continue;
        std::lock_guard<std::mutex> lk(state.mtx);
        state.v2i         = frame;
        state.v2i_valid   = true;
        state.last_v2i_ts = std::chrono::steady_clock::now();
    }
    rx.close_fd();
}

void joystick_thread(SharedState& state) {
    static constexpr int RETRY_INTERVAL_MS = 1000;

    JoystickReceiver rx;

    // Retry loop — recovers from transient init failures without killing the thread
    while (running) {
        if (rx.init() >= 0) break;
        fprintf(stderr, "[JOY] Failed to init %s — retry in %dms\n",
                JOYSTICK_SOCKET, RETRY_INTERVAL_MS);
        std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_INTERVAL_MS));
    }

    if (!running) return;
    printf("[JOY] Listening on %s\n", JOYSTICK_SOCKET);

    while (running) {
        JoystickMsg msg = rx.receive();
        std::lock_guard<std::mutex> lk(state.mtx);
        if (msg.type == JoystickMsg::Type::J) {
            state.joy_steering = msg.steering;
            state.joy_throttle = msg.throttle;
            state.joy_valid    = true;
            state.last_joy_ts  = std::chrono::steady_clock::now();
        } else if (msg.type == JoystickMsg::Type::T) {
            state.joy_toggle = true;
        } else if (msg.type == JoystickMsg::Type::FORCE_MANUAL) {
            state.joy_force_manual = true;
        } else if (msg.type == JoystickMsg::Type::FORCE_AUTO) {
            state.joy_force_auto = true;
        }
    }
    rx.close_fd();
}
