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
                text: "0"
                color: BaseTheme.white
                font.pixelSize: 18
                font.bold: true
            }

            Text {
                text: "km"
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
                        text: "0"
                        color: BaseTheme.white
                        font.pixelSize: 18
                        font.bold: true
                    }

                    Text {
                        text: "km/h"
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
                        text: "0"
                        color: BaseTheme.white
                        font.pixelSize: 18
                        font.bold: true
                    }

                    Text {
                        text: "km/h"
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
                    text: "0"
                    color: BaseTheme.white
                    font.pixelSize: 18
                    font.bold: true
                }
            }

            Column {
                spacing: 2

                Text {
                    text: "Wh/km"
                    font.pixelSize: 10
                    font.bold: true
                    color: BaseTheme.gaugeTicksActive
                }

                Text {
                    text: "0"
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
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 4

            Text {
                text: "00"
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
                text: "00"
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