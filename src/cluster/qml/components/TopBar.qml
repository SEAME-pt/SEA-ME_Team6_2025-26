import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ClusterTheme 1.0
import Cluster.Backend 1.0

Item {
    id: topBar
    Layout.preferredHeight: 32
    Layout.fillWidth: true

    TimeProvider { id: clock }

    Text {
        id: dateText
        text: clock.currDate
        color: "white"
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
    }

    Text {
        id: timeText
        text: clock.currTime
        color: "white"
        anchors.centerIn: parent
    }

    Text {
        id: rightText
        text: "Right"
        color: "white"
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
    }
}


