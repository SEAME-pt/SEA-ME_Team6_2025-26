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

    //readonly property color black: "#212529"
    readonly property color black: "#000000"
    readonly property color blackLight: "#191a1a"

    //? Information Colors
    readonly property color success: "#378b1b"
    readonly property color warning: "#F0D500"
    readonly property color danger: "#F32013"


    //? Accent / Vibrant color
    readonly property color vibrant: "#0ADD08"
}
