import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ClusterTheme 1.0

Item {
    id: navButton
    property string iconActive
    property string iconInactive
    property bool active: false

    signal clicked()

    Layout.fillWidth: true
    height: 32

    Rectangle {
        anchors.fill: parent
        color: "transparent"

        MouseArea {
            anchors.fill: parent
            onClicked: navButton.clicked()
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
        }
    }

    Rectangle {
        id: indicator
        width: 4
        height: 32
        radius: 2
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        color: BaseTheme.primaryLight
        visible: navButton.active
    }

    Image {
        id: buttonIcon
        source: active ? iconActive : iconInactive
        width: 32
        height: 32
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: indicator.right
        anchors.leftMargin: 8
        fillMode: Image.PreserveAspectFit
    }
}
