/**
 * micro:bit A — Traffic Light TX (bidirectional)
 * Flash via: https://makecode.microbit.org  (paste as JavaScript, not Blocks)
 * Radio group: 23
 *
 * Sends:   "TL_STATE:R" / "TL_STATE:Y" / "TL_STATE:G"
 * Accepts: "TL_CMD_R"   / "TL_CMD_Y"   / "TL_CMD_G"
 *
 * Wiring (Kitronik traffic light board):
 *   P0 = RED
 *   P1 = YELLOW
 *   P2 = GREEN
 *
 * Manual operator control still works:
 *   Button A       -> RED
 *   Button B       -> YELLOW
 *   Button A+B     -> GREEN
 */

radio.setGroup(23)

let state = "R"
let activeLow = false

function pinWrite(pin: DigitalPin, on: boolean) {
    if (activeLow) {
        pins.digitalWritePin(pin, on ? 0 : 1)
    } else {
        pins.digitalWritePin(pin, on ? 1 : 0)
    }
}

function applyLights() {
    pinWrite(DigitalPin.P0, state == "R")
    pinWrite(DigitalPin.P1, state == "Y")
    pinWrite(DigitalPin.P2, state == "G")

    // Kitronik traffic-light boards can glitch to "all on" unless these are held high.
    pins.digitalWritePin(DigitalPin.P8, 1)
    pins.digitalWritePin(DigitalPin.P12, 1)
    pins.digitalWritePin(DigitalPin.P16, 1)
}

function setState(newState: string) {
    state = newState
    applyLights()
    radio.sendString("TL_STATE:" + state)
}

// Manual operator buttons (still work independently)
input.onButtonPressed(Button.A, function () {
    setState("R")
})
input.onButtonPressed(Button.B, function () {
    setState("Y")
})
input.onButtonPressed(Button.AB, function () {
    setState("G")
})

// Long-press LOGO toggles active-low mode for boards wired inversely.
input.onLogoEvent(TouchButtonEvent.LongPressed, function () {
    activeLow = !activeLow
    applyLights()
    basic.showString(activeLow ? "AL1" : "AL0")
})

// Remote commands from gateway (micro:bit B relaying Pi commands)
radio.onReceivedString(function (msg) {
    if (msg == "TL_CMD_R") {
        setState("R")
    } else if (msg == "TL_CMD_Y") {
        setState("Y")
    } else if (msg == "TL_CMD_G") {
        setState("G")
    }
})

// Startup: RED and broadcast initial state
setState("R")

// Heartbeat: re-broadcast state every 1 s so the gateway stays in sync
basic.forever(function () {
    radio.sendString("TL_STATE:" + state)
    basic.pause(1000)
})
