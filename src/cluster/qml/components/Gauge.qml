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
    property real centerValue: 0
    property real centerMinValue: 0
    property real centerMaxValue: 1000
    property real centerStartAngle: 135
    property real centerEndAngle: 270
    property string centerUnit: ""
    property var centerTicks: []

    property var showBottom: false
    property real bottomValue: 0
    property real bottomMinValue: 9
    property real bottomMaxValue: 13
    property real bottomStartAngle: (root.centerStartAngle + root.centerEndAngle) % 360 + 30
    property real bottomSweepAngle: 360 - root.centerEndAngle - 60
    property string middleIcon: ""
    property string middleText: ""

    function getMiddleColor(icon) {
        if (middleIcon == "qrc:/assets/icons/flash.svg")
            return BaseTheme.white
        else if (middleIcon == "qrc:/assets/icons/flash-warning.svg")
            return BaseTheme.warning
        return BaseTheme.danger
    }

    function sweepValue(v, minVal, maxVal, endAngle) {
        var t = Math.max(0, Math.min(1,(v - minVal) / (maxVal - minVal)))
        return endAngle * t
    }

    // Inner circle
    Rectangle {
        id: innerCircle
        anchors.centerIn: parent
        width: 245
        height: 245
        radius: width / 2
        color: "#0a0a0a"
    }

    // Background Center Arc
    Shape {
        anchors.fill: parent
        antialiasing: true
        layer.enabled: true
        layer.smooth: true
        
        ShapePath {
            strokeWidth: 21
            strokeColor: BaseTheme.blackLight
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap
            PathMove {
                x: root.width / 2 + Math.cos(root.centerStartAngle * Math.PI / 180) * (root.width / 2 - 10)
                y: root.height / 2 + Math.sin(root.centerStartAngle * Math.PI / 180) * (root.height / 2 - 10)
            }
            PathAngleArc {
                centerX: root.width / 2
                centerY: root.height / 2
                radiusX: root.width / 2 - 10
                radiusY: root.height / 2 - 10
                startAngle: root.centerStartAngle
                sweepAngle: root.centerEndAngle
            }
        }
    }

    // Fill Center Arc
    Shape {
        id: fillArc
        anchors.fill: parent
        antialiasing: true
        layer.enabled: true
        layer.smooth: true
        
        ShapePath {
            strokeWidth: 21
            strokeColor: BaseTheme.white
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap
            PathMove {
                x: root.width / 2 + Math.cos(root.centerStartAngle * Math.PI / 180) * (root.width / 2 - 10)
                y: root.height / 2 + Math.sin(root.centerStartAngle * Math.PI / 180) * (root.height / 2 - 10)
            }
            PathAngleArc {
                centerX: root.width / 2
                centerY: root.height / 2
                radiusX: root.width / 2 - 10
                radiusY: root.height / 2 - 10
                startAngle: root.centerStartAngle
                sweepAngle: sweepValue(root.centerValue, root.centerMinValue, root.centerMaxValue, root.centerEndAngle)
            }
        }
    }

    // Tick marks and numbers
    Repeater {
        model: root.centerTicks
        
        Item {
            id: tickItem
            anchors.fill: parent
            property real tickValue: modelData
            property real normalizedValue: (tickValue - root.centerMinValue) / (root.centerMaxValue - root.centerMinValue)
            property real angle: root.centerStartAngle + normalizedValue * root.centerEndAngle
            property real tickRadius: root.width / 2 - 24
            property real numberRadius: root.width / 2 - 42
            
            // Tick mark
            Rectangle {
                width: 3
                height: 6
                color: "white"
                radius: 1
                x: root.width / 2 + Math.cos(tickItem.angle * Math.PI / 180) * tickItem.tickRadius - width / 2
                y: root.height / 2 + Math.sin(tickItem.angle * Math.PI / 180) * tickItem.tickRadius - height / 2
                rotation: tickItem.angle + 90
                transformOrigin: Item.Center
            }
            
            // Number label
            Text {
                text: tickItem.tickValue
                font.pixelSize: 12
                font.weight: Font.Normal
                color: "white"
                opacity: 0.8
                x: root.width / 2 + Math.cos(tickItem.angle * Math.PI / 180) * tickItem.numberRadius - width / 2
                y: root.height / 2 + Math.sin(tickItem.angle * Math.PI / 180) * tickItem.numberRadius - height / 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    // Center content
    Column {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -4
        spacing: -10
        
        // Value label
        Text {
            text: Math.round(root.centerValue)
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 70
            font.bold: true
            color: "white"
            
            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "white"
                shadowBlur: 0.3
                shadowHorizontalOffset: 0
                shadowVerticalOffset: 0
            }
        }
        
        // Unit label
        Text {
            text: root.centerUnit
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 18
            color: "white"
            opacity: 0.7
        }
    }

    // Animate changes
    Behavior on centerValue {
        NumberAnimation {
            duration: 500
            easing.type: Easing.InOutQuad
        }
    }

    Column {
        visible: root.showBottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 32
        spacing: 4

        // Icon
        Image {
            source: root.middleIcon
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode: Image.PreserveAspectFit
            width: 24
            height: 24

            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: getMiddleColor(root.middleIcon)
                shadowBlur: 0.3
                shadowScale: 1.2
                shadowHorizontalOffset: 0
                shadowVerticalOffset: 0
            }
        }
        
        // Text
        Text {
            text: root.middleText
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 12
            font.weight: Font.Medium
            color: getMiddleColor(root.middleIcon)
            opacity: 0.8

            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: getMiddleColor(root.middleIcon)
                shadowBlur: 0.3
                shadowHorizontalOffset: 0
                shadowVerticalOffset: 0
            }
        }
    }

    Behavior on middleIcon {
        NumberAnimation {
            duration: 500
            easing.type: Easing.InOutQuad
        }
    }

    // Background Bottom arc
    Shape {
        visible: root.showBottom
        anchors.fill: parent
        antialiasing: true
        layer.enabled: true
        layer.smooth: true
        
        ShapePath {
            strokeWidth: 21
            strokeColor: BaseTheme.blackLight
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap
            PathMove {
                x: root.width / 2 + Math.cos(root.bottomStartAngle * Math.PI / 180) * (root.width / 2 - 10)
                y: root.height / 2 + Math.sin(root.bottomStartAngle * Math.PI / 180) * (root.height / 2 - 10)
            }
            PathAngleArc {
                centerX: root.width / 2
                centerY: root.height / 2
                radiusX: root.width / 2 - 10
                radiusY: root.height / 2 - 10
                startAngle: root.bottomStartAngle
                sweepAngle: root.bottomSweepAngle
            }
        }
    }

    // Bottom filled arc
    Shape {
        id: bottomFillArc
        visible: root.showBottom
        anchors.fill: parent
        antialiasing: true
        layer.enabled: true
        layer.smooth: true
        
        ShapePath {
            strokeWidth: 21
            strokeColor: BaseTheme.primaryLight
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap
            PathMove {
                x: root.width / 2 + Math.cos((root.bottomStartAngle + root.bottomSweepAngle) * Math.PI / 180) * (root.width / 2 - 10)
                y: root.height / 2 + Math.sin((root.bottomStartAngle + root.bottomSweepAngle) * Math.PI / 180) * (root.height / 2 - 10)
            }
            PathAngleArc {
                centerX: root.width / 2
                centerY: root.height / 2
                radiusX: root.width / 2 - 10
                radiusY: root.height / 2 - 10
                startAngle: root.bottomStartAngle + root.bottomSweepAngle
                sweepAngle: -sweepValue(root.bottomValue, root.bottomMinValue, root.bottomMaxValue, root.bottomSweepAngle)
            }
        }
    }

    Text {
        visible: root.showBottom
        text: "E"
        font.pixelSize: 16
        font.weight: Font.Bold
        color: "white"
        z: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        anchors.left: parent.left
        anchors.leftMargin: root.width * 0.26
        
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "white"
            shadowBlur: 0.3
            shadowHorizontalOffset: 0
            shadowVerticalOffset: 0
        }
    }

    Text {
        visible: root.showBottom
        text: "F"
        font.pixelSize: 16
        font.weight: Font.Bold
        color: "white"
        z: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        anchors.right: parent.right
        anchors.rightMargin: root.width * 0.26
        
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "white"
            shadowBlur: 0.3
            shadowHorizontalOffset: 0
            shadowVerticalOffset: 0
        }
    }

    Behavior on bottomValue {
        NumberAnimation {
            duration: 500
            easing.type: Easing.InOutQuad
        }
    }

}
