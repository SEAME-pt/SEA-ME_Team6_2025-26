#include "../inc/can_sender.hpp"

// -----------------------------
// SocketCAN helpers
// -----------------------------
int open_can_tx_socket(const std::string& ifname)
{
  int s = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (s < 0) {
    std::cerr << "[EMU] socket() failed: " << std::strerror(errno) << "\n";
    return -1;
  }

  ifreq ifr{};
  std::strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ - 1);

  if (::ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
    std::cerr << "[EMU] ioctl(SIOCGIFINDEX) failed: " << std::strerror(errno) << "\n";
    ::close(s);
    return -1;
  }

  sockaddr_can addr{};
  addr.can_family  = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;

  if (::bind(s, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
    std::cerr << "[EMU] bind() failed: " << std::strerror(errno) << "\n";
    ::close(s);
    return -1;
  }

  std::cout << "[EMU] Sending on interface: " << ifname << "\n";
  return s;
}

bool send_payload8(int sock, uint32_t can_id, const void* payload8)
{
  can_frame f{};
  f.can_id  = can_id;
  f.can_dlc = 8;
  std::memcpy(f.data, payload8, 8);
  const ssize_t n = ::write(sock, &f, sizeof(f));
  if (n != static_cast<ssize_t>(sizeof(f))) {
    std::cerr << "[EMU] write() failed/short: " << std::strerror(errno) << "\n";
    return false;
  }
  return true;
}