"""MicroPython firmware for a micro:bit-controlled blinking street light.

Flash this file to the street-light micro:bit as main.py.
Supported commands over serial (newline-terminated):
- PING   -> replies ACK PONG
- ON     -> turns lights on steadily
- OFF    -> turns lights off
- BLINK  -> toggles all configured outputs on a timer
- STATUS -> replies current state

This version drives pin0, pin1 and pin2 together so it can be used with a
simple lamp strip or multiple LEDs.
"""

from microbit import *

STATE = "OFF"
LIGHT_PINS = [pin0, pin1, pin2]
BLINK_INTERVAL_MS = 400
last_toggle_ms = running_time()
blink_on = False


def send_line(text):
    print(text)


def write_outputs(value):
    for pin in LIGHT_PINS:
        pin.write_digital(value)


def show_state(state):
    if state == "ON":
        display.show(Image.ASLEEP)
    elif state == "BLINK":
        display.show(Image.DIAMOND)
    else:
        display.clear()


def set_state(new_state):
    global STATE, blink_on, last_toggle_ms
    STATE = new_state
    last_toggle_ms = running_time()
    if STATE == "ON":
        blink_on = True
        write_outputs(1)
    elif STATE == "OFF":
        blink_on = False
        write_outputs(0)
    else:
        blink_on = False
        write_outputs(0)
    show_state(STATE)


set_state("OFF")
send_line("BOOT OK")

while True:
    if STATE == "BLINK":
        now = running_time()
        if now - last_toggle_ms >= BLINK_INTERVAL_MS:
            blink_on = not blink_on
            write_outputs(1 if blink_on else 0)
            last_toggle_ms = now

    if uart.any():
        try:
            cmd = input().strip().upper()
        except Exception:
            send_line("ERR decode")
            sleep(20)
            continue

        if cmd == "PING":
            send_line("ACK PONG")
        elif cmd == "ON":
            set_state("ON")
            send_line("ACK ON")
        elif cmd == "OFF":
            set_state("OFF")
            send_line("ACK OFF")
        elif cmd == "BLINK":
            set_state("BLINK")
            send_line("ACK BLINK")
        elif cmd == "STATUS":
            send_line("ACK STATE " + STATE)
        else:
            send_line("ERR unknown")

    sleep(20)
