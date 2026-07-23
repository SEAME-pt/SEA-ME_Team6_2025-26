import QtQuick
import QtQuick.Controls
import QtQuick.Shapes
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0

Item {
    id: root
    anchors.fill: parent
    //? Data
    property real lateralDeviationValue: adas.lateralDeviation
    property string laneStatusValue: adas.laneStatus
    //? Helpers
    property real lateralDeviationTicks: 33
    property real lateralDeviationMax: 50.0
    property int lateralDeviationMiddleIndex: Math.floor(lateralDeviationTicks / 2)
    property real lateralDeviationClamped: Math.max(-lateralDeviationMax, Math.min(lateralDeviationMax, lateralDeviationValue))
    property int activatedUpTo: Math.round((lateralDeviationClamped / lateralDeviationMax + 1) / 2 * (lateralDeviationTicks - 1))
    property bool leftLaneDetected:  laneStatusValue === "both" || laneStatusValue === "left"
    property bool rightLaneDetected: laneStatusValue === "both" || laneStatusValue === "right"
    property bool laneWarningActive: !leftLaneDetected && !rightLaneDetected

    Behavior on lateralDeviationValue {
        NumberAnimation {
            duration: 500
            easing.type: Easing.InOutQuad
        }
    }

    function getLateralDeviationColor(index) {
        if (lateralDeviationValue == 0 || index === lateralDeviationMiddleIndex) {
            return BaseTheme.gaugeMainTextInformation
        } else if (lateralDeviationValue >= 0) {
            return (index >= lateralDeviationMiddleIndex && index <= activatedUpTo)
                ? BaseTheme.gaugeTicksActive
                : BaseTheme.gaugeTicksInactive
        } else {
            return (index <= lateralDeviationMiddleIndex && index >= activatedUpTo)
                ? BaseTheme.gaugeTicksActive
                : BaseTheme.gaugeTicksInactive
        }
    }

    function getLateralDeviationWidth(index) {
        if (index === activatedUpTo)
            return 6
        return 2
    }

    function getLateralDeviationHeight(index) {
        if (index == lateralDeviationMiddleIndex)
            return 13
        return 9
    }

    function isTickActive(index) {
        if (lateralDeviationValue >= 0)
            return index >= lateralDeviationMiddleIndex && index <= activatedUpTo
        else
            return index <= lateralDeviationMiddleIndex && index >= activatedUpTo
    }

    Column {
        anchors.centerIn: parent
        spacing: 12

        Row {
            spacing: 10

            Column {
                spacing: 4

                Text {
                    text: "DETECTION"
                    font.pixelSize: 10
                    font.bold: true
                    color: BaseTheme.gaugeMainTextInformation
                }

                Row {
                    spacing: 4

                    Rectangle {
                        width: 40
                        height: 16
                        radius: 4
                        color: leftLaneDetected ? BaseTheme.gaugeTicksActive : "transparent"
                        border.width: 1
                        border.color: BaseTheme.gaugeTicksActive
                        opacity: leftLaneDetected ? 1.0 : 0.45
                    }

                    Rectangle {
                        width: 40
                        height: 16
                        radius: 4
                        color: rightLaneDetected ? BaseTheme.gaugeTicksActive : "transparent"
                        border.width: 1
                        border.color: BaseTheme.gaugeTicksActive
                        opacity: rightLaneDetected ? 1.0 : 0.45
                    }
                }
            }

            Item { width: 16; height: 1 }

            Image {
                id: noLaneWarning
                visible: laneWarningActive
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:/assets/icons/warning.svg"
                width: 32
                height: 32
                sourceSize.width: 32
                sourceSize.height: 32
                fillMode: Image.PreserveAspectFit
                smooth: true
                mipmap: true
            }
        }

        Rectangle {
            width: parent.width
            height: 2
            color: BaseTheme.gaugeTicksInactive
            opacity: 0.45
        }

        Row {
            id: deviationRow
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 24

            Column {
                spacing: 4

                Text {
                    text: "LATERAL DEVIATION"
                    font.pixelSize: 10
                    font.bold: true
                    color: BaseTheme.gaugeMainTextInformation
                }

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 3

                    Repeater {
                        id: deviationTicks
                        model: lateralDeviationTicks

                        delegate: Rectangle {
                            width: getLateralDeviationWidth(index)
                            height: getLateralDeviationHeight(index)
                            color: getLateralDeviationColor(index)
                            opacity: isTickActive(index) || index === activatedUpTo ? 1 : 0.45
                        }
                    }
                }
            }
        }

        Rectangle {
            width: parent.width
            height: 2
            color: BaseTheme.gaugeTicksInactive
            opacity: 0.45
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 24

            Column {
                spacing: 4

                Text {
                    text: "DEVIATION HISTORY"
                    font.pixelSize: 10
                    font.bold: true
                    color: BaseTheme.gaugeMainTextInformation
                }

                Canvas {
                    id: deviationGraph
                    width: deviationRow.width
                    height: 40

                    property string watchDeviation: lateralDeviationValue
                    onWatchDeviationChanged: requestPaint()

                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.clearRect(0, 0, width, height)

                        var history = adas.lateralDeviationHistory
                        //var history = root.debugDeviationHistory
                        if (history.length < 2) return

                        var maxDeviation = root.lateralDeviationMax
                        var midY = height / 2

                        ctx.beginPath()
                        ctx.strokeStyle = BaseTheme.gaugeTicksActive
                        ctx.lineWidth = 1.5

                        for (var i = 0; i < history.length; i++) {
                            var x = (i / (history.length - 1)) * width
                            var y = midY - (history[i] / maxDeviation) * midY
                            if (i === 0)
                                ctx.moveTo(x, y)
                            else
                                ctx.lineTo(x, y)
                        }

                        ctx.stroke()

                        ctx.beginPath()
                        ctx.strokeStyle = BaseTheme.gaugeTicksInactive
                        ctx.lineWidth = 0.5
                        ctx.setLineDash([4, 4])
                        ctx.moveTo(0, midY)
                        ctx.lineTo(width, midY)
                        ctx.stroke()
                        ctx.setLineDash([])
                    }
                }
            }
        }
    }

    SequentialAnimation {
        running: laneWarningActive
        loops: Animation.Infinite

        NumberAnimation {
            target: noLaneWarning
            property: "opacity"
            to: 1
            duration: 200
        }
        PauseAnimation { duration: 600 }
        NumberAnimation {
            target: noLaneWarning
            property: "opacity"
            to: 0.45
            duration: 200
        }
        PauseAnimation { duration: 600 }
    }
}