/**
 * micro:bit D — Streetlight radio firmware
 * Flash via: https://makecode.microbit.org  (paste as JavaScript, not Blocks)
 * Radio group: 23
 *
 * Accepts from gateway:
 *   LGT_CMD_ON
 *   LGT_CMD_OFF
 *   LGT_CMD_BLINK
 *
 * Publishes to gateway:
 *   LGT_STATE:ON / LGT_STATE:OFF / LGT_STATE:BLINK
 */

radio.setGroup(23)

let state = "OFF"
let blinkOn = false
let lastToggleMs = input.runningTime()
const LIGHT_PINS = [DigitalPin.P0, DigitalPin.P1, DigitalPin.P2]
const BLINK_INTERVAL_MS = 400

function writeOutputs(value: number) {
    for (let pin of LIGHT_PINS) {
        pins.digitalWritePin(pin, value)
    }
}

function publishState() {
    radio.sendString("LGT_STATE:" + state)
}

function applyState(newState: string) {
    state = newState
    lastToggleMs = input.runningTime()

    if (state == "ON") {
        blinkOn = true
        writeOutputs(1)
        basic.showIcon(IconNames.Asleep)
    } else if (state == "BLINK") {
        blinkOn = false
        writeOutputs(0)
        basic.showIcon(IconNames.Diamond)
    } else {
        blinkOn = false
        writeOutputs(0)
        basic.clearScreen()
    }

    publishState()
}

input.onButtonPressed(Button.A, function () {
    applyState("ON")
})
input.onButtonPressed(Button.B, function () {
    applyState("OFF")
})
input.onButtonPressed(Button.AB, function () {
    applyState("BLINK")
})

radio.onReceivedString(function (msg) {
    if (msg == "LGT_CMD_ON") {
        applyState("ON")
    } else if (msg == "LGT_CMD_OFF") {
        applyState("OFF")
    } else if (msg == "LGT_CMD_BLINK") {
        applyState("BLINK")
    }
})

applyState("OFF")

basic.forever(function () {
    if (state == "BLINK") {
        let now = input.runningTime()
        if (now - lastToggleMs >= BLINK_INTERVAL_MS) {
            blinkOn = !blinkOn
            writeOutputs(blinkOn ? 1 : 0)
            lastToggleMs = now
        }
    }
    publishState()
    basic.pause(1000)
})