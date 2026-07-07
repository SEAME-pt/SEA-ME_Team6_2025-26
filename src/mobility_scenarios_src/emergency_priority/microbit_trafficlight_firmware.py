"""MicroPython firmware for BBC micro:bit traffic-light serial control.

Flash this file to the micro:bit (as main.py).
Supported commands over serial (newline-terminated):
- PING   -> replies PONG
- RED    -> turns on red indication
- YELLOW -> turns on yellow indication
- GREEN  -> turns on green indication
- STATUS -> replies current state

The LED matrix is used as fallback indication, so this works even without
external LEDs connected.

This version uses USB serial stdin/stdout (`input`/`print`) so it can be
controlled directly from a host PC via `/dev/ttyACM*`.
"""

from microbit import *

STATE = "RED"
ACTIVE_LOW = False

# Typical Kitronik traffic light wiring on micro:bit.
RED_PIN = pin0
YELLOW_PIN = pin1
GREEN_PIN = pin2

# On this Kitronik setup, these lines can make all lamps turn on if pulled low.
# Keep them deasserted for stable behavior.
AUX_DISABLE_PINS = [pin8, pin12, pin16]

PIN_MAP = {
    "0": pin0,
    "1": pin1,
    "2": pin2,
    "8": pin8,
    "12": pin12,
    "16": pin16,
}


def show_state(state):
    if state == "RED":
        display.show(Image.NO)
    elif state == "YELLOW":
        display.show(Image.TRIANGLE)
    elif state == "GREEN":
        display.show(Image.YES)
    else:
        display.show("?")


def apply_light_outputs(state):
    red_on = state == "RED"
    yellow_on = state == "YELLOW"
    green_on = state == "GREEN"

    if ACTIVE_LOW:
        RED_PIN.write_digital(0 if red_on else 1)
        YELLOW_PIN.write_digital(0 if yellow_on else 1)
        GREEN_PIN.write_digital(0 if green_on else 1)
    else:
        RED_PIN.write_digital(1 if red_on else 0)
        YELLOW_PIN.write_digital(1 if yellow_on else 0)
        GREEN_PIN.write_digital(1 if green_on else 0)

    # Hold auxiliary lines in a safe state to avoid "all lights on" glitches.
    for p in AUX_DISABLE_PINS:
        p.write_digital(1)


def set_state(new_state):
    global STATE
    STATE = new_state
    show_state(STATE)
    apply_light_outputs(STATE)


def set_mode(mode):
    global ACTIVE_LOW
    if mode == "LOW":
        ACTIVE_LOW = True
        apply_light_outputs(STATE)
        send_line("ACK MODE LOW")
    elif mode == "HIGH":
        ACTIVE_LOW = False
        apply_light_outputs(STATE)
        send_line("ACK MODE HIGH")
    else:
        send_line("ERR mode")


def set_raw_pin(pin_name, value):
    pin_obj = PIN_MAP.get(pin_name)
    if pin_obj is None:
        send_line("ERR pin")
        return
    if value not in (0, 1):
        send_line("ERR value")
        return
    pin_obj.write_digital(value)
    send_line("ACK PIN " + pin_name + " " + str(value))


def send_line(text):
    print(text)


# Start in safe mode.
set_state("RED")
send_line("BOOT OK")

while True:
    try:
        cmd = input().strip().upper()
    except Exception:
        send_line("ERR decode")
        sleep(20)
        continue

    if cmd == "PING":
        send_line("ACK PONG")
    elif cmd == "RED":
        set_state("RED")
        send_line("ACK RED")
    elif cmd == "YELLOW":
        set_state("YELLOW")
        send_line("ACK YELLOW")
    elif cmd == "GREEN":
        set_state("GREEN")
        send_line("ACK GREEN")
    elif cmd == "STATUS":
        send_line("ACK STATE " + STATE + " MODE " + ("LOW" if ACTIVE_LOW else "HIGH"))
    elif cmd.startswith("MODE "):
        set_mode(cmd.split(" ", 1)[1].strip())
    elif cmd.startswith("PIN "):
        # Manual diagnostic command: PIN <num> <0|1>
        parts = cmd.split()
        if len(parts) != 3:
            send_line("ERR pincmd")
            continue
        try:
            value = int(parts[2])
        except Exception:
            send_line("ERR value")
            continue
        set_raw_pin(parts[1], value)
    else:
        send_line("ERR unknown")
