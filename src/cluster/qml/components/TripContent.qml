import QtQuick
import QtQuick.Controls
import QtQuick.Shapes
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0

Item {
    id: tripContent
    anchors.fill: parent

    Column {
        anchors.centerIn: parent
        spacing: 12

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 4

            Text {
                text: vehicle.traveledDistanceTrip
                color: BaseTheme.white
                font.pixelSize: 18
                font.bold: true
            }

            Text {
                text: "Km"
                color: BaseTheme.white
                font.pixelSize: 14
                anchors.bottom: parent.bottom
            }
        }

        Rectangle {
            width: parent.width
            height: 2
            color: BaseTheme.gaugeTicksInactive
            opacity: 0.45
        }

        Row {
            spacing: 32

            Column {
                spacing: 2

                Text {
                    text: "MAX. SPEED"
                    font.pixelSize: 10
                    font.bold: true
                    color: BaseTheme.gaugeBattery
                }

                Row {
                    spacing: 4

                    Text {
                        text: vehicle.maxSpeedTrip
                        color: BaseTheme.white
                        font.pixelSize: 18
                        font.bold: true
                    }

                    Text {
                        text: "Km/h"
                        color: BaseTheme.white
                        font.pixelSize: 14
                        anchors.bottom: parent.bottom
                    }
                }
            }

            Column {
                spacing: 2

                Text {
                    text: "AVG. SPEED"
                    font.pixelSize: 10
                    font.bold: true
                    color: BaseTheme.gaugeBattery
                }

                Row {
                    spacing: 4

                    Text {
                        text: vehicle.averageSpeed
                        color: BaseTheme.white
                        font.pixelSize: 18
                        font.bold: true
                    }

                    Text {
                        text: "Km/h"
                        color: BaseTheme.white
                        font.pixelSize: 14
                        anchors.bottom: parent.bottom
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
            spacing: 32

            Column {
                spacing: 2

                Text {
                    text: "ENERGY kWh"
                    font.pixelSize: 10
                    font.bold: true
                    color: BaseTheme.gaugeTicksActive
                }

                Text {
                    text: vehicle.totalEnergyTrip
                    color: BaseTheme.white
                    font.pixelSize: 18
                    font.bold: true
                }
            }

            Column {
                spacing: 2

                Text {
                    text: "Wh/m"
                    font.pixelSize: 10
                    font.bold: true
                    color: BaseTheme.gaugeTicksActive
                }

                Text {
                    text: vehicle.energyPerKmTrip
                    color: BaseTheme.white
                    font.pixelSize: 18
                    font.bold: true
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
            id: durationRow
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 4

            property int totalSeconds: vehicle.durationTrip
            property int minutes: Math.floor(totalSeconds / 60)
            property int seconds: totalSeconds % 60

            Text {
                text: durationRow.minutes
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

            Text {
                text: durationRow.seconds < 10 ? "0" + durationRow.seconds : durationRow.seconds
                color: BaseTheme.white
                font.pixelSize: 18
                font.bold: true
            }

            Text {
                text: "s"
                color: BaseTheme.white
                font.pixelSize: 14
                anchors.bottom: parent.bottom
            }
        }
    }
}