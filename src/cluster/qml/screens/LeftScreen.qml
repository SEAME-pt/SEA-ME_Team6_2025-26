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

        // Container with the Gauge
        Item {
            id: gaugeContainer
            anchors.centerIn: parent
            width: 300
            height: 300

            // Rectangle {
            //     id: shadowSource
            //     anchors.centerIn: parent
            //     width: 225
            //     height: 225
            //     radius: width / 2
            //     color: "#0a0a0a"
            //     visible: false
            // }

            // MultiEffect {
            //     source: shadowSource
            //     anchors.centerIn: shadowSource
            //     width: shadowSource.width * 1.1
            //     height: shadowSource.height * 1.1
            //     shadowEnabled: true
            //     shadowColor: "#b0b0b0"
            //     shadowBlur: 1.0
            //     shadowOpacity: 0.3
            //     shadowScale: 1.2
            //     shadowVerticalOffset: 0
            //     shadowHorizontalOffset: 0
            // }

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
                middleText: "67 m"
            }
        }
    }
}
