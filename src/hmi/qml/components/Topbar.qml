import QtQuick 6.7
import QtQuick.Controls 6.7
import QtQuick.Layouts 6.7
import QtQuick.Effects
import ClusterTheme 1.0
import Cluster.Backend 1.0

Item {
    id: root
    Layout.preferredHeight: 160
    Layout.fillWidth: true
    property real currSpeed: 0

    Behavior on currSpeed {
        NumberAnimation {
            duration: 500
            easing.type: Easing.InOutQuad
        }
    }

    ExtraProvider { id: clock }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 0

        // Logo and User information
        Item {
            Layout.preferredWidth: 128
            Layout.fillHeight: true
            Layout.leftMargin: 8

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 4

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Image {
                        source: "qrc:/assets/images/seame-logo.png"
                        anchors.centerIn: parent
                        Layout.preferredWidth: 32
                        Layout.preferredHeight: 16
                        sourceSize.width: 32
                        sourceSize.height: 16
                        fillMode: Image.PreserveAspectFit
                        smooth: false
                        mipmap: false
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Rectangle {
                        anchors.left: parent.left
                        width: parent.width
                        height: 25
                        radius: 4
                        color: "transparent"
                        border.color: BaseTheme.danger
                        border.width: 1

                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 8
                            anchors.verticalCenter: parent.verticalCenter
                            text: "TEAM 6"
                            color: BaseTheme.danger
                            font.pixelSize: 14
                            font.bold: true
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                anchors.leftMargin: 8

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 4

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 4

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 25
                                radius: 4
                                color: "transparent"
                                border.color: BaseTheme.gaugeBattery
                                border.width: 1

                                Text {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 8
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: "SPEED"
                                    color: BaseTheme.gaugeBattery
                                    font.pixelSize: 14
                                    font.bold: true
                                }
                            }

                            Text {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                text: Math.round(root.currSpeed) + " km/h"
                                color: BaseTheme.white
                                font.pixelSize: 28
                                font.bold: false
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 4

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 25
                                radius: 4
                                color: "transparent"
                                border.color: BaseTheme.gaugeTicksActive
                                border.width: 1

                                Text {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 8
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: "BATTERY"
                                    color: BaseTheme.gaugeTicksActive
                                    font.pixelSize: 14
                                    font.bold: true
                                }
                            }

                            Text {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                text: "0 kW"
                                color: BaseTheme.white
                                font.pixelSize: 28
                                font.bold: false
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                }
            }

            // GRÁFICOS
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }

        Rectangle {
            Layout.preferredWidth: 2
            Layout.fillHeight: true
            color: BaseTheme.gaugeTicksInactive
            opacity: 0.45
        }

        RowLayout {
            Layout.preferredWidth: 352
            Layout.maximumWidth: 352
            Layout.fillWidth: false
            Layout.fillHeight: true
            spacing: 0

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                anchors.leftMargin: 8

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 4

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        ColumnLayout {
                            anchors.left: parent.left
                            anchors.top: parent.top
                            width: parent.width
                            spacing: 0

                            Text {
                                Layout.fillWidth: true
                                leftPadding: 8
                                text: clock.time
                                color: BaseTheme.white
                                font.pixelSize: 28
                                font.bold: true
                            }

                            Text {
                                Layout.fillWidth: true
                                leftPadding: 8
                                text: clock.date
                                color: BaseTheme.white
                                opacity: 0.45
                                font.pixelSize: 14
                                font.bold: true
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        ColumnLayout {
                            anchors.left: parent.left
                            anchors.top: parent.top
                            width: parent.width
                            spacing: 0

                            Text {
                                Layout.fillWidth: true
                                leftPadding: 8
                                text: exterior.airTemperature + "°C"
                                color: BaseTheme.white
                                font.pixelSize: 28
                                font.bold: true
                            }

                            Text {
                                Layout.fillWidth: true
                                leftPadding: 8
                                text: "(Humidity)"
                                color: BaseTheme.white
                                opacity: 0.45
                                font.pixelSize: 14
                                font.bold: true
                            }

                            Text {
                                Layout.fillWidth: true
                                leftPadding: 8
                                text: currentLocation.heading + "°"
                                color: BaseTheme.white
                                opacity: 0.45
                                font.pixelSize: 14
                                font.bold: true
                            }
                        }
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                anchors.leftMargin: 8

                //? OUTER RING
                Rectangle {
                    id: outerCircle
                    anchors.centerIn: parent
                    width: 144
                    height: 144
                    radius: width / 2
                    color: BaseTheme.sportBlack
                    layer.enabled: true
                    layer.live: false
                    layer.smooth: false
                    layer.effect: MultiEffect {
                        shadowEnabled: true
                        shadowColor: BaseTheme.white
                        shadowBlur: 1.0
                        shadowOpacity: 0.6
                        shadowHorizontalOffset: 0
                        shadowVerticalOffset: 0
                        shadowScale: 1.01
                    }
                }

                //? GAUGE CONTENT AREA
                Rectangle {
                    id: innerCircle
                    anchors.centerIn: parent
                    width: 135
                    height: 135
                    radius: width / 2
                    color: BaseTheme.darkBlack
                    border.color: BaseTheme.blackboard
                    border.width: 2
                    layer.enabled: true
                    layer.live: false
                    layer.smooth: false
                    layer.effect: MultiEffect {
                        shadowEnabled: true
                        shadowColor: BaseTheme.white
                        shadowBlur: 1.0
                        shadowOpacity: 0.9
                        shadowHorizontalOffset: 0
                        shadowVerticalOffset: 0
                        shadowScale: 0.98
                    }
                }

                Text {
                    anchors.centerIn: parent
                    text: "To be added"
                    color: BaseTheme.white
                    font.pixelSize: 14
                    font.bold: true
                }
            }

        }
    }
}