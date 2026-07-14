#pragma once

#include "socket_receiver.hpp"
#include "drive_mode.hpp"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

// ── KUKSA Bridge — thread assíncrona ─────────────────────────────────────────
// O loop de controlo chama pub_lane / pub_objects sem bloquear.
// A thread de bridge faz o fprintf/fflush ao pipe do subprocess.
// Queue com max 4 mensagens — se KUKSA estiver lento, descarta (telemetria).
class KuksaBridge {
public:
    bool start(const char* cmd) {
        pipe_ = popen(cmd, "w");
        if (!pipe_) {
            fprintf(stderr, "[KUKSA] Falhou a lançar bridge: %s\n", cmd);
            return false;
        }
        printf("[KUKSA] Bridge iniciado: %s\n", cmd);
        running_ = true;
        thread_  = std::thread(&KuksaBridge::_loop, this);
        return true;
    }

    void pub_mode(DriveMode mode) {
        if (!pipe_) return;
        char buf[32];
        snprintf(buf, sizeof(buf), "D %s\n",
                 mode == DriveMode::MANUAL ? "MANUAL" : "AUTONOMOUS");
        _enqueue(buf);
    }

    void pub_lane(float deviation, uint8_t status_code) {
        if (!pipe_) return;
        const char* status = (status_code < 4) ? LANE_STATUS_STR[status_code] : "none";
        char buf[64];
        snprintf(buf, sizeof(buf), "L %.4f %s\n", deviation, status);
        _enqueue(buf);
    }

    void pub_objects(const ObjectFrame& obj) {
        if (!pipe_) return;

        float speed_limit = 0.0f;
        char  signs_buf[128]  = "[";
        char  extras_buf[128] = "[";
        int   spos = 1, epos = 1;
        bool  first_sign = true, first_extra = true;

        for (uint8_t i = 0; i < obj.count && i < MAX_OBJECTS; ++i) {
            const auto& o = obj.objects[i];
            switch (o.class_id) {
                case SIGN_SPEED_30: speed_limit = 30.0f; break;
                case SIGN_SPEED_50: speed_limit = 50.0f; break;
                case SIGN_SPEED_80: speed_limit = 80.0f; break;
                case SIGN_STOP:
                case SIGN_TL_RED:
                    spos += snprintf(signs_buf + spos, sizeof(signs_buf) - spos - 2,
                                     "%s\"stop\"", first_sign ? "" : ",");
                    first_sign = false; break;
                case SIGN_YIELD:
                case SIGN_TL_YELLOW:
                    spos += snprintf(signs_buf + spos, sizeof(signs_buf) - spos - 2,
                                     "%s\"yield\"", first_sign ? "" : ",");
                    first_sign = false; break;
                case SIGN_TL_GREEN:
                    spos += snprintf(signs_buf + spos, sizeof(signs_buf) - spos - 2,
                                     "%s\"tl_green\"", first_sign ? "" : ",");
                    first_sign = false; break;
                case SIGN_PEDESTRIAN:
                    spos += snprintf(signs_buf + spos, sizeof(signs_buf) - spos - 2,
                                     "%s\"pedestrian\"", first_sign ? "" : ",");
                    first_sign = false; break;
                default:
                    epos += snprintf(extras_buf + epos, sizeof(extras_buf) - epos - 2,
                                     "%s\"obj:%.2f:%.1fm\"", first_extra ? "" : ",",
                                     o.confidence, o.distance);
                    first_extra = false; break;
            }
        }
        snprintf(signs_buf  + spos, 3, "]");
        snprintf(extras_buf + epos, 3, "]");

        char msg[256];
        snprintf(msg, sizeof(msg), "O %.1f none %s %s\n",
                 speed_limit, signs_buf, extras_buf);
        _enqueue(msg);
    }

    void stop() {
        running_ = false;
        cv_.notify_all();
        if (thread_.joinable()) thread_.join();
        if (pipe_) { pclose(pipe_); pipe_ = nullptr; }
    }

private:
    static constexpr int MAX_QUEUE = 4;

    FILE*                    pipe_    = nullptr;
    std::atomic<bool>        running_ {false};
    std::thread              thread_;
    std::mutex               mtx_;
    std::condition_variable  cv_;
    std::queue<std::string>  queue_;

    void _enqueue(const char* msg) {
        std::lock_guard<std::mutex> lk(mtx_);
        if ((int)queue_.size() >= MAX_QUEUE) queue_.pop();  // descarta mais antigo
        queue_.push(msg);
        cv_.notify_one();
    }

    void _loop() {
        while (running_) {
            std::unique_lock<std::mutex> lk(mtx_);
            cv_.wait_for(lk, std::chrono::milliseconds(100),
                         [&]{ return !queue_.empty() || !running_; });
            while (!queue_.empty()) {
                std::string msg = std::move(queue_.front());
                queue_.pop();
                lk.unlock();
                if (pipe_) { fputs(msg.c_str(), pipe_); fflush(pipe_); }
                lk.lock();
            }
        }
    }
};
