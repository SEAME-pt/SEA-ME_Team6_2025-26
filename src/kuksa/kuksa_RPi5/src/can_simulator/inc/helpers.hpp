#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <chrono>
#include <iostream>

#include "../inc/sim_state.hpp"

uint8_t crc8(const uint8_t* data, size_t len);
int16_t clamp_i16(long v);
uint8_t clamp_u8(long v);
std::string trim(const std::string& s);
bool split_kv(const std::string& token, std::string& k, std::string& v);
void apply_kv(SimState& st, const std::string& k, const std::string& v);
uint64_t now_ms();
