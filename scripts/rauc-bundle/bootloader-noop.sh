#!/bin/bash
# No-op bootloader backend for tar/raw app-only slot.
# RAUC requires bootloader= even when no boot switching is needed.
case "$1" in
    get-primary)           echo "A" ;;
    get-state)             echo "good" ;;
    set-primary)           exit 0 ;;
    set-state)             exit 0 ;;
    get-remaining-attempts) echo "3" ;;
    *) exit 1 ;;
esac
