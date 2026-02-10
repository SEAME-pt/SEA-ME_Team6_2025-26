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

        Gauge {
            anchors.centerIn: parent
            centerValue: wheelSpeedProvider.currWheelSpeed
            centerMaxValue: 80
            centerTicks: [0, 10, 20, 30, 40, 50, 60, 70, 80]
            centerUnit: "RPM"
        }
    }
}
