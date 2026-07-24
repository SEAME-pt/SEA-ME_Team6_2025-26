/**
 * Shared micro:bit gateway / relay firmware
 * Used by both V2I and emergency scenarios.
 * Flash via: https://makecode.microbit.org  (paste as JavaScript, not Blocks)
 * Radio group: 23
 *
 * Pi -> USB serial -> Gateway -> radio:
 *   TL RED/YELLOW/GREEN  -> TL_CMD_R / TL_CMD_Y / TL_CMD_G
 *   BAR OPEN/CLOSE       -> BAR_CMD_OPEN / BAR_CMD_CLOSE
 *   LGT ON/OFF/BLINK     -> LGT_CMD_ON / LGT_CMD_OFF / LGT_CMD_BLINK
 *
 * Radio -> Gateway -> USB serial:
 *   TL_STATE:R/Y/G, BAR_STATE:OPEN/CLOSED, LGT_STATE:ON/OFF/BLINK
 */

radio.setGroup(23)
serial.redirectToUSB()
serial.setBaudRate(BaudRate.BaudRate115200)

let lastTrafficLightState = "R"
let lastBarrierState = "CLOSED"
let lastStreetlightState = "OFF"

function writeStatus(prefix: string, value: string) {
    serial.writeLine(prefix + value)
}

function sendRadioForCommand(cmd: string) {
    if (cmd == "TL RED") {
        radio.sendString("TL_CMD_R")
        serial.writeLine("ACK TL RED")
    } else if (cmd == "TL YELLOW") {
        radio.sendString("TL_CMD_Y")
        serial.writeLine("ACK TL YELLOW")
    } else if (cmd == "TL GREEN") {
        radio.sendString("TL_CMD_G")
        serial.writeLine("ACK TL GREEN")
    } else if (cmd == "BAR OPEN") {
        radio.sendString("BAR_CMD_OPEN")
        serial.writeLine("ACK BAR OPEN")
    } else if (cmd == "BAR CLOSE") {
        radio.sendString("BAR_CMD_CLOSE")
        serial.writeLine("ACK BAR CLOSE")
    } else if (cmd == "LGT ON") {
        radio.sendString("LGT_CMD_ON")
        serial.writeLine("ACK LGT ON")
    } else if (cmd == "LGT OFF") {
        radio.sendString("LGT_CMD_OFF")
        serial.writeLine("ACK LGT OFF")
    } else if (cmd == "LGT BLINK") {
        radio.sendString("LGT_CMD_BLINK")
        serial.writeLine("ACK LGT BLINK")
    } else if (cmd == "STATUS") {
        writeStatus("TL_STATE:", lastTrafficLightState)
        writeStatus("BAR_STATE:", lastBarrierState)
        writeStatus("LGT_STATE:", lastStreetlightState)
    }
}

radio.onReceivedString(function (msg) {
    if (msg.indexOf("TL_STATE:") == 0) {
        lastTrafficLightState = msg.substr(9)
        serial.writeLine(msg)
    } else if (msg.indexOf("BAR_STATE:") == 0) {
        lastBarrierState = msg.substr(10)
        serial.writeLine(msg)
    } else if (msg.indexOf("LGT_STATE:") == 0) {
        lastStreetlightState = msg.substr(10)
        serial.writeLine(msg)
    }
})

serial.onDataReceived(serial.delimiters(Delimiters.NewLine), function () {
    sendRadioForCommand(serial.readLine().trim().toUpperCase())
})

basic.forever(function () {
    serial.writeLine("GW_HEARTBEAT")
    basic.pause(100)
})

basic.showString("GW")
serial.writeLine("GW_BOOT_OK")
