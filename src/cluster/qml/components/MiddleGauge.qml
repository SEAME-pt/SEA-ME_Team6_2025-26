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
    //? Data
    property real currSpeed: 0
    property real maxSpeed: 4000
    property real currBattery: 0
    property real maxBattery: 12
    property real minBattery: 9
    property real currAutonomy: 0
    property string currBatteryIcon: "qrc:/assets/icons/battery.svg"
    property bool isBatteryWarning: false
    property bool isBatteryDanger: false
    property string speedUnit: "m/h"
    //? Helpers
    property real mainAngleStart: 245
    property real mainAngleSweep: 230
    property real secundaryAngleStart: 140 // @note: mainAngleStart - (1/2 * division space) - secundaryAngleSweep
    property real secundaryAngleSweep: 80
    property real  activeIndex: (displaySpeed / maxSpeed) * (innerTotalTicks - 1)
    property int bottomActiveIndex:  Math.round(batteryNormalized() * (bottomTotalTicks - 1))
    //? Outer Circle
    property real outerTotalTicks: 161
    property real outerAngleStep: mainAngleSweep / (outerTotalTicks - 1)
    property real outerActiveIndex: (activeIndex / (innerTotalTicks - 1)) * (outerTotalTicks - 1)
    //? Inner Circle
    property real innerTotalTicks: 33
    property real innerAngleStep: mainAngleSweep / (innerTotalTicks - 1)
    //? Bottom Background Circle
    property real bottomTotalTicks: 81
    property real bottomAngleStep: secundaryAngleSweep / (bottomTotalTicks - 1)
    //? Animation Display
    property real displaySpeed: 0
    property real displayBattery: 0
    onCurrSpeedChanged: displaySpeed = currSpeed
    onCurrBatteryChanged: displayBattery = currBattery

    Behavior on displaySpeed {
        NumberAnimation {
            duration: 250
            easing.type: Easing.InOutQuad
        }
    }

    Behavior on displayBattery {
        NumberAnimation {
            duration: 500
            easing.type: Easing.InOutQuad
        }
    }


    function getBatteryTickColor(index) {
        if (index === root.bottomTotalTicks - 1)
            return BaseTheme.gaugeBatteryEmpty
        else if (index % 20 === 0)
            return BaseTheme.gaugeMainTextInformation
        else if (index >= (root.bottomTotalTicks - 1 - root.bottomActiveIndex)) {
            if (isBatteryDanger)
                return BaseTheme.danger
            if (isBatteryWarning)
                return BaseTheme.warning
            return BaseTheme.gaugeBattery
        }
        return BaseTheme.gaugeTicksInactive
    }

    function getBatteryOpacity(index) {
        if (index >= (root.bottomTotalTicks - 1 - root.bottomActiveIndex))
            return 1.0
        else if (index % 20 === 0)
            return 1.0
        return 0.45
    }

    function getAutonomyColor() {
        if (isBatteryDanger)
            return BaseTheme.danger
        if (isBatteryWarning)
            return BaseTheme.warning
        return BaseTheme.gaugeBattery
    }

    function batteryNormalized() {
        if (root.displayBattery <= root.minBattery)
            return 0
        if (root.displayBattery >= root.maxBattery)
            return 1
        return (root.displayBattery - root.minBattery) / (root.maxBattery - root.minBattery)
    }


    //? OUTER RING
    Rectangle {
        id: outerCircle
        anchors.centerIn: parent
        width: 375
        height: 375
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

    //? GAUGE CONTENT AREA
    Rectangle {
        id: innerCircle
        anchors.centerIn: parent
        width: 350
        height: 350
        radius: width / 2
        color: BaseTheme.darkBlack
        border.color: BaseTheme.blackboard
        border.width: 2
         layer.enabled: true
        layer.live: false
        layer.smooth: false
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: BaseTheme.white
            shadowBlur: 1.0
            shadowOpacity: 0.9
            shadowHorizontalOffset: 0
            shadowVerticalOffset: 0
            shadowScale: 0.98
        }
    }

    //? OUTER TICKS
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
                width: index % 10 === 0 ? 2.5 : 1.5
                height: 10
                anchors.horizontalCenter: parent.horizontalCenter
                y: 12
                color: index <= root.outerActiveIndex ? BaseTheme.gaugeTicksActive : BaseTheme.gaugeTicksInactive
                opacity: index <= root.outerActiveIndex ? 1.0 : 0.45
                antialiasing: true
                radius: 1
            }
        }
    }

    //? INNER TICKS WITH TEXT 
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
                width: index % 2 === 0 ? 4 : 1.5
                height: index % 2 === 0 ? 16 : 10
                anchors.horizontalCenter: parent.horizontalCenter
                y: 24
                color: index <= root.activeIndex ? BaseTheme.gaugeTicksActive : BaseTheme.gaugeMainTextInformation
                opacity: 1.0
                antialiasing: true
                radius: 1
            }

            Text {
                visible: index % 4 === 0
                text: Math.round((index / (root.innerTotalTicks - 1)) * root.maxSpeed)
                color: index <= root.activeIndex ? BaseTheme.gaugeTicksActive : BaseTheme.gaugeMainTextInformation
                font.pixelSize: 15
                anchors.horizontalCenter: parent.horizontalCenter
                y: 56
                rotation: -(root.mainAngleStart + (index * root.innerAngleStep))
            }
        }
    }

    //? CENTER ARC INVOLVING THE MAIN TEXT
    Shape {
        id: centerArc
        anchors.fill: parent
        antialiasing: true
        opacity: 0.45

        property real radius: 75
        property real sweepAngle: root.mainAngleSweep + 10
        property real startAngle: 180 + (180 - sweepAngle) / 2

        ShapePath {
            strokeWidth: 1
            strokeColor: BaseTheme.gaugeTicksInactive
            fillColor: "transparent"

            PathAngleArc {
                centerX: innerCircle.x + innerCircle.width / 2
                centerY: innerCircle.y + innerCircle.height / 2
                radiusX: centerArc.radius
                radiusY: centerArc.radius
                startAngle: centerArc.startAngle
                sweepAngle: centerArc.sweepAngle
            }
        }
    }

    //? CENTER TEXT AND UNIT
    Column {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 0
        spacing: 0

        Text {
            text: Math.round(root.displaySpeed)
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 48
            color: BaseTheme.gaugeMainTextInformation
        }

        Text {
            text: root.speedUnit
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 18
            color: BaseTheme.gaugeMainTextInformation
            opacity: 0.7
        }
    }

    // Item {
    //     anchors.horizontalCenter: parent.horizontalCenter
    //     anchors.bottom: parent.bottom
    //     anchors.bottomMargin: 112

    //     Column {
    //         anchors.centerIn: parent
    //         anchors.verticalCenterOffset: 0
    //         spacing: 4

    //         Image {
    //             source: root.currBatteryIcon
    //             anchors.horizontalCenter: parent.horizontalCenter
    //             width: 24
    //             height: 24
    //             sourceSize.width: 24
    //             sourceSize.height: 24
    //             fillMode: Image.PreserveAspectFit
    //             smooth: true
    //             mipmap: true
    //         }

    //         Text {
    //             text: root.currAutonomy + " km"
    //             anchors.horizontalCenter: parent.horizontalCenter
    //             font.pixelSize: 13
    //             font.bold: true
    //             color: getAutonomyColor()
    //         }
    //     }
    // }

    //? BOTTOM TICKS FOR BATTERY
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
                width: index % 20 === 0 ? 4 : 2
                height: index % 20 === 0 ? 18 : 10
                anchors.horizontalCenter: parent.horizontalCenter
                y: 12
                color: getBatteryTickColor(index)
                opacity: getBatteryOpacity(index)
                antialiasing: true
                radius: 1
            }

            Text {
                visible: index % 20 === 0
                text: {
                    if (index === 0)
                        return 'F'
                    else if (index === 40)
                        return '1/2'
                    else if (index === root.bottomTotalTicks - 1)
                        return 'E'
                    return ''
                }
                color: BaseTheme.gaugeMainTextInformation
                font.pixelSize: 14
                anchors.horizontalCenter: parent.horizontalCenter
                y: 36
                rotation: -(root.secundaryAngleStart + (index * root.bottomAngleStep))
            }
        }
    }
}