import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0
import Cluster.Backend 1.0
import "../components"

Item {
    id: rightScreen
    Layout.fillHeight: true
    Layout.fillWidth: true
    Layout.margins: 0

    Rectangle {
        id: base
        anchors.fill: parent
        color: BaseTheme.black

        SpeedGauge {
            anchors.centerIn: parent
            value: temperatureProvider.currTemperature
            minValue: 0
            maxValue: 300
            startAngle: 135
            endAngle: 270
        }
    }
}
