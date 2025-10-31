#!/bin/bash -e

declare -rx BAR_SIZE="##########"
declare -rx CLEAR_LINE="\\033[K"

start_PB() {
  local MAX_STEPS=${#STEPS[@]}
  local MAX_BAR_SIZE="${#BAR_SIZE}"

  # ANSI color codes
  local COLOR_FILLED="\033[1;32m"   # bold green
  local COLOR_EMPTY="\033[1;90m"    # bold gray
  local COLOR_STEP="\033[1;34m"     # bold blue for step name
  local COLOR_RESET="\033[0m"

  tput civis 2>/dev/null || true

  for step in "${!STEPS[@]}"; do
    # Show step name first
    echo -e "${COLOR_STEP}>> ${STEPS[$step]}${COLOR_RESET}"

    # Run the command
    ${CMDS[$step]}

    # Calculate progress
    perc=$(((step + 1) * 100 / MAX_STEPS))
    percBar=$((perc * MAX_BAR_SIZE / 100))

    # Construct colored progress bar
    filled="${BAR_SIZE:0:percBar}"
    empty="${BAR_SIZE:percBar}"

    # Print progress bar and percentage
    echo -ne "[${COLOR_FILLED}${filled}${COLOR_EMPTY}${empty}${COLOR_RESET}] "
    echo -e "${COLOR_FILLED}${perc}%${COLOR_RESET}$CLEAR_LINE"
  done

  tput cnorm 2>/dev/null || true
}



