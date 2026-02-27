import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0
import "../components"

Item {
    id: rightScreen
    Layout.fillWidth: true
    Layout.fillHeight: true

    Text {
        text: "Wheel Speed: " + powertrain.motorSpeed + "\n" +
                "Speed: " + vehicle.speed + "\n" +
                "Temperature: " + exterior.airTemperature + "\n" +
                "Front Distance: " + adas.frontDistance + "\n" +
                "Battery Voltage: " + powertrain.batteryVoltage + "\n" +
                "Battery Voltage Icon: " + powertrain.batteryVoltageIcon + "\n" +
                "Battery Level Low: " + powertrain.isBatteryLow + "\n" +
                "Battery Level Critical: " + powertrain.isBatteryCritical + "\n" +
                "Wheel Angle: " + chassis.steeringWheelAngle + "\n" +
                "Blinker Left: " + chassis.isBlinkerLeftActive + "\n" +
                "Blinker Right: " + chassis.isBlinkerRightActive + "\n" +
                "Heading: " + currentLocation.heading  
        font.pixelSize: 12
        color: "white"
        anchors.fill: parent
        anchors.margins: 20
        verticalAlignment: Text.AlignTop
        wrapMode: Text.WordWrap
    }
}