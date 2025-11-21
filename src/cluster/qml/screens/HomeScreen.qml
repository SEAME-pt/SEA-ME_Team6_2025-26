import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: homeScreen
    radius: 8

    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: "Home"
            font.pixelSize: 24
            font.bold: true
            color: "black"
        }
    }
}
