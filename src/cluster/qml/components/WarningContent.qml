import QtQuick
import ClusterTheme 1.0

Item {
    anchors.fill: parent
    Column {
        anchors.centerIn: parent
        spacing: 16

        Text {
            text: "WARNING"
            color: BaseTheme.white
            font.pixelSize: 18
        }

        Text {
            text: adas.frontDistance
            color: BaseTheme.white
            font.pixelSize: 18
        }
    }
}