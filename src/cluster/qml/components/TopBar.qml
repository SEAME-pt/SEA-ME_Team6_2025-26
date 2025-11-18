import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ClusterTheme 1.0
import Cluster.Backend 1.0

Item {
    id: topBar
    Layout.preferredHeight: 16
    Layout.fillWidth: true

    TimeProvider {
        id: clock
    }
    SystemStatus {
        id: system
    }

    // LEFT BLOCK
    Item {
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        width: 60
        height: parent.height

        Column {
            anchors.verticalCenter: parent.verticalCenter
            spacing: -2

            Text {
                text: clock.currTime
                font.pixelSize: 14
                font.bold: true
                color: "white"
            }
            Text {
                text: clock.currDate
                font.pixelSize: 12
                color: "white"
            }
        }
    }

    // CENTER BLOCK
    Text {
        text: clock.currTime
        font.pixelSize: 14
        color: "white"
        anchors.centerIn: parent
    }

    // RIGHT BLOCK
    Item {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        height: parent.height
        anchors.rightMargin: 16

        Row {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 4

            Image {
                source: system.batteryIconSource
                width: 16
                height: 16
                fillMode: Image.PreserveAspectFit
            }
            /*
            Text {
                text: system.currBatteryLvl + "%"
                font.pixelSize: 10
                color: "white"
            }*/
        }
    }
}
