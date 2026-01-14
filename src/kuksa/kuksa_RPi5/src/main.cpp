#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include "../inc/kuksa_client.hpp"
#include "../inc/dispatch_frames.hpp"

static int open_can_socket(const std::string& ifname)
{
    int s = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        std::cerr << "[CAN] socket() failed: " << std::strerror(errno) << "\n";
        return -1;
    }

    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ - 1);

    if (::ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
        std::cerr << "[CAN] ioctl(SIOCGIFINDEX) failed: " << std::strerror(errno) << "\n";
        ::close(s);
        return -1;
    }

    struct sockaddr_can addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (::bind(s, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "[CAN] bind() failed: " << std::strerror(errno) << "\n";
        ::close(s);
        return -1;
    }

    // Optional: receive only data frames (no error frames)
    // can_err_mask_t err_mask = 0;
    // setsockopt(s, SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &err_mask, sizeof(err_mask));

    std::cout << "[CAN] Listening on interface: " << ifname << "\n";
    return s;
}

int main(int argc, char** argv)
{
    std::string can_if   = "can1";
    std::string kuksa_at = "127.0.0.1:55555";
    //std::string kuksa_at = "0.0.0.0:55555";

    if (argc >= 2) can_if = argv[1];
    if (argc >= 3) kuksa_at = argv[2];

    // 1) Open CAN socket
    const int can_sock = open_can_socket(can_if);
    if (can_sock < 0) return 1;

    // 2) Connect to KUKSA
    try {
        KuksaClient kuksa(kuksa_at);
        std::cout << "[KUKSA] Target: " << kuksa_at << "\n";

        // 3) Read loop
        while (true) {
            struct can_frame frame;
            const ssize_t n = ::read(can_sock, &frame, sizeof(frame));

            if (n < 0) {
                if (errno == EINTR) continue;
                std::cerr << "[CAN] read() failed: " << std::strerror(errno) << "\n";
                break;
            }

            if (n != static_cast<ssize_t>(sizeof(frame))) {
                std::cerr << "[CAN] Short read: " << n << " bytes\n";
                continue;
            }

            // 4) Dispatch to correct handler
            dispatch_can_frame(frame, kuksa);
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        ::close(can_sock);
        return 1;
    }

    ::close(can_sock);
    return 0;
}
