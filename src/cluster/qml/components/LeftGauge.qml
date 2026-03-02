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
    //? Data
    property real currMotorSpeed: 0
    property real maxMotorSpeed: 120
    property int  currTotalKm: 0
    //? Helpers
    property real mainAngleStart: 230
    property real mainAngleSweep: 260
    property real activeIndex: (currMotorSpeed / maxMotorSpeed) * (innerTotalTicks - 1)
    //? Outer Circle
    property real outerTotalTicks: 161
    property real outerAngleStep: mainAngleSweep / (outerTotalTicks - 1)
    property real outerActiveIndex: (activeIndex / (innerTotalTicks - 1)) * (outerTotalTicks - 1)
    //? Inner Circle
    property real innerTotalTicks: 33
    property real innerAngleStep: mainAngleSweep / (innerTotalTicks - 1)

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
                text: Math.round((index / (root.innerTotalTicks - 1)) * root.maxMotorSpeed)
                color: index <= root.activeIndex ? BaseTheme.gaugeTicksActive : BaseTheme.gaugeMainTextInformation
                font.pixelSize: 16
                anchors.horizontalCenter: parent.horizontalCenter
                y: 48
                rotation: -(root.mainAngleStart + (index * root.innerAngleStep))
            }
        }
    }

    Behavior on currMotorSpeed {
        NumberAnimation {
            duration: 500
            easing.type: Easing.InOutQuad
        }
    }

    //? CENTER CIRCLE
    Shape {
        id: centerCircle
        anchors.fill: parent
        antialiasing: true
        opacity: 0.45

        property real radius: 55
        property real sweepAngle: 360
        property real startAngle: 0

        ShapePath {
            strokeWidth: 1
            strokeColor: BaseTheme.gaugeTicksInactive
            fillColor: "transparent"

            PathAngleArc {
                centerX: innerCircle.x + innerCircle.width / 2
                centerY: innerCircle.y + innerCircle.height / 2
                radiusX: centerCircle.radius
                radiusY: centerCircle.radius
                startAngle: centerCircle.startAngle
                sweepAngle: centerCircle.sweepAngle
            }
        }
    }

    Image {
        anchors.centerIn: parent
        source: "qrc:/assets/images/seame-logo.png"
        width: 80
        height: 80
        sourceSize.width: 80
        sourceSize.height: 80
        fillMode: Image.PreserveAspectFit
        smooth: true
        mipmap: true
    }

    //? ODOMETER
    Item {
        width: 80
        height: 22
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 46

        Row {
            anchors.centerIn: parent
            spacing: 0

            Repeater {
                model: 4
                Rectangle {
                    width: 16
                    height: 20
                    color: BaseTheme.carbon
                    topLeftRadius: index === 0 ? 4 : 0
                    bottomLeftRadius: index === 0 ? 4 : 0
    
                    Text {
                        anchors.centerIn: parent
                        text: Math.floor((root.currTotalKm / Math.pow(10, 4 - index)) % 10)
                        font.pixelSize: 15
                        font.bold: true
                        color: BaseTheme.gaugeMainTextInformation
                        verticalAlignment: Text.AlignVCenter
                        opacity: 0.6
                    }
                }
            }

            Rectangle {
                width: 16
                height: 20
                color: BaseTheme.gaugeBatteryEmpty
                topRightRadius: 4
                bottomRightRadius: 4
    
                Text {
                    anchors.centerIn: parent
                    text: Math.floor(root.currTotalKm % 10)
                    font.pixelSize: 15
                    font.bold: true
                    color: BaseTheme.gaugeMainTextInformation
                    verticalAlignment: Text.AlignVCenter
                    opacity: 0.75
                }
            }
        }

        Text {
            text: "km"
            font.pixelSize: 12
            color: BaseTheme.gaugeMainTextInformation
            opacity: 0.7
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.bottom
            anchors.topMargin: 4
        }
    }
}