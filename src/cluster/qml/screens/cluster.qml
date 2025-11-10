import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: clusterScreen
    width: 1028
    height: 400

    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: "Welcome to SEA:ME Cluster"
            font.pixelSize: 24
            font.bold: true
            color: "black"
        }

        Rectangle {
            width: 300
            height: 150
            radius: 20
            color: "#d0d0d0"
            border.color: "black"

            Text {
                anchors.centerIn: parent
                text: "Cluster Info"
                font.pixelSize: 16
                color: "black"
            }
        }
    }
}
