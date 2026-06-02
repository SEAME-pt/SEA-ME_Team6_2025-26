import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0
import Cluster.Backend 1.0
import "../components"

Item {
    id: centerScreen
    Layout.fillWidth: true
    Layout.fillHeight: true
    property int speedLimitDetected: adas.speedLimit

    ExtraProvider { id: clock }

    // Behavior on speedLimitDetected {
    //     NumberAnimation {
    //         duration: 1000
    //         easing.type: Easing.InOutQuad
    //     }
    // }

    function getSpeedLimitSignal(val) {
        if (val == 50)
            return "qrc:/assets/icons/50-speed-limit.svg"
        else if (val == 80)
            return "qrc:/assets/icons/80-speed-limit.svg"
        return ""
    }

    /*
    * currSpeed:        vehicle.speed (Current Speed of the vehicle)
    * currBattery:      powertrain.batteryVoltage (Current Voltage of the Battery)
    * currBatteryIcon:  powertrain.batterVoltageIcon (Icon for UI)
    * isBatteryWarning: powertrain.isBatteryLow (Indicator that the battery is low)
    * isBatteryDanger:  powertrain.isBatteryCritical (Indicator that the battery is critical)
    */
    MiddleGauge {
        anchors.fill: parent
        currSpeed: vehicle.speed
        currBattery: parseFloat(powertrain.batteryVoltage) || 0.0
        currBatteryIcon: powertrain.batteryVoltageIcon
        isBatteryWarning: powertrain.isBatteryLow
        isBatteryDanger: powertrain.isBatteryCritical
        currAutonomy: powertrain.batteryVoltage
    }

    Item {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 40
        anchors.leftMargin: 8

        Text {
            text: exterior.airTemperature + "°C"
            color: BaseTheme.white
            font.bold: true
            font.pixelSize: 14
        }
    }

    Item {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 32
        anchors.leftMargin: -64

        Blinker {
            activeBlinkerSource: "qrc:/assets/icons/left-arrow-active.png"
            inactiveBlinkerSource: "qrc:/assets/icons/left-arrow.png"
            isActive: chassis.isBlinkerLeftActive
        }
    }

    Item {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 40
        anchors.leftMargin: centerScreen.width - timeText.width - 8

        Text {
            id: timeText
            text: clock.time
            color: BaseTheme.white
            font.bold: true
            font.pixelSize: 14
        }
    }

    Item {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 32
        anchors.leftMargin: centerScreen.width - timeText.width + 64

        Blinker {
            activeBlinkerSource: "qrc:/assets/icons/right-arrow-active.png"
            inactiveBlinkerSource: "qrc:/assets/icons/right-arrow.png"
            isActive: chassis.isBlinkerRightActive
        }
    }

    Item {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 96
        anchors.leftMargin: centerScreen.width - timeText.width + 48

        Image {
            id: speedLimitIcon
            anchors.centerIn: parent
            source: getSpeedLimitSignal(centerScreen.speedLimitDetected)
                width: 48
                height: 48
                sourceSize.width: 48
                sourceSize.height: 48
                fillMode: Image.PreserveAspectFit
                smooth: true
                mipmap: true

            onSourceChanged: {
                fadeIn.restart()
            }

            SequentialAnimation {
                id: fadeIn
                NumberAnimation { target: speedLimitIcon; property: "opacity"; to: 0; duration: 100 }
                NumberAnimation { target: speedLimitIcon; property: "opacity"; to: 1; duration: 750 }
            }
        }
    }
}