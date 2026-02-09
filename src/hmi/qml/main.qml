import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

ApplicationWindow {
    id: root
    visible: true
    width: 1024
    height: 600
    minimumWidth: 1024
    maximumWidth: 1024
    minimumHeight: 600
    maximumHeight: 600
    title: qsTr("SEA:ME HMI Team 6")

    Loader {
        anchors.fill: parent
        source: "screens/BaseScreen.qml"
    }
}
