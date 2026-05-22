pragma Singleton
import QtQuick 2.15

QtObject {
    //? Primary Palette
    readonly property color primary: "#12768A"
    readonly property color primaryLight: "#6DADBE"
    readonly property color primaryExtraLight: "#CEDADB"
    readonly property color primaryDark: "#0B3F43"

    //? Neutrals
    readonly property color white: "#EBE8E7"

    //readonly property color black: "#151618"
    readonly property color black: "#000000"
    readonly property color darkBlack: "#020202"
    readonly property color authenticBlack: "#080808"
    readonly property color sportBlack: "#141414"
    readonly property color blackboard: "#1c1c1c"
    readonly property color carbon: "#232323"
    readonly property color blackLight: "#191a1a"


    //? Gauge
    readonly property color gaugeTicksActive: "#edca30"
    readonly property color gaugeTicksInactive: "#777777"
    readonly property color gaugeMainTextInformation: "#c4c4c4"
    readonly property color gaugeBattery: "#4E9367"
    readonly property color gaugeBatteryEmpty: "#881633"

    //? Information Colors
    readonly property color success: "#4E9367"
    readonly property color warning: "#edca30"
    readonly property color danger: "#881633"
    readonly property color info: "#1E88E5"

    //? Accent / Vibrant color
    readonly property color vibrant: "#0ADD08"
}
