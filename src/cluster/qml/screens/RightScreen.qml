import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0
import "../components"

Item {
    id: rightScreen
    Layout.fillWidth: true
    Layout.fillHeight: true

    RightGauge {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 75
        isWarningActive: adas.frontDistance < 200
    }
}
