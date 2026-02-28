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

    LeftGauge {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 75
        currMotorSpeed: 67
        currTotalKm: 0
        // currMotorSpeed: powertrain.motorSpeed 
    }
}
