#!/usr/bin/env bash
set -euo pipefail

SERVICES=(
  adas-normal-stack-mod11.service
  adas-v2i-stack-mod11.service
  adas-emergency-stack-mod11.service
)

stop_optional_adas_manager() {
  local load_state
  load_state="$(systemctl show adas-manager.service -p LoadState --value 2>/dev/null || true)"
  if [[ "$load_state" != "not-found" && -n "$load_state" ]]; then
    systemctl stop adas-manager.service || true
  fi
}

kill_orphan_stack_processes() {
  pkill -x adas_manager || true
  pkill -f '/joystick_control\.py' || true
  pkill -f '/kuksa_bridge\.py' || true
  pkill -f '/roadside_scenario_runtime\.py' || true
  pkill -f '/roadside_emergency_controller\.py' || true
}

stop_all() {
  echo "[stack-switch-mod11] a parar todas as stacks"
  systemctl stop "${SERVICES[@]}" || true
  stop_optional_adas_manager
  kill_orphan_stack_processes
}

show_status() {
  echo "== status mod11 =="
  for s in "${SERVICES[@]}"; do
    printf "[%s] " "$s"
    systemctl is-active "$s" 2>/dev/null || true
  done
}

print_compact_service_state() {
  local s="$1"
  local active sub pid
  active="$(systemctl is-active "$s" 2>/dev/null || true)"
  sub="$(systemctl show "$s" -p SubState --value 2>/dev/null || true)"
  pid="$(systemctl show "$s" -p MainPID --value 2>/dev/null || true)"
  echo "[$s] active=$active sub=$sub pid=$pid"
}

start_normal() {
  stop_all
  echo "[stack-switch-mod11] a arrancar NORMAL"
  systemctl start adas-normal-stack-mod11.service
  print_compact_service_state adas-normal-stack-mod11.service
}

start_v2i() {
  stop_all
  echo "[stack-switch-mod11] a arrancar V2I"
  systemctl start adas-v2i-stack-mod11.service
  print_compact_service_state adas-v2i-stack-mod11.service
}

start_emergency() {
  stop_all
  echo "[stack-switch-mod11] a arrancar EMERGENCY"
  systemctl start adas-emergency-stack-mod11.service
  print_compact_service_state adas-emergency-stack-mod11.service
}

usage() {
  echo "usage: $0 {normal|v2i|emergency|stop|status}"
  exit 1
}

cmd="${1:-}"
case "$cmd" in
  normal)
    start_normal
    ;;
  v2i)
    start_v2i
    ;;
  emergency)
    start_emergency
    ;;
  stop)
    stop_all
    show_status
    ;;
  status)
    show_status
    ;;
  *)
    usage
    ;;
esac
