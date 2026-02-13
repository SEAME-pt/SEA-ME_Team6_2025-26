import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0
import Cluster.Backend 1.0
import "../components"

Item {
    id: leftScreen
    Layout.preferredWidth: 350
    Layout.fillHeight: true
    Layout.margins: 0

    Rectangle {
        id: base
        anchors.fill: parent
        color: BaseTheme.black

        Item {
            id: gaugeContainer
            anchors.centerIn: parent
            width: 300
            height: 300

            Gauge {
                id: speedGauge
                anchors.centerIn: parent
                centerValue: speedProvider.currSpeed
                centerMaxValue: 1000
                centerTicks: [0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000]
                centerUnit: "m/h"
                showBottom: true
                bottomValue: voltageProvider.currVoltage
                bottomMinValue: 9
                bottomMaxValue: 13
                middleIcon: voltageProvider.voltageIcon
                middleText: "x m"
            }
        }
    }
}
