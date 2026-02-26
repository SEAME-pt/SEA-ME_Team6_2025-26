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

// -----------------------------
// CAN sender thread
// -----------------------------

void can_sender_loop(int sock, AppContext& ctx)
{
  uint64_t t0 = now_ms();

  uint64_t next_motor = t0;
  uint64_t next_imu   = t0;
  uint64_t next_mag   = t0;
  uint64_t next_wheel = t0;
  uint64_t next_tof   = t0;
  uint64_t next_env   = t0;
  uint64_t next_batt  = t0;
  uint64_t next_hb    = t0;
  uint64_t next_estop = t0;

  uint8_t motor_counter = 0;

  while (ctx.running.load()) {
    const uint64_t now = now_ms();
    const uint32_t uptime = static_cast<uint32_t>(now - t0);

    // snapshot the latest state (under lock)
    SimState st;
    {
      std::lock_guard<std::mutex> lk(ctx.play_mtx);
      st = ctx.state;
    }
    // snapshot the periods (no lock needed since they are const)
    const Periods& P = ctx.periods;

    // Send frames - check each one
    // if the time passed is equal or greater than the scheduled
    if (now >= next_motor) {
      MotorStatus_t m = make_motor_status(st, motor_counter++);
      send_payload8(sock, CAN_ID_MOTOR_STATUS, &m);
      next_motor += P.motor;
    }

    if (now >= next_imu) {
      ImuAccel_t a = make_accel(st);
      ImuGyro_t  g = make_gyro(st);
      send_payload8(sock, CAN_ID_IMU_ACCEL, &a);
      send_payload8(sock, CAN_ID_IMU_GYRO,  &g);
      next_imu += P.imu_fast;
    }

    if (now >= next_mag) {
      ImuMag_t m = make_mag(st);
      send_payload8(sock, CAN_ID_IMU_MAG, &m);
      next_mag += P.imu_mag;
    }

    if (now >= next_wheel) {
      WheelSpeed_t w = make_wheel(st);
      send_payload8(sock, CAN_ID_WHEEL_SPEED, &w);
      next_wheel += P.wheel;
    }

    if (now >= next_tof) {
      ToFDistance_t t = make_tof(st);
      send_payload8(sock, CAN_ID_TOF_DISTANCE, &t);
      next_tof += P.tof;
    }

    if (now >= next_env) {
      Environment_t e = make_env(st);
      send_payload8(sock, CAN_ID_ENVIRONMENT, &e);
      next_env += P.env;
    }

    if (now >= next_batt) {
      BatteryStatus_t b = make_batt(st);
      send_payload8(sock, CAN_ID_BATTERY, &b);
      next_batt += P.batt;
    }

    if (now >= next_hb) {
      Heartbeat_t h = make_hb(st, uptime);
      send_payload8(sock, CAN_ID_HEARTBEAT_STM32, &h);
      next_hb += P.hb;
    }

    if (now >= next_estop) {
      EmergencyStop_t e = make_estop(st);
      send_payload8(sock, CAN_ID_EMERGENCY_STOP, &e);
      next_estop += P.estop;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }
}