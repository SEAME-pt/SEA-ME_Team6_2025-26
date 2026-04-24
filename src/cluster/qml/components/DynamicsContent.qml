import QtQuick
import ClusterTheme 1.0

Item {
    anchors.fill: parent

    Column {
        anchors.centerIn: parent
        spacing: 6

        Text {
            text: "To be added..."
            color: BaseTheme.white
            font.pixelSize: 18
        }

        Text {
            text: adas.lateralDeviation
            color: BaseTheme.white
            font.pixelSize: 18
        }

        Text {
            text: adas.laneStatus
            color: BaseTheme.white
            font.pixelSize: 18
        }
    }
}