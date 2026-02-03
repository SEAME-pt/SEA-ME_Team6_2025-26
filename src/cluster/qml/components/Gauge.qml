import QtQuick
import QtQuick.Controls
import QtQuick.Shapes
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0

Item {
    id: root
    width: 275
    height: 275
    property real centerValue: 0
    property real centerMinValue: 0
    property real centerMaxValue: 1000
    property real centerStartAngle: 135
    property real centerEndAngle: 270
    property string centerUnit: "m/h"
    property var centerTicks: []

    property var showBottom: false
    property real bottomValue: 0
    property real bottomMinValue: 9
    property real bottomMaxValue: 13
    property real bottomStartAngle: (root.centerStartAngle + root.centerEndAngle) % 360 + 25
    property real bottomSweepAngle: 360 - root.centerEndAngle - 50

    function sweepValue(v, minVal, maxVal, endAngle) {
        var t = Math.max(0, Math.min(1,(v - minVal) / (maxVal - minVal)))
        return endAngle * t
    }

    // Outer glow/shadow for the entire gauge
    Rectangle {
        anchors.centerIn: parent
        width: parent.width + 20
        height: parent.height + 20
        radius: width / 2
        color: "transparent"
        
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "#60000000"
            shadowBlur: 0.4
            shadowHorizontalOffset: 0
            shadowVerticalOffset: 0
        }
    }

    // Inner shadow background circle
    Rectangle {
        anchors.centerIn: parent
        width: parent.width - 50
        height: parent.height - 50
        radius: width / 2
        color: "#0a0a0a"
    }

    // Background Arc with shadow
    Shape {
        anchors.fill: parent
        antialiasing: true
        layer.enabled: true
        layer.smooth: true
        
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "#40000000"
            shadowBlur: 0.2
            shadowHorizontalOffset: 0
            shadowVerticalOffset: 2
        }
        
        ShapePath {
            strokeWidth: 20
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

    // Filled Arc with glow effect
    Shape {
        id: fillArc
        visible: true
        anchors.fill: parent
        antialiasing: true
        layer.enabled: true
        layer.smooth: true
        
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "#60ffffff"
            shadowBlur: 0.5
            shadowHorizontalOffset: 0
            shadowVerticalOffset: 0
        }
        
        ShapePath {
            strokeWidth: 20
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
            
            // Tick mark with subtle shadow
            Rectangle {
                width: 2
                height: 6
                color: "white"
                radius: 1
                x: root.width / 2 + Math.cos(tickItem.angle * Math.PI / 180) * tickItem.tickRadius - width / 2
                y: root.height / 2 + Math.sin(tickItem.angle * Math.PI / 180) * tickItem.tickRadius - height / 2
                rotation: tickItem.angle + 90
                transformOrigin: Item.Center
                
                layer.enabled: true
                layer.effect: MultiEffect {
                    shadowEnabled: true
                    shadowColor: "#30ffffff"
                    shadowBlur: 0.2
                    shadowHorizontalOffset: 0
                    shadowVerticalOffset: 0
                }
            }
            
            // Number label with text shadow
            Text {
                text: tickItem.tickValue
                font.pixelSize: 12
                font.weight: Font.Normal
                color: "white"
                x: root.width / 2 + Math.cos(tickItem.angle * Math.PI / 180) * tickItem.numberRadius - width / 2
                y: root.height / 2 + Math.sin(tickItem.angle * Math.PI / 180) * tickItem.numberRadius - height / 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                
                layer.enabled: true
                layer.effect: MultiEffect {
                    shadowEnabled: true
                    shadowColor: "#60000000"
                    shadowBlur: 0.3
                    shadowHorizontalOffset: 0
                    shadowVerticalOffset: 1
                }
            }
        }
    }

    // Center content
    Column {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 0
        spacing: -10
        
        // Value label with prominent glow
        Text {
            text: Math.round(root.centerValue)
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 75
            font.bold: true
            color: "white"
            
            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#80ffffff"
                shadowBlur: 0.6
                shadowHorizontalOffset: 0
                shadowVerticalOffset: 0
            }
        }
        
        // Unit label with subtle shadow
        Text {
            text: root.centerUnit
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 18
            color: "white"
            opacity: 0.7
            
            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#40000000"
                shadowBlur: 0.3
                shadowHorizontalOffset: 0
                shadowVerticalOffset: 1
            }
        }
    }

    // Animate changes
    Behavior on centerValue {
        NumberAnimation {
            duration: 300
            easing.type: Easing.InOutQuad
        }
    }

    // Bottom arc background with shadow
    Shape {
        visible: root.showBottom
        anchors.fill: parent
        antialiasing: true
        layer.enabled: true
        layer.smooth: true
        
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "#30000000"
            shadowBlur: 0.2
            shadowHorizontalOffset: 0
            shadowVerticalOffset: 1
        }
        
        ShapePath {
            strokeWidth: 20
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

    // Bottom filled arc with glow
    Shape {
        id: bottomFillArc
        visible: root.showBottom
        anchors.fill: parent
        antialiasing: true
        layer.enabled: true
        layer.smooth: true
        
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: Qt.rgba(
                BaseTheme.primaryLight.r,
                BaseTheme.primaryLight.g,
                BaseTheme.primaryLight.b,
                0.5
            )
            shadowBlur: 0.5
            shadowHorizontalOffset: 0
            shadowVerticalOffset: 0
        }
        
        ShapePath {
            strokeWidth: 20
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
        anchors.bottomMargin: 16
        anchors.left: parent.left
        anchors.leftMargin: root.width * 0.24
        
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "#60000000"
            shadowBlur: 0.3
            shadowHorizontalOffset: 0
            shadowVerticalOffset: 1
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
        anchors.bottomMargin: 16
        anchors.right: parent.right
        anchors.rightMargin: root.width * 0.24
        
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "#60000000"
            shadowBlur: 0.3
            shadowHorizontalOffset: 0
            shadowVerticalOffset: 1
        }
    }

    Behavior on bottomValue {
        NumberAnimation {
            duration: 500
            easing.type: Easing.InOutQuad
        }
    }

}
