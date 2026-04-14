import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ClusterTheme 1.0

Item {
    id: navButton
    property string iconSrc
    property bool active: false

    signal clicked()

    MouseArea {
        anchors.centerIn: parent
        width: 32
        height: 32
        onClicked: navButton.clicked()
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
    }

    Image {
        id: icon
        anchors.centerIn: parent
        source: iconSrc
        width: 32
        height: 32
        sourceSize.width: 32
        sourceSize.height: 32
        fillMode: Image.PreserveAspectFit
        smooth: true
        mipmap: true
        opacity: active ? 1.0 : 0.5
        scale: active ? 1.0 : 0.8

        Behavior on opacity {
            NumberAnimation {
                duration: 300
                easing.type: Easing.InOutQuad
            }
        }

        Behavior on scale {
            NumberAnimation {
                duration: 300
                easing.type: Easing.InOutQuad
            }
        }
    }
}