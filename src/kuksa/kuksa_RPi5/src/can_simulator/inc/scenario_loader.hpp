#include <vector>
#include <fstream>
#if __has_include(<filesystem>)
  #include <filesystem>
  namespace fs = std::filesystem;
#else
  #error "C++17 <filesystem> required"
#endif

#include "app_context.hpp"
#include "helpers.hpp"

std::vector<Scenario> load_scenarios_dir(const std::string& dir);
Scenario load_scenario_file(const fs::path& p);
void apply_scenario_start_locked(AppContext& ctx, const Scenario& sc);