import QtQuick
import QtQuick.Controls
import QtQuick.Shapes
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0

Item {
    id: root
    width: 300
    height: 300

    property int currPage: 1
    property int maxPages: 3
    property bool isWarningActive: false

    function pageSource(page) {
        if (isWarningActive)
            return "WarningContent.qml"
        switch(page) {
            case 0: return "TripContent.qml"
            case 1: return "LKAContent.qml"
            case 2: return "DynamicsContent.qml"
        }
    }

    //? OUTER RING
    Rectangle {
        id: outerCircle
        anchors.centerIn: parent
        width: 300
        height: 300
        radius: width / 2
        color: BaseTheme.sportBlack
        layer.enabled: true
        layer.live: false
        layer.smooth: false
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: BaseTheme.white
            shadowBlur: 1.0
            shadowOpacity: 0.6
            shadowHorizontalOffset: 0
            shadowVerticalOffset: 0
            shadowScale: 1.01
        }
    }

    Repeater {
        id: pageIndications
        model: root.maxPages

        Rectangle {
            id: indicatorDot
            visible: !root.isWarningActive
            width: 8
            height: 8
            radius: 4

            property real angle: -45 + (index * 6)
            property real distance: 165

            x: parent.width / 2 + Math.cos(angle * Math.PI / 180) * distance - width / 2
            y: parent.height / 2 + Math.sin(angle * Math.PI / 180) * distance - height / 2

            color: root.currPage === index ? BaseTheme.white : BaseTheme.carbon

            Behavior on color {
                ColorAnimation {
                    duration: 200
                    easing.type: Easing.InOutQuad
                }
            }

            layer.enabled: root.currPage === index
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: BaseTheme.white
                shadowBlur: 0.8
                shadowOpacity: 0.8
                shadowHorizontalOffset: 0
                shadowVerticalOffset: 0
                shadowScale: 1.5
            }
        }
    }

    //? GAUGE CONTENT AREA
    Rectangle {
        id: innerCircle
        anchors.centerIn: parent
        width: 275
        height: 275
        radius: width / 2
        color: BaseTheme.darkBlack
        border.color: BaseTheme.blackboard
        border.width: 2
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: BaseTheme.white
            shadowBlur: 1.0
            shadowOpacity: 0.9
            shadowHorizontalOffset: 0
            shadowVerticalOffset: 0
            shadowScale: 0.98
        }

        Loader {
            id: contentLoader
            anchors.fill: parent

            source: root.pageSource(root.currPage)

            Behavior on opacity {
                NumberAnimation { duration: 200 }
            }
        }
    }

    MouseArea {
        anchors.centerIn: parent
        width: 275
        height: 275
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            if (root.isWarningActive) return

            contentLoader.opacity = 0
            fadeTimer.start()
        }
    }

    Timer {
        id: fadeTimer
        interval: 200
        onTriggered: {
            if (root.isWarningActive) {
                contentLoader.opacity = 1.0
                return
            }

            root.currPage = (root.currPage + 1) % root.maxPages
            contentLoader.opacity = 1.0
        }
    }
}