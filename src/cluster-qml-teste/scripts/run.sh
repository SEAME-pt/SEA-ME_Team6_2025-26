#!/bin/bash

#*For better commentary visualization, install extension: "Better Comments"

#? Other .sh files includes
    #* Progressbar originaly created by: https://github.com/lnfnunes/bash-progress-indicator
source "$(dirname "$0")/progressbar.sh"

#? ANSI color codes
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
BLUE="\033[34m"
BOLD="\033[1m"
RESET="\033[0m"

#? Variable definitions
PROJECT_DIR=$(pwd)
BUILD_DIR="$PROJECT_DIR/build"
APP_NAME="team6-cluster"
#QT_PATH="/usr/lib/x86_64-linux-gnu/cmake/Qt6"
QT_PATH=~/Qt/6.9.3/gcc_64/lib/cmake/Qt6

#? Header
echo -e "${BOLD}${YELLOW}================================================================${RESET}"
echo -e "${BLUE}                       QT Cluster Exec Script${RESET}"
echo -e "${BOLD}${YELLOW}================================================================${RESET}"

export PATH=~/Qt/6.9.3/gcc_64/bin:$PATH
export QML2_IMPORT_PATH=~/Qt/6.9.3/gcc_64/qml
export LD_LIBRARY_PATH=~/Qt/6.9.3/gcc_64/lib:$LD_LIBRARY_PATH

#? Steps and Commands definition
STEPS=(
    "Entering on Project Directory"
    "Checking for Code Updates"
    "Creating or Build Directory"
    "Entering on Build Directory"
    "Configuring CMake"
	"Compiling App"
)

CMDS=(
    "cd $PROJECT_DIR"
    "git pull origin development"
    "mkdir -p $BUILD_DIR"
    "cd $BUILD_DIR"
    "cmake .. -DCMAKE_PREFIX_PATH=$QT_PATH .."
	"make -j$(nproc) "
)

start_PB

echo -e "${BOLD}${YELLOW}================================================================${RESET}"
echo -e "${BLUE}                QT Cluster Exec Script Completed!${RESET}"
echo -e "${BOLD}${YELLOW}================================================================${RESET}"
echo ""
echo -e "${BLUE}Launching Cluster...${RESET}"

./$APP_NAME


