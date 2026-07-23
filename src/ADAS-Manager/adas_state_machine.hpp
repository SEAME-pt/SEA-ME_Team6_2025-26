#pragma once

#include "adas_state.hpp"
#include "drive_mode.hpp"
#include "config.hpp"
#include "socket_receiver.hpp"
#include "lka_controller.hpp"
#include "oa_controller.hpp"
#include "can_sender.hpp"
#include "kuksa_bridge.hpp"
#include <chrono>
#include <cstdint>
#include <cstdio>

// ── Object throttle override ──────────────────────────────────────────────────
// min_throttle: piso de throttle pra YIELD/YELLOW (cfg.curve_throttle_min) —
// um limite absoluto fixo (ex: 50) fica sem efeito se cfg.throttle já andar
// abaixo dele; usar o mesmo piso anti-empanque garante sempre abrandamento
// visível, seja qual for o throttle configurado.
static int obj_throttle_limit(const ObjectFrame& obj, bool obj_valid,
                               float conf_thresh, float collision_dist_m,
                               float stop_sign_dist_m, int min_throttle) {
    if (!obj_valid || obj.count == 0) return 100;

    for (uint8_t i = 0; i < obj.count && i < MAX_OBJECTS; ++i) {
        const auto& o = obj.objects[i];
        if (o.confidence < conf_thresh) continue;
        if (o.distance < collision_dist_m) return 0;
        // SIGN_STOP só trava perto do sinal físico — detetado longe (ex: 5m)
        // não deve parar o carro já. TL_RED mantém-se imediato (semáforo).
        if (o.class_id == SIGN_STOP && o.distance <= stop_sign_dist_m) return 0;
        if (o.class_id == SIGN_TL_RED) return 0;
        if (o.class_id == SIGN_YIELD  || o.class_id == SIGN_TL_YELLOW) return min_throttle;
    }
    return 100;
}

static int v2i_throttle_limit(const V2IFrame& v2i, bool v2i_valid, int min_throttle) {
    if (!v2i_valid) return 100;

    // Emergency priority bypasses V2I restrictions (TL/barrier),
    // but object/collision safety still applies separately.
    if (v2i.priority_active) return 100;

    if (v2i.barrier_state == V2I_BARRIER_CLOSED ||
        v2i.barrier_state == V2I_BARRIER_MOVING)
        return 0;

    switch (v2i.traffic_light_state) {
        case V2I_TL_RED:    return 0;
        case V2I_TL_YELLOW: return min_throttle;
        case V2I_TL_GREEN:  return 100;
        default:            return 100;
    }
}

// ── ADAS state machine (AUTONOMOUS only) ─────────────────────────────────────
static void adas_state_machine(
    bool lane_ok,
    std::chrono::steady_clock::time_point now,
    const AdasConfig& cfg,
    AdasState& adas_state,
    int& degraded_frames,
    int& recovery_frames,
    std::chrono::steady_clock::time_point& degraded_since,
    std::chrono::steady_clock::time_point& emergency_since,
    bool& estop_sent,
    LKAController& lka,
    CanSender& can)
{
    if (adas_state == AdasState::EMERGENCY_STOP) {
        if (lane_ok) {
            recovery_frames++;
            if (recovery_frames >= cfg.recovery_threshold_frames) {
                adas_state      = AdasState::ACTIVE;
                degraded_frames = 0;
                recovery_frames = 0;
                lka.reset();
                if (estop_sent) {
                    can.send_estop(0);
                    estop_sent = false;
                }
                printf("[ADAS] → ACTIVE (recovered)\n");
            }
        } else {
            recovery_frames = 0;
        }

        /* Timeout-based recovery — deadlock breaker.
         *
         * Cenários reais que isto resolve:
         *   1) Carro pára via STM32 AEB (SRF08) numa posição onde a câmara
         *      já não vê lane suficiente. lane_ok fica falso indefinidamente,
         *      recovery normal nunca dispara.
         *   2) Câmara é tapada momentaneamente → EMERGENCY_STOP. Ao destapar,
         *      o detector de lane pode demorar a estabilizar; entretanto o
         *      Manager continua a enviar aeb_request=true em cada tick,
         *      bloqueando o STM32.
         *
         * Solução: ao fim de emergency_timeout_ms em EMERGENCY_STOP, força
         * regresso a DEGRADED (não ACTIVE, por segurança). Em DEGRADED:
         *   - autonomous_driving NÃO envia aeb_request → STM32 fica livre
         *   - se lane voltar a aparecer → transita normal para ACTIVE
         *   - se lane continuar perdida → re-entra em EMERGENCY_STOP após
         *     emergency_threshold_ms (timer recomeça com degraded_since=now)
         *
         * IMPORTANTE: só aplicar se ainda estamos em EMERGENCY_STOP — se
         * o bloco anterior já fez recovery via lane_ok, não sobrescrever.
         */
        if (adas_state == AdasState::EMERGENCY_STOP) {
            auto emergency_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - emergency_since).count();
            if (emergency_ms >= cfg.emergency_timeout_ms) {
                adas_state      = AdasState::DEGRADED;
                degraded_since  = now;
                recovery_frames = 0;
                if (estop_sent) {
                    can.send_estop(0);
                    estop_sent = false;
                }
                printf("[ADAS] EMERGENCY_STOP timeout %lds → DEGRADED"
                       " (lane=%s — recovery via timeout)\n",
                       (long)(cfg.emergency_timeout_ms / 1000),
                       lane_ok ? "OK" : "lost");
            }
        }
    } else {
        if (lane_ok) {
            degraded_frames = 0;
            recovery_frames = 0;
            adas_state      = AdasState::ACTIVE;
        } else {
            degraded_frames++;
            if (degraded_frames >= cfg.degraded_threshold_frames) {
                if (adas_state != AdasState::DEGRADED) {
                    adas_state     = AdasState::DEGRADED;
                    degraded_since = now;
                    // reset_soft: mantém last_steering_ — DEGRADED conduz com
                    // lka.last_steering(), e zerá-lo fazia as rodas saltar
                    // para o centro a meio da saída de curva (zigzag pós-curva)
                    lka.reset_soft();
                    printf("[ADAS] → DEGRADED\n");
                }
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - degraded_since).count();
                if (ms >= cfg.emergency_threshold_ms) {
                    adas_state      = AdasState::EMERGENCY_STOP;
                    emergency_since = now;
                    recovery_frames = 0;
                    printf("[ADAS] → EMERGENCY_STOP\n");
                    if (!estop_sent) {
                        can.send_estop(1, 0x10);
                        estop_sent = true;
                    }
                }
            }
        }
    }
}

// ── Drive mode toggle ─────────────────────────────────────────────────────────
static void change_drive_mode(
    DriveMode& drive_mode,
    AdasState& adas_state,
    int& degraded_frames,
    int& recovery_frames,
    LKAController& lka,
    OAController& oa,
    KuksaBridge& bridge)
{
    drive_mode = (drive_mode == DriveMode::MANUAL)
                 ? DriveMode::AUTONOMOUS : DriveMode::MANUAL;

    adas_state      = AdasState::INIT;
    degraded_frames = 0;
    recovery_frames = 0;
    oa.reset();
    if (drive_mode == DriveMode::AUTONOMOUS)
        lka.reset();

    printf("[ADAS] Drive mode → %s\n",
           drive_mode == DriveMode::MANUAL ? "MANUAL" : "AUTONOMOUS");
    bridge.pub_mode(drive_mode);
}
