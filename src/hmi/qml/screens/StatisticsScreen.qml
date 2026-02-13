import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0

Item {

    GridLayout {
        anchors.fill: parent
        anchors.leftMargin: 32
        anchors.rightMargin: 32
        anchors.bottomMargin: 32
        columns: 3
        rows: 3
        columnSpacing: 16
        rowSpacing: 16

        // Top Left (1x1)
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: BaseTheme.white
            radius: 8

            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#20000000"
                shadowBlur: 0.3
                shadowHorizontalOffset: 0
                shadowVerticalOffset: 1
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 32
                spacing: 8

                Text {
                    Layout.alignment: Qt.AlignLeft
                    Layout.leftMargin: 8
                    Layout.topMargin: 8
                    text: "Top Level"
                    font.pixelSize: 14
                    font.bold: true
                    color: BaseTheme.black
                }

                Text {
                    Layout.alignment: Qt.AlignLeft
                    Layout.leftMargin: 8
                    text: "Speed: " + speedProvider.currSpeed +
                            "\nAverageSpeed: - \nTraveledDistance: - \nTraveledDistanceSinceStart: - \nIsMoving: -" 
                    font.pixelSize: 12
                    color: BaseTheme.black
                }
            }

        }

        // Middle (1x2 tall)
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.rowSpan: 2
            color: BaseTheme.white
            radius: 8

            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#20000000"
                shadowBlur: 0.3
                shadowHorizontalOffset: 0
                shadowVerticalOffset: 1
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 32
                spacing: 8

                Text {
                    Layout.alignment: Qt.AlignLeft
                    Layout.leftMargin: 8
                    Layout.topMargin: 8
                    text: "Powertrain"
                    font.pixelSize: 14
                    font.bold: true
                    color: BaseTheme.black
                }

                Text {
                    Layout.alignment: Qt.AlignLeft
                    Layout.leftMargin: 8
                    text: "Type: -\nElectricMotor.IsRunning: - \nElectricMotor.Speed:" + wheelSpeedProvider.currWheelSpeed +
                        "\nElectricMotor.Power: - \nElectricMotor.Temperature: - \nTransmission.CurrentGear: - \nTransmission.DriveType: -" +
                        "\nTractionBattery.Id: - \nTractionBattery.NominalVoltage: - \nTractionBattery.MaxVoltage: - \nTractionBattery.GrossCapacity: -" +
                        "\nTractionBattery.CurrentVoltage: " + voltageProvider.currVoltage + 
                        "\nTractionBattery.CurrentCurrent: - \nTractionBattery.CurrentPower: - \nTractionBattery.StateOfCharge.Current: -"  +
                        "\nTractionBattery.StateOfCharge.Displayed: - \nTractionBattery.Range: - \nTractionBattery.IsLevelLow: - \nTractionBattery.IsCritical: -"
                    font.pixelSize: 12
                    color: BaseTheme.black
                }
            }
        }

        // Right (1x2 tall)
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.rowSpan: 2
            color: BaseTheme.white
            radius: 8

            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#20000000"
                shadowBlur: 0.3
                shadowHorizontalOffset: 0
                shadowVerticalOffset: 1
            }
            
            ColumnLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 32
                spacing: 8

                Text {
                    Layout.alignment: Qt.AlignLeft
                    Layout.leftMargin: 8
                    Layout.topMargin: 8
                    text: "Chassis"
                    font.pixelSize: 14
                    font.bold: true
                    color: BaseTheme.black
                }

                Text {
                    Layout.alignment: Qt.AlignLeft
                    Layout.leftMargin: 8
                    text: "SteeringWheel.Angle: " + wheelAngleProvider.currWheelAngle +
                        "\nAccelerator.PedalPosition: - \nBrake.PedalPosition: - "
                    color: BaseTheme.black
                }

                Text {
                    Layout.alignment: Qt.AlignLeft
                    Layout.leftMargin: 8
                    Layout.topMargin: 8
                    text: "ADAS"
                    font.pixelSize: 14
                    font.bold: true
                    color: BaseTheme.black
                }

                Text {
                    Layout.alignment: Qt.AlignLeft
                    Layout.leftMargin: 8
                    text: "ObstacleDetection.IsEnabled: - \nObstacleDetection.Front.IsWarning: - " +
                        "\nObstacleDetection.Front.Distance: " + frontDistanceProvider.currFrontDistance +
                        "\nActiveAutonomyLevel: -"
                    color: BaseTheme.black
                }
            }
        }

        // Bottom Left (1x1)
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: BaseTheme.white
            radius: 8

            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#20000000"
                shadowBlur: 0.3
                shadowHorizontalOffset: 0
                shadowVerticalOffset: 1
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 32
                spacing: 8

                Text {
                    Layout.alignment: Qt.AlignLeft
                    Layout.leftMargin: 8
                    Layout.topMargin: 8
                    text: "Spatial Acceleration/Rotation"
                    font.pixelSize: 14
                    font.bold: true
                    color: BaseTheme.black
                }

                Text {
                    Layout.alignment: Qt.AlignLeft
                    Layout.leftMargin: 8
                    text: "Acceleration.Longitudinal: -\nAcceleration.Lateral: - \nAcceleration.Vertical: - \nAngularVelocity.Roll: - \nAngularVelocity.Pitch: - \nAngularVelocity.Yaw: - \n" 
                    font.pixelSize: 12
                    color: BaseTheme.black
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: BaseTheme.white
            radius: 8

            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#20000000"
                shadowBlur: 0.3
                shadowHorizontalOffset: 0
                shadowVerticalOffset: 1
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 32
                spacing: 8

                Text {
                    Layout.alignment: Qt.AlignLeft
                    Layout.leftMargin: 8
                    Layout.topMargin: 8
                    text: "Location / Exterior"
                    font.pixelSize: 14
                    font.bold: true
                    color: BaseTheme.black
                }

                Text {
                    Layout.alignment: Qt.AlignLeft
                    Layout.leftMargin: 8
                    text: "CurrentLocation.Heading: " + headingProvider.currHeading +
                        "\nExterior.AirTemperature: " + temperatureProvider.currTemperature +
                        "\nExterior.Humidity: - \nExterior.LightIntensity: - \nExterior.AtmosphericPressure: -"
                    color: BaseTheme.black
                }
            }
        }

        // Rectangle {
        //     Layout.fillWidth: true
        //     Layout.fillHeight: true
        //     color: BaseTheme.white
        //     radius: 8

        //     layer.enabled: true
        //     layer.effect: MultiEffect {
        //         shadowEnabled: true
        //         shadowColor: "#20000000"
        //         shadowBlur: 0.3
        //         shadowHorizontalOffset: 0
        //         shadowVerticalOffset: 1
        //     }
        // }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: BaseTheme.white
            radius: 8

            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#20000000"
                shadowBlur: 0.3
                shadowHorizontalOffset: 0
                shadowVerticalOffset: 1
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 32
                spacing: 8

                Text {
                    Layout.alignment: Qt.AlignLeft
                    Layout.leftMargin: 8
                    Layout.topMargin: 8
                    text: "Extras"
                    font.pixelSize: 14
                    font.bold: true
                    color: BaseTheme.black
                }

                Text {
                    Layout.alignment: Qt.AlignLeft
                    Layout.leftMargin: 8
                    text: "VehicleIdentification.Brand: -\nVehicleIdentification.Model: - \nVehicleIdentification.Year: - \nVersionVSS.Major: - \nVersionVSS.Minor: - \nVersionVSS.Patch: - \nVersionVSS.Label: -" 
                    font.pixelSize: 12
                    color: BaseTheme.black
                }
            }
        }
    }
}