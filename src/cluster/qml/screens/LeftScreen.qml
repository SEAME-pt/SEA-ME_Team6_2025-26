import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ClusterTheme 1.0
import Cluster.Backend 1.0

Item {
    id: leftScreen
    Layout.fillHeight: true
    Layout.fillWidth: true
    Layout.margins: 0

    SpeedProvider {
        id: speedProvider
    }

    Rectangle {
        id: base
        anchors.fill: parent
        color: BaseTheme.black

        Row {
            anchors.verticalCenter: parent.verticalCenter
            anchors.centerIn: parent
            spacing: 8

            Text {
                text: speedProvider.currSpeed
                font.pixelSize: 64
                font.bold: true
                color: BaseTheme.white
            }
            Text {
                text: "m/h"
                font.pixelSize: 12
                color: BaseTheme.white
            }
        }
    }

}
