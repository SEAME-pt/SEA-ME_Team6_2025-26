import QtQuick
import QtQuick.Controls

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello Qt 6 QML")

    Rectangle {
        anchors.fill: parent
        color: "#282c34"

        Text {
            text: "SEA:ME"
            anchors.centerIn: parent
            font.pixelSize: 40
            color: "white"
        }
    }
}
