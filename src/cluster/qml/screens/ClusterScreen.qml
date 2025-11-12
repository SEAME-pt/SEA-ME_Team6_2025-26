import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: clusterScreen
    radius: 8

    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: "Welcome to SEA:ME Cluster"
            font.pixelSize: 24
            font.bold: true
            color: "black"
        }
    }
}
