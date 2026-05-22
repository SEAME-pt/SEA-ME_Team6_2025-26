import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0
import "../components"

Item {
    id: leftScreen
    Layout.fillWidth: true
    Layout.fillHeight: true
    //? Data
    property string currentGear: "D"
    property var gearData: [{label: "R", icon: "reverse"}, {label: "N", icon: "neutral"}, {label: "D", icon: "forward"}]

    LeftGauge {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 75
        currMotorSpeed: powertrain.motorSpeed
        currTotalKm: 0
        streetSignal: adas.streetSignals.length > 1 ? adas.streetSignals[0] : ""
    }

    // Item {
    //     id: gearSelector
    //     width: 80
    //     height: 180
    //     anchors.top: parent.top
    //     anchors.right: parent.right
    //     anchors.topMargin: 100
    //     anchors.rightMargin: -18

    //     Column {
    //         anchors.fill: parent
    //         spacing: 4

    //         Repeater {
    //             model: leftScreen.gearData

    //             delegate: Item {
    //                 width: 80
    //                 height: 40

    //                 property bool isActive: modelData.label === leftScreen.currentGear

    //                 Rectangle {
    //                     anchors.centerIn: parent
    //                     width: 32
    //                     height: 32
    //                     radius: 18
    //                     visible: isActive
    //                     color: BaseTheme.white
    //                 }

    //                 Image {
    //                     anchors.centerIn: parent
    //                     source: isActive
    //                         ? "qrc:/assets/icons/" + modelData.icon + "-icon-black.svg"
    //                         : "qrc:/assets/icons/" + modelData.icon + "-icon-white.svg"
    //                     width: 16
    //                     height: 16
    //                     sourceSize.width: 16
    //                     sourceSize.height: 16
    //                     fillMode: Image.PreserveAspectFit
    //                     smooth: true
    //                     mipmap: true
    //                 }
    //             }
    //         }
    //     }
    // }
}
