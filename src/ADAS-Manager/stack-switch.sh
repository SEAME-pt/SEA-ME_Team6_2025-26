#!/usr/bin/env bash
set -euo pipefail

SERVICES=(
  adas-normal-stack.service
  adas-v2i-stack.service
  adas-emergency-stack.service
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
  echo "[stack-switch] a parar todas as stacks"
  systemctl stop "${SERVICES[@]}" || true
  stop_optional_adas_manager
  kill_orphan_stack_processes
}

show_status() {
  echo "== status =="
  systemctl --no-pager --full status adas-normal-stack.service adas-v2i-stack.service adas-emergency-stack.service || true
}

start_normal() {
  stop_all
  echo "[stack-switch] a arrancar NORMAL"
  systemctl start adas-normal-stack.service
  systemctl --no-pager --full status adas-normal-stack.service
}

start_v2i() {
  stop_all
  echo "[stack-switch] a arrancar V2I"
  systemctl start adas-v2i-stack.service
  systemctl --no-pager --full status adas-v2i-stack.service
}

start_emergency() {
  stop_all
  echo "[stack-switch] a arrancar EMERGENCY"
  systemctl start adas-emergency-stack.service
  systemctl --no-pager --full status adas-emergency-stack.service
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
