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
    property real frontDistanceValue: adas.frontDistance

    //? Helpers
    property real warningThreshold: 250.0
    property real criticalThreshold: 100.0
    property bool isWarning: frontDistanceValue <= warningThreshold
    property bool isCritical: frontDistanceValue <= criticalThreshold

    property int distanceTicks: 15
    property real distanceMax: warningThreshold
    property real distanceClamped: Math.max(0, Math.min(distanceMax, frontDistanceValue))
    property int activatedUpTo: Math.round((1 - distanceClamped / distanceMax) * (distanceTicks - 1))

    Behavior on frontDistanceValue {
        NumberAnimation {
            duration: 500
            easing.type: Easing.InOutQuad
        }
    }

    function getDistanceColor(index) {
        if (index > activatedUpTo)
            return BaseTheme.gaugeTicksInactive
        return isCritical ? BaseTheme.gaugeBatteryEmpty : BaseTheme.gaugeTicksActive
    }

    function getDistanceHeight(index) {
        if (index === activatedUpTo)
            return 13
        return 9
    }

    Column {
        anchors.centerIn: parent
        spacing: 12

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 10

            Image {
                id: warningIcon
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

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "FORWARD COLLISION"
                color: BaseTheme.white
                font.pixelSize: 14
                font.bold: true
            }
        }

        Rectangle {
            width: parent.width
            height: 2
            color: BaseTheme.gaugeTicksInactive
            opacity: 0.45
        }

        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 4

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "FRONT DISTANCE"
                font.pixelSize: 10
                font.bold: true
                color: BaseTheme.gaugeMainTextInformation
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 4

                Text {
                    text: frontDistanceValue.toFixed(1)
                    color: BaseTheme.white
                    font.pixelSize: 18
                    font.bold: true
                }

                Text {
                    text: "m"
                    color: BaseTheme.white
                    font.pixelSize: 14
                    anchors.bottom: parent.bottom
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
            id: distanceRow
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 3

            Repeater {
                id: distanceBar
                model: distanceTicks

                delegate: Rectangle {
                    width: 6
                    height: getDistanceHeight(index)
                    color: getDistanceColor(index)
                    opacity: index <= activatedUpTo ? 1 : 0.45
                }
            }
        }
    }

    SequentialAnimation {
        running: isCritical
        loops: Animation.Infinite

        NumberAnimation {
            target: warningIcon
            property: "opacity"
            to: 1
            duration: 200
        }
        PauseAnimation { duration: 300 }
        NumberAnimation {
            target: warningIcon
            property: "opacity"
            to: 0.45
            duration: 200
        }
        PauseAnimation { duration: 300 }
    }
}