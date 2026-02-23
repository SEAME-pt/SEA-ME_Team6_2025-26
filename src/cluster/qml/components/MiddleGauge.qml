import QtQuick
import QtQuick.Controls
import QtQuick.Shapes
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0

Item {
    id: root
    width: 375
    height: 375
    property real currSpeed: 124
    property real maxSpeed: 320
    //? Helpers
    property real mainAngleStart: 240
    property real mainAngleSweep: 240
    property real secundaryAngleStart: 145 // @note: mainAngleStart - (1/2 * division space) - secundaryAngleSweep 
    property real secundaryAngleSweep: 70
    property int  activeIndex: Math.round((currSpeed / maxSpeed) * (innerTotalTicks - 1))
    //? Outer Circle
    property real outerTotalTicks: 33 * 4
    property real outerAngleStep: mainAngleSweep / (outerTotalTicks - 1)
    property int  outerActiveIndex: Math.round((currSpeed / maxSpeed) * (outerTotalTicks - 1))
    //? Inner Circle
    property real innerTotalTicks: 33
    property real innerAngleStep: mainAngleSweep / (innerTotalTicks - 1)
    //? Bottom Background Circle
    property real bottomTotalTicks: 49
    property real bottomAngleStep: secundaryAngleSweep / (bottomTotalTicks - 1)

    Rectangle {
        id: innerCircle
        anchors.centerIn: parent
        width: 375
        height: 375
        radius: width / 2
        color: "#141414"
    }

    Repeater {
        id: outerTicks
        model: outerTotalTicks
        delegate: Item {
            anchors.centerIn: parent
            width: innerCircle.width
            height: innerCircle.height

            transform: Rotation {
                origin.x: width / 2
                origin.y: height / 2
                angle: root.mainAngleStart + (index * root.outerAngleStep)
            }

            Rectangle {
                width: 1.5
                height: 10
                anchors.horizontalCenter: parent.horizontalCenter
                y: 8
                color: index <= root.outerActiveIndex ? "#FFD700" : "#444444"
                opacity: index <= root.outerActiveIndex ? 1.0 : 0.35
                antialiasing: true
                radius: 1
            }
        }
    }

    Repeater {
        id: innerTicks
        model: innerTotalTicks
        delegate: Item {
            anchors.centerIn: parent
            width: innerCircle.width
            height: innerCircle.height

            transform: Rotation {
                origin.x: width / 2
                origin.y: height / 2
                angle: root.mainAngleStart + (index * root.innerAngleStep)
            }

            Rectangle {
                width: index % 2 === 0 ? 4 : 2
                height: index % 2 === 0 ? 16 : 9
                anchors.horizontalCenter: parent.horizontalCenter
                y: 22
                color: index <= root.activeIndex ? "#FFD700" : "#444444"
                opacity: index <= root.activeIndex ? 1.0 : 0.35
                antialiasing: true
                radius: 1
            }

            Text {
                visible: index % 4 === 0
                text: Math.round((index / (root.innerTotalTicks - 1)) * root.maxSpeed)
                color: index <= root.activeIndex ? "#FFD700" : "#666666"
                font.pixelSize: 10
                anchors.horizontalCenter: parent.horizontalCenter
                y: 42
                transform: Rotation {
                    origin.x: width / 2
                    origin.y: -42 + parent.height / 2
                }
            }
        }
    }

    Repeater {
        id: bottomTicks
        model: bottomTotalTicks
        delegate: Item {
            anchors.centerIn: parent
            width: innerCircle.width
            height: innerCircle.height

            transform: Rotation {
                origin.x: width / 2
                origin.y: height / 2
                angle: root.secundaryAngleStart + (index * root.bottomAngleStep)
            }

            Rectangle {
                width: index % 12 === 0 ? 4 : 2
                height: 9
                anchors.horizontalCenter: parent.horizontalCenter
                y: 8
                color: BaseTheme.success
                opacity: 1.0
                antialiasing: true
                radius: 1
            }

            Rectangle {
                visible: index % 12 === 0
                width: (index === 0 || index === 24 || index === 48) ? 4 : 2
                height: 16
                anchors.horizontalCenter: parent.horizontalCenter
                y: 8
                color: BaseTheme.white
                opacity: 1.0
                antialiasing: true
                radius: 1
            }
        }
    }
}