#include <thread>
#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include <mutex>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include "../../../inc/can_id.h"
#include "../inc/frames_builder.hpp"

int open_can_tx_socket(const std::string& ifname);
bool send_payload8(int sock, uint32_t can_id, const void* payload8);
void can_sender_loop(int sock, AppContext& ctx);