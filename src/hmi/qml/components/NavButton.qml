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

    MouseArea {
        anchors.fill: parent
        onClicked: navButton.clicked()
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
    }
    
    Image {
        id: buttonIcon
        anchors.centerIn: parent
        source: active ? iconActive : iconInactive
        width: 32
        height: 32
        fillMode: Image.PreserveAspectFit
        smooth: true
        mipmap: true
        sourceSize.width: 32
        sourceSize.height: 32
    }
}