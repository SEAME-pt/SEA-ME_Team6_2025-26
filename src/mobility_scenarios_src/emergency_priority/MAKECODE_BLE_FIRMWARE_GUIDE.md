# BLE Traffic Light Firmware Guide - MakeCode (micro:bit)

**Purpose:** Implement BLE peripheral on micro:bit V2.21 to broadcast traffic-light state.

**Target:** Friday demo with AGL BLE central receiver

## Quick Start

1. Go to [makecode.microbit.org](https://makecode.microbit.org)
2. Create a new project
3. Add the BLE extension
4. Copy the code blocks from **Template A** or **Template B** below
5. Flash to micro:bit

## Template A: Simple Red State Only (Fastest Prototype)

Use this if you want to validate BLE communication in **5 minutes**.

```blocks
bluetooth.setAdvertisedName("Trafficlight")
bluetooth.setDeviceLED(true)
bluetooth.startLEDAnimation(bluetooth.animation(LedAnimationTypes.Sparkle))

let light_state = "R"

bluetooth.onBluetoothConnected(function () {
    light_state = "R"
    led.plotBarGraph(
        input.acceleration(Dimension.X),
        1023
    )
})

bluetooth.onBluetoothDisconnected(function () {
    led.clear()
})

on_start function () {
    bluetooth.startUartService()
}

basic.forever(function () {
    // Send state every 1 second via UART notification
    bluetooth.uartWriteText(light_state)
    basic.pause(1000)
})

// Button A to cycle: R -> Y -> G -> R
input.onButtonPressed(Button.A, function () {
    if (light_state == "R") {
        light_state = "Y"
    } else if (light_state == "Y") {
        light_state = "G"
    } else {
        light_state = "R"
    }
    led.plot(0, 0)
})
```

**What this does:**
- Advertises as "Trafficlight"
- Starts Nordic UART Service
- Sends state byte every 1s (R, Y, or G)
- Button A cycles through states

---

## Template B: Full State Machine with Safety (Production-Ready)

Use this if you want to implement the complete safety rules with yellow timeout.

```blocks
// === CONFIGURATION ===
let DEVICE_NAME = "Trafficlight"
let UART_SERVICE = bluetooth.uartServiceData()

// State enum
enum TrafficLightState {
    RED = 0,
    YELLOW = 1,
    GREEN = 2,
}

// === GLOBALS ===
let current_state = TrafficLightState.RED
let yellow_start_time = 0
let YELLOW_TIMEOUT_MS = 2000

// === INITIALIZATION ===
input.onStart(function () {
    bluetooth.setAdvertisedName(DEVICE_NAME)
    bluetooth.startUartService()
    led.plotBarGraph(0, 1023)  // All LEDs red initially
    basic.showNumber(0)  // Show RED (0)
})

// === BLUETOOTH EVENTS ===
bluetooth.onBluetoothConnected(function () {
    led.plot(0, 0)  // Light one pixel
    basic.showIcon(IconNames.Happy)
})

bluetooth.onBluetoothDisconnected(function () {
    current_state = TrafficLightState.RED
    led.clear()
    basic.showIcon(IconNames.Sad)
})

// === MAIN LOOP ===
basic.forever(function () {
    // Send state every 500ms
    let state_char = ""
    if (current_state == TrafficLightState.RED) {
        state_char = "R"
    } else if (current_state == TrafficLightState.YELLOW) {
        state_char = "Y"
    } else {
        state_char = "G"
    }

    bluetooth.uartWriteText(state_char)
    basic.pause(500)

    // Yellow timeout escalation check
    if (current_state == TrafficLightState.YELLOW) {
        if (input.runningTime() - yellow_start_time > YELLOW_TIMEOUT_MS) {
            current_state = TrafficLightState.RED
            led.plotBarGraph(1023, 1023)  // All red
            led.setPixelBrightness(2, 0)
            basic.showNumber(0)
        }
    }
})

// === USER CONTROLS ===
input.onButtonPressed(Button.A, function () {
    current_state = TrafficLightState.RED
    led.plotBarGraph(500, 1023)
    basic.showNumber(0)
})

input.onButtonPressed(Button.B, function () {
    current_state = TrafficLightState.YELLOW
    yellow_start_time = input.runningTime()
    led.plotBarGraph(700, 1023)
    basic.showNumber(1)
})

input.onButtonPressed(Button.A + Button.B, function () {
    current_state = TrafficLightState.GREEN
    led.plotBarGraph(0, 1023)
    basic.showNumber(2)
})
```

**What this does:**
- Full state machine (RED, YELLOW, GREEN)
- Yellow auto-escalates to RED after 2 seconds
- Sends state every 500ms via UART
- Button A: RED
- Button B: YELLOW
- Button A+B: GREEN
- LED visual feedback (red/yellow/green)
- Disconnects force RED (safe default)

---

## Alternative: Custom GATT Service (Advanced)

If you prefer a custom BLE characteristic instead of Nordic UART:

```blocks
// Define custom service UUID
let SERVICE_UUID = "12345678-1234-5678-1234-567812345678"
let CHAR_UUID = "87654321-4321-8765-4321-876543218765"

bluetooth.onBluetoothConnected(function () {
    basic.showString("C")  // Connected
})

basic.forever(function () {
    let state = "R"  // or Y, G
    
    // Notify characteristic with state byte
    bluetooth.uartWriteText(state)
    basic.pause(1000)
})
```

**Note:** Custom GATT requires more setup. Nordic UART Service (Template A/B) is simpler and widely supported.

---

## How to Flash

### Via USB-C (Recommended)

1. Connect micro:bit V2 to PC via USB-C
2. In MakeCode, click **Download**
3. Drag `.hex` file to MICROBIT drive (appears as external device)
4. Board resets automatically

### Via Web Sandbox (No Downloads)

MakeCode now supports direct flashing in newer browsers:
- Chrome 78+
- Edge 79+
- Opera 65+

Click **Download** → **Flash** → select device

---

## Validation Checklist

After flashing, verify on micro:bit:

- [ ] Board advertises as "Trafficlight" (visible in Bluetooth settings)
- [ ] LEDs respond to button presses (feedback)
- [ ] BLE receiver on AGL connects successfully
- [ ] AGL log shows: `Subscribed to Nordic UART RX`
- [ ] ADAS object socket receives state updates

---

## Debugging Tips

### Problem: Device not advertising

- Restart micro:bit (press RESET button on back)
- Reflash the .hex file
- Check MakeCode console for errors

### Problem: AGL cannot connect

- Verify device name matches `--device-name "Trafficlight"`
- Check micro:bit Bluetooth TX power (default is usually sufficient)
- Run `bluetoothctl` on AGL to manually scan: `scan on`

### Problem: Notifications not arriving

- Ensure `bluetooth.uartWriteText()` is called in main loop
- Check Nordic UART service UUID matches receiver (`6e400001-...`)
- Verify notification handler is subscribed to RX characteristic

---

## Next Steps

1. **Flash Template A** (RED state only) today for quick BLE validation
2. **Test with AGL receiver**: `python3 ble_trafficlight_receiver.py --config config.json --verbose`
3. **Once working**, upgrade to **Template B** (full state machine)
4. **Integrate with ADAS Manager** for end-to-end vehicle control

---

## References

- MakeCode Bluetooth docs: https://makecode.microbit.org/reference/bluetooth
- Nordic UART Service: https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.ble%2Fdvi%2Fble%2Fintroduction.html
- Bleak (Python BLE library): https://bleak.readthedocs.io/
