#include "../inc/scenario_loader.hpp"

Scenario load_scenario_file(const fs::path& p)
{
  Scenario sc;
  sc.name = p.filename().string();
  sc.path = p.string();

  std::ifstream in(sc.path.c_str());
  if (!in) throw std::runtime_error("Failed to open scenario: " + sc.path);

  std::string line;
  uint32_t ln = 0;

  while (std::getline(in, line)) {
    ln++;
    line = trim(line);
    if (line.empty() || line[0] == '#') continue;

    // @init ...
    if (line.rfind("@init", 0) == 0) {
      std::istringstream iss(line.substr(5));
      std::string tok;
      while (iss >> tok) {
        std::string k,v;
        if (split_kv(tok, k, v)) sc.init_kv.push_back({k,v});
      }
      continue;
    }

    std::istringstream iss(line);
    std::string tok;
    Event ev;
    bool has_t = false;

    while (iss >> tok) {
      std::string k, v;
      if (!split_kv(tok, k, v)) continue;

      if (k == "t" || k == "t_ms") {
        ev.t_ms = static_cast<uint32_t>(std::strtoul(v.c_str(), nullptr, 10));
        has_t = true;
      } else {
        ev.kv.push_back({k, v});
      }
    }

    if (!has_t) {
      std::cerr << "[EMU] " << sc.name << ": line " << ln << ": missing t=..., skipping\n";
      continue;
    }
    sc.events.push_back(ev);
  }

  std::sort(sc.events.begin(), sc.events.end(),
            [](const Event& a, const Event& b){ return a.t_ms < b.t_ms; });

  if (!sc.events.empty()) sc.duration_ms = sc.events.back().t_ms;
  return sc;
}

std::vector<Scenario> load_scenarios_dir(const std::string& dir)
{
  std::vector<Scenario> out;
  fs::path root(dir);
  if (!fs::exists(root)) throw std::runtime_error("Scenario dir does not exist: " + dir);
  if (!fs::is_directory(root)) throw std::runtime_error("Not a directory: " + dir);

  for (auto const& ent : fs::directory_iterator(root)) {
    if (!ent.is_regular_file()) continue;
    const fs::path p = ent.path();
    const std::string ext = p.extension().string();
    if (ext != ".txt" && ext != ".scn" && ext != ".scenario") continue;

    try {
      Scenario sc = load_scenario_file(p);
      if (sc.events.empty() && sc.init_kv.empty()) {
        std::cerr << "[EMU] Skipping empty scenario: " << sc.name << "\n";
        continue;
      }
      out.push_back(sc);
    } catch (const std::exception& e) {
      std::cerr << "[EMU] Failed to load " << p.filename().string() << ": " << e.what() << "\n";
    }
  }

  std::sort(out.begin(), out.end(), [](const Scenario& a, const Scenario& b){
    return a.name < b.name;
  });

  return out;
}

// Apply init + any t=0 events to current state (caller holds g_state_mtx)
void apply_scenario_start_locked(AppContext& ctx, const Scenario& sc)
{
  ctx.state = ctx.defaults;
  for (const auto& kv : sc.init_kv) apply_kv(ctx.state, kv.first, kv.second);
  // Apply t=0 events
  for (const auto& ev : sc.events) {
    if (ev.t_ms != 0) break;
    for (const auto& kv : ev.kv) apply_kv(ctx.state, kv.first, kv.second);
  }
}
