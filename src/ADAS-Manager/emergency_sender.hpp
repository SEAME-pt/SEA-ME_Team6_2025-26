#pragma once

#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

// Envia o estado do toggle de emergência pro roadside_emergency_controller.py
// (src/mobility_scenarios_src/emergency_priority/). Fire-and-forget — se o
// controller não estiver a correr, sendto falha em silêncio (ENOENT), não
// bloqueia o loop de controlo.
static const char* EMERGENCY_SOCKET = "/tmp/adas_emergency.sock";

class EmergencySender {
public:
    EmergencySender() {
        sock_ = socket(AF_UNIX, SOCK_DGRAM, 0);
    }

    ~EmergencySender() {
        if (sock_ >= 0) close(sock_);
    }

    void send(bool active) {
        if (sock_ < 0) return;
        const char* msg = active ? "1\n" : "0\n";

        struct sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, EMERGENCY_SOCKET, sizeof(addr.sun_path) - 1);

        sendto(sock_, msg, strlen(msg), 0,
               reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
    }

private:
    int sock_ = -1;
};
