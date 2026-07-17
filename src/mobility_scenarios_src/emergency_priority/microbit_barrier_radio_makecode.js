/**
 * micro:bit C — Barrier radio firmware
 * Flash via: https://makecode.microbit.org  (paste as JavaScript, not Blocks)
 * Radio group: 23
 *
 * Accepts from gateway:
 *   BAR_CMD_OPEN
 *   BAR_CMD_CLOSE
 *
 * Publishes to gateway:
 *   BAR_STATE:OPEN / BAR_STATE:CLOSED
 *
 * Hardware note:
 *   This assumes a servo on P0.
 */

radio.setGroup(23)

let state = "CLOSED"
const SERVO_PIN = AnalogPin.P0
const OPEN_ANGLE = 20
const CLOSED_ANGLE = 160
let invertDirection = false

function publishState() {
    radio.sendString("BAR_STATE:" + state)
}

function applyState(newState: string) {
    state = newState
    let targetAngle = (state == "OPEN") ? OPEN_ANGLE : CLOSED_ANGLE
    if (invertDirection) {
        targetAngle = (state == "OPEN") ? CLOSED_ANGLE : OPEN_ANGLE
    }
    // Write once; MakeCode hardware PWM maintains the signal continuously without blocking.
    pins.servoWritePin(SERVO_PIN, targetAngle)
    if (state == "OPEN") {
        basic.showIcon(IconNames.Yes)
    } else {
        basic.showIcon(IconNames.No)
    }
    publishState()
}

input.onButtonPressed(Button.A, function () {
    applyState("OPEN")
})
input.onButtonPressed(Button.B, function () {
    applyState("CLOSED")
})

input.onButtonPressed(Button.AB, function () {
    applyState(state == "OPEN" ? "CLOSED" : "OPEN")
})

// Long press logo flips open/close direction for inverted mechanics.
input.onLogoEvent(TouchButtonEvent.LongPressed, function () {
    invertDirection = !invertDirection
    basic.showString(invertDirection ? "INV1" : "INV0")
    applyState(state)
})

radio.onReceivedString(function (msg) {
    if (msg == "BAR_CMD_OPEN") {
        applyState("OPEN")
    } else if (msg == "BAR_CMD_CLOSE") {
        applyState("CLOSED")
    }
})

applyState("CLOSED")
basic.showString("BAR")

basic.forever(function () {
    publishState()
    basic.pause(1000)
})