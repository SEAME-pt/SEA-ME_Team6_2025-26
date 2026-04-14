#include <cerrno>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>

#include "../inc/sim_state.hpp"
#include "../inc/app_context.hpp"
#include "../inc/helpers.hpp"
#include "../inc/scenario_loader.hpp"

void print_help();
void cmd_list(AppContext& ctx);
void cmd_play(const std::string& arg, AppContext& ctx);
void cmd_stop(AppContext& ctx);
void cmd_loop(const std::string& arg, AppContext& ctx);
void cmd_status(AppContext& ctx);
void cmd_show(AppContext& ctx);
void cmd_reset(AppContext& ctx);
void cmd_reload(AppContext& ctx);
void cli_loop(AppContext& ctx);