import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0
import Cluster.Backend 1.0
import "../components"

Item {
    id: rightScreen
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
                id: rpmGauge
                anchors.centerIn: parent
                centerValue: wheelSpeedProvider.currWheelSpeed
                centerMaxValue: 80
                centerTicks: [0, 10, 20, 30, 40, 50, 60, 70, 80]
                centerUnit: "RPM"
            }
        }
    }
}
