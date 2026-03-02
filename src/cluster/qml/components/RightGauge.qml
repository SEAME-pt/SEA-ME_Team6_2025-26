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

    property int currPage: 0
    property int maxPages: 3

    //? OUTER RING
    Rectangle {
        id: outerCircle
        anchors.centerIn: parent
        width: 300
        height: 300
        radius: width / 2
        color: BaseTheme.sportBlack
        layer.enabled: true
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
            //anchors.margins: 6

            sourceComponent: {
                switch(root.currPage) {
                    case 0: return batteryContent
                    case 1: return tripContent
                    case 2: return headingContent
                }
            }

            Behavior on opacity {
                NumberAnimation { duration: 200 }
            }
        }
    }

    Component {
        id: batteryContent
        Item {
            anchors.fill: parent
            Text {
                anchors.centerIn: parent
                text: "Battery Content"
                color: BaseTheme.white
                font.pixelSize: 18
            }
        }
    }

    Component {
        id: tripContent
        Item {
            anchors.fill: parent
            Text {
                anchors.centerIn: parent
                text: "Trip Content"
                color: BaseTheme.white
                font.pixelSize: 18
            }
        }
    }

    Component {
        id: headingContent
        Item {
            anchors.fill: parent
            Text {
                anchors.centerIn: parent
                text: "Heading Content"
                color: BaseTheme.white
                font.pixelSize: 18
            }
        }
    }

    MouseArea {
        width: 275
        height: 275
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            contentLoader.opacity = 0
            fadeTimer.start()
        }
    }

    Timer {
        id: fadeTimer
        interval: 200
        onTriggered: {
            root.currPage = (root.currPage + 1) % root.maxPages
            contentLoader.opacity = 1.0
        }
    }
}