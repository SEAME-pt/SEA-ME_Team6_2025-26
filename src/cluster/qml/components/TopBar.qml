import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ClusterTheme 1.0
import Cluster.Backend 1.0

Rectangle {
    id: topBar
    color: BaseTheme.black
    Layout.preferredHeight: 48
    Layout.fillWidth: true

    TimeProvider { id: clock }

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
                font.pixelSize: 12
                font.bold: true
                color: "white"
            }
            Text {
                text: clock.currDate
                font.pixelSize: 10
                color: "white"
            }
        }
    }

    // CENTER BLOCK
    Text {
        text: clock.currTime
        color: "white"
        anchors.centerIn: parent
    }

    // RIGHT BLOCK
    Text {
        text: "Right"
        color: "white"
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.verticalCenter: parent.verticalCenter
    }
}
