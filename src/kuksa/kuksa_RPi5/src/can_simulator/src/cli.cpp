#include "../inc/cli.hpp"

// -----------------------------
// CLI
// -----------------------------
void print_help()
{
  std::cout <<
    "Commands:\n"
    "  help\n"
    "  list\n"
    "  play <index|name>\n"
    "  stop\n"
    "  loop on|off\n"
    "  status\n"
    "  show\n"
    "  reset\n"
    "  reload\n"
    "  quit\n";
}

void cmd_list(AppContext& ctx)
{
  if (ctx.scenarios.empty()) {
    std::cout << "(no scenarios loaded)\n";
    return;
  }
  for (size_t i = 0; i < ctx.scenarios.size(); ++i) {
    std::cout << i << ") " << ctx.scenarios[i].name
              << "  (events=" << ctx.scenarios[i].events.size()
              << ", duration=" << ctx.scenarios[i].duration_ms << "ms)\n";
  }
}

bool parse_index_or_name(AppContext& ctx, const std::string& s, size_t& out_idx)
{
  // try integer
  char* end = nullptr;
  long v = std::strtol(s.c_str(), &end, 10);
  if (end && *end == '\0') {
    if (v < 0) return false;
    out_idx = static_cast<size_t>(v);
    return true;
  }

  // try name match
  for (size_t i = 0; i < ctx.scenarios.size(); ++i) {
    if (ctx.scenarios[i].name == s) { out_idx = i; return true; }
  }
  return false;
}

void cmd_play(const std::string& arg, AppContext& ctx)
{
  if (ctx.scenarios.empty()) {
    std::cout << "No scenarios loaded. Use 'reload' or check the folder.\n";
    return;
  }

  size_t idx = 0;
  if (!parse_index_or_name(ctx, arg, idx) || idx >= ctx.scenarios.size()) {
    std::cout << "Unknown scenario: " << arg << "\n";
    return;
  }

  const Scenario& sc = ctx.scenarios[idx];

  {
    std::lock_guard<std::mutex> lk_state(ctx.state_mtx);
    apply_scenario_start_locked(ctx, sc);
  }

  {
    std::lock_guard<std::mutex> lk(ctx.play_mtx);
    ctx.play.playing = true;
    ctx.play.scenario_index = idx;
    ctx.play.next_event_idx = 0;
    ctx.play.start_ms = now_ms();
  }

  std::cout << "[OK] Playing " << sc.name << " (loop=" << (ctx.play.loop ? "on" : "off") << ")\n";
}

void cmd_stop(AppContext& ctx)
{
  std::lock_guard<std::mutex> lk(ctx.play_mtx);
  ctx.play.playing = false;
  std::cout << "[OK] Stopped\n";
}

void cmd_loop(const std::string& arg, AppContext& ctx)
{
  bool on = (arg == "on" || arg == "1" || arg == "true");
  {
    std::lock_guard<std::mutex> lk(ctx.play_mtx);
    ctx.play.loop = on;
  }
  std::cout << "[OK] loop=" << (on ? "on" : "off") << "\n";
}

void cmd_status(AppContext& ctx)
{
  Playback pb;
  {
    std::lock_guard<std::mutex> lk(ctx.play_mtx);
    pb = ctx.play;
  }

  std::cout << "playing=" << (pb.playing ? "yes" : "no")
            << " loop=" << (pb.loop ? "on" : "off");

  if (pb.playing && !ctx.scenarios.empty() && pb.scenario_index < ctx.scenarios.size()) {
    std::cout << " scenario=" << ctx.scenarios[pb.scenario_index].name
              << " next_event_idx=" << pb.next_event_idx;
  }
  std::cout << "\n";
}

void cmd_show(AppContext& ctx)
{
  SimState st;
  {
    std::lock_guard<std::mutex> lk(ctx.state_mtx);
    st = ctx.state;
  }

  std::cout
    << "hb_state=" << (int)st.hb_state
    << " hb_errors=" << (int)st.hb_errors
    << " hb_mode=" << (int)st.hb_mode
    << " | estop_active=" << (int)st.estop_active
    << " source=" << (int)st.estop_source
    << " dist_mm=" << st.estop_mm
    << " reason=" << (int)st.estop_reason
    << " | rpm=" << st.rpm
    << " pulses=" << st.pulses
    << " dir=" << (int)st.direction
    << " | tof_mm=" << st.tof_mm
    << " zone=" << (int)st.tof_zone
    << " targets=" << (int)st.tof_count
    << " | env=" << st.env_temp_c << "C " << st.env_humidity << "% " << st.env_light_lux << "lux " << st.env_pressure_hpa << "hPa"
    << " | batt_mv=" << st.batt_mv << " soc=" << (int)st.batt_soc
    << " | motor thr=" << (int)st.motor_throttle << " steer=" << (int)st.motor_steering
    << "\n";
}

void cmd_reset(AppContext& ctx)
{
  {
    std::lock_guard<std::mutex> lk_state(ctx.state_mtx);
    ctx.state = ctx.defaults;
  }
  std::cout << "[OK] State reset to defaults\n";
}

void cmd_reload(AppContext& ctx)
{
  try {
    ctx.scenarios = load_scenarios_dir(ctx.scenario_dir);
    std::cout << "[OK] Reloaded scenarios (" << ctx.scenarios.size() << ") from " << ctx.scenario_dir << "\n";
  } catch (const std::exception& e) {
    std::cout << "[ERR] " << e.what() << "\n";
  }
}

void cli_loop(AppContext& ctx)
{
  print_help();
  std::string line;

  // Main CLI loop
  while (ctx.running.load()) {
    // Prompt
    std::cout << "emu> " << std::flush;
    // Read line
    if (!std::getline(std::cin, line)) {
      ctx.running.store(false);
      break;
    }

    // Trim and skip empty
    line = trim(line);
    if (line.empty()) continue;

    // Parse command
    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;

    // Handle commands
    if (cmd == "help") print_help();
    else if (cmd == "list") cmd_list(ctx);
    else if (cmd == "play") {
      std::string arg; iss >> arg;
      if (arg.empty()) {
        std::cout << "usage: play <index|name>\n";
      }
      else cmd_play(arg, ctx);
    }
    else if (cmd == "stop") cmd_stop(ctx);
    else if (cmd == "loop") {
      std::string arg; iss >> arg;
      if (arg.empty()) std::cout << "usage: loop on|off\n";
      else cmd_loop(arg, ctx);
    }
    else if (cmd == "status") cmd_status(ctx);
    else if (cmd == "show") cmd_show(ctx);
    else if (cmd == "reset") cmd_reset(ctx);
    else if (cmd == "reload") cmd_reload(ctx);
    else if (cmd == "quit" || cmd == "exit") {
      ctx.running.store(false);
      break;
    }
    else {
      std::cout << "Unknown command. Type 'help'.\n";
    }
  }
}