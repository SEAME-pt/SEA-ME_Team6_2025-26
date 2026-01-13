#pragma once
#include <cstdint>
#include <linux/can.h> // struct can_frame

// Speedometer constants
#define PI 3.14159265
#define WHEEL_DIAMETER 0.06675f  // Em metros (66.75mm)
#define ENCODER_HOLES 18
constexpr double WHEEL_PERIMETER = PI * WHEEL_DIAMETER;