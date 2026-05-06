import QtQuick
import ClusterTheme 1.0

Item {
    anchors.fill: parent

    Column {
        anchors.centerIn: parent
        spacing: 4

        Text {
            text: "lateralDeviation: " + adas.lateralDeviation
            color: BaseTheme.white
            font.pixelSize: 10
        }

        Text {
            text: "laneStatus: " + adas.laneStatus
            color: BaseTheme.white
            font.pixelSize: 10
        }

        Text {
            text: "speedLimit: " + adas.speedLimit
            color: BaseTheme.white
            font.pixelSize: 10
        }

        Text {
            text: "trafficLight: " + adas.trafficLight
            color: BaseTheme.white
            font.pixelSize: 10
        }

        Text {
            text: "streetSignals: "
            color: BaseTheme.white
            font.pixelSize: 10
        }

        Repeater {
            model: adas.streetSignals
            Text {
                text: modelData
                color: BaseTheme.white
                font.pixelSize: 10
            }
        }

        Text {
            text: "extras: "
            color: BaseTheme.white
            font.pixelSize: 10
        }

        Repeater {
            model: adas.extras
            Text {
                text: modelData
                color: BaseTheme.white
                font.pixelSize: 10
            }
        }
    }
}