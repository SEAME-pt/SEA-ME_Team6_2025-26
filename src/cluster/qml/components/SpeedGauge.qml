import QtQuick
import QtQuick.Controls
import QtQuick.Shapes
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0

Item {
    id: root
    width: 243
    height: 243

    property real value: 0
    property real minValue: 0
    property real maxValue: 40

    property real startAngle: 135
    property real endAngle: 270

    function sweepForValue(v) {
        var t = Math.max(0, Math.min(1,(v - root.minValue) / (root.maxValue - root.minValue)))
        return root.endAngle * t
    }

    // Background Arc
    Shape {
        anchors.fill: parent
        antialiasing: true
        layer.enabled: true
        layer.smooth: true
        ShapePath {
            strokeWidth: 18
            strokeColor: BaseTheme.blackLight
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap
            PathMove {
                x: width / 2 + Math.cos(
                       startAngle * Math.PI / 180) * (width / 2 - 10)
                y: height / 2 + Math.sin(
                       startAngle * Math.PI / 180) * (height / 2 - 10)
            }
            PathAngleArc {
                centerX: width / 2
                centerY: height / 2
                radiusX: width / 2 - 10
                radiusY: height / 2 - 10
                startAngle: root.startAngle
                sweepAngle: root.endAngle
            }
        }
    }

    // Filled Arc
    Shape {
        id: fillArc
        visible: false
        anchors.fill: parent
        antialiasing: true
        layer.enabled: true
        layer.smooth: true
        ShapePath {
            strokeWidth: 18
            strokeColor: BaseTheme.white
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap
            PathMove {
                x: width / 2 + Math.cos(root.startAngle * Math.PI / 180) * (width / 2 - 10)
                y: height / 2 + Math.sin(root.startAngle * Math.PI / 180) * (height / 2 - 10)
            }
            PathAngleArc {
                centerX: width / 2
                centerY: height / 2
                radiusX: width / 2 - 10
                radiusY: height / 2 - 10
                startAngle: root.startAngle
                sweepAngle: sweepForValue(root.value)
            }
        }
    }

    MultiEffect {
        source: fillArc
        anchors.fill: fillArc
        shadowEnabled: true
        brightness: 0.4
            saturation: 0.2
            blurEnabled: true
            blurMax: 8
            blur: 1.0
    }


    // Value label
    Text {
        text: Math.round(value)
        anchors.centerIn: parent
        font.pixelSize: 64
        font.bold: true
        color: "white"
    }

    // Animate changes
    Behavior on value {
        NumberAnimation {
            duration: 300
            easing.type: Easing.InOutQuad
        }
    }
}
