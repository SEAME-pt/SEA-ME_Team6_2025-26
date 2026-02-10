import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0
import Cluster.Backend 1.0

Item {
    id: centerScreen
    Layout.fillWidth: true
    Layout.fillHeight: true


    Text {
        text: "Speed: " + speedProvider.currSpeed + "\n" +
              "Temperature: " + temperatureProvider.currTemperature + "\n" +
              "Wheel Speed: " + wheelSpeedProvider.currWheelSpeed + "\n" +
              "Front Distance: " + frontDistanceProvider.currFrontDistance + "\n" +
              "Voltage: " + voltageProvider.currVoltage + "\n" +
              "VoltageIcon: " + voltageProvider.voltageIcon + "\n" +
              "Heading: " + headingProvider.currHeading  
        font.pixelSize: 12
        color: "white"
        anchors.centerIn: parent
    }
}
