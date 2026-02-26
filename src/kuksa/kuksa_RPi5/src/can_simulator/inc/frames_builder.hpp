#include <cmath>

#include "../inc/helpers.hpp"
#include "../inc/sim_state.hpp"
#include "../inc/app_context.hpp"

WheelSpeed_t make_wheel(const SimState& st);
Heartbeat_t make_hb(const SimState& st, uint32_t uptime_ms);
Environment_t make_env(const SimState& st);
BatteryStatus_t make_batt(const SimState& st);
ImuAccel_t make_accel(const SimState& st);
ImuGyro_t make_gyro(const SimState& st);
ImuMag_t make_mag(const SimState& st);
ToFDistance_t make_tof(const SimState& st);
EmergencyStop_t make_estop(const SimState& st);
MotorStatus_t make_motor_status(const SimState& st, uint8_t counter);