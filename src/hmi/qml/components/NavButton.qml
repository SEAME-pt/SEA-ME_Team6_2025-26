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
        anchors.centerIn: parent
        width: 54
        height: 48
        onClicked: navButton.clicked()
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
    }
    
    Image {
        id: inactiveIcon
        anchors.centerIn: parent
        source: iconInactive
        width: 32
        height: 32
        sourceSize.width: 32
        sourceSize.height: 32
        fillMode: Image.PreserveAspectFit
        smooth: true
        mipmap: true
        opacity: active ? 0.0 : 1.0
        scale: active ? 0.8 : 1.0

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

    Image {
        id: activeIcon
        anchors.centerIn: parent
        source: iconActive
        width: 32
        height: 32
        sourceSize.width: 32
        sourceSize.height: 32
        fillMode: Image.PreserveAspectFit
        smooth: true
        mipmap: true
        opacity: active ? 1.0 : 0.0
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