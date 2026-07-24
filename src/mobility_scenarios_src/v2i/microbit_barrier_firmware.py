"""MicroPython firmware for a micro:bit-controlled barrier servo.

Flash this file to the barrier micro:bit as main.py.
Supported commands over serial (newline-terminated):
- PING   -> replies ACK PONG
- OPEN   -> moves servo to open position
- CLOSE  -> moves servo to closed position
- STATUS -> replies current state

This firmware uses pin0 as the servo signal line.
Adjust OPEN_US and CLOSED_US if your hardware geometry differs.
"""

from microbit import *

STATE = "CLOSED"
SERVO_PIN = pin0
SERVO_PERIOD_MS = 20
CLOSED_US = 600
OPEN_US = 2400


def send_line(text):
    print(text)


def set_servo_us(pulse_us):
    analog = int(max(0, min(1023, (pulse_us / 20000.0) * 1023)))
    SERVO_PIN.set_analog_period(SERVO_PERIOD_MS)
    SERVO_PIN.write_analog(analog)


def show_state(state):
    if state == "OPEN":
        display.show(Image.YES)
    else:
        display.show(Image.NO)


def apply_state(state):
    global STATE
    STATE = state
    if STATE == "OPEN":
        set_servo_us(OPEN_US)
    else:
        set_servo_us(CLOSED_US)
    show_state(STATE)


apply_state("CLOSED")
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
    elif cmd == "OPEN":
        apply_state("OPEN")
        send_line("ACK OPEN")
    elif cmd == "CLOSE":
        apply_state("CLOSED")
        send_line("ACK CLOSE")
    elif cmd == "STATUS":
        send_line("ACK STATE " + STATE)
    else:
        send_line("ERR unknown")
