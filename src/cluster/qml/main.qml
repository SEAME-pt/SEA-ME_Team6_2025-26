import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

ApplicationWindow {
    id: root
    visible: true
    width: 1028
    height: 400
    minimumWidth: 1028
    maximumWidth: 1028
    minimumHeight: 400
    maximumHeight: 400
    title: qsTr("SEA:ME Cluster Team 6")

    Loader {
        anchors.fill: parent
        source: "screens/BaseScreen.qml"
    }
}
