import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: settingsScreen
    radius: 8

    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: "settings"
            font.pixelSize: 24
            font.bold: true
            color: "black"
        }
    }
}
