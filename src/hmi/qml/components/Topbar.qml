import QtQuick 6.7
import QtQuick.Controls 6.7
import QtQuick.Layouts 6.7
import QtQuick.Effects
import ClusterTheme 1.0
import Cluster.Backend 1.0

Item {
    id: root
    Layout.preferredHeight: 120
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
        anchors.margins: 4
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
                        height: 18
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
                            font.pixelSize: 12
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
                                Layout.preferredHeight: 18
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
                                    font.pixelSize: 12
                                    font.bold: true
                                }
                            }

                            Text {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                text: Math.round(root.currSpeed) + " m/h"
                                color: BaseTheme.white
                                font.pixelSize: 20
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
                                Layout.preferredHeight: 18
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
                                    font.pixelSize: 12
                                    font.bold: true
                                }
                            }

                            Text {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                text: powertrain.batteryVoltage + "V"
                                color: BaseTheme.white
                                font.pixelSize: 20
                                font.bold: false
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                }
            }

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
                                Layout.preferredHeight: 18
                                radius: 4
                                color: "transparent"
                                border.color: BaseTheme.white
                                border.width: 1

                                Text {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 8
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: "RPM"
                                    color: BaseTheme.white
                                    font.pixelSize: 12
                                    font.bold: true
                                }
                            }

                            Text {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                text: Math.round(powertrain.motorSpeed)
                                color: BaseTheme.white
                                font.pixelSize: 20
                                font.bold: false
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }

                    Item {
                        id: durationRow
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        property int totalSeconds: vehicle.durationTrip
                        property int minutes: Math.floor(totalSeconds / 60)
                        property int seconds: totalSeconds % 60

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 4

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 18
                                radius: 4
                                color: "transparent"
                                border.color: BaseTheme.info
                                border.width: 1

                                Text {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 8
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: "TRIP DURATION"
                                    color: BaseTheme.info
                                    font.pixelSize: 12
                                    font.bold: true
                                }
                            }

                            Text {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                text: durationRow.minutes + "m " + durationRow.seconds + "s"
                                color: BaseTheme.white
                                font.pixelSize: 20
                                font.bold: false
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                }
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
                                font.pixelSize: 20
                                font.bold: true
                            }

                            Text {
                                Layout.fillWidth: true
                                leftPadding: 8
                                text: clock.date
                                color: BaseTheme.white
                                opacity: 0.45
                                font.pixelSize: 12
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
                                font.pixelSize: 20
                                font.bold: true
                            }

                            Text {
                                Layout.fillWidth: true
                                leftPadding: 8
                                text: currentLocation.heading + "°"
                                color: BaseTheme.white
                                opacity: 0.45
                                font.pixelSize: 12
                                font.bold: true
                            }
                        }
                    }
                }
            }

            Item {
                id: compassRoot
                Layout.fillWidth: true
                Layout.fillHeight: true
                anchors.leftMargin: 8

                // --- TEST MODE: simulates a vehicle driving around, turning at "junctions" ---
                // Set to false to go back to the real backend value.
                property bool testMode: true
                property real simHeading: 0
                property real simTarget: 0

                Timer {
                    id: headingSimTimer
                    interval: 150
                    running: compassRoot.testMode
                    repeat: true
                    onTriggered: {
                        // Occasionally decide to "turn" toward a new heading, like reaching a junction
                        if (Math.random() < 0.01)
                            compassRoot.simTarget = Math.random() * 360;

                        // Ease current heading toward target via shortest path (a gentle turn, not a snap)
                        var diff = compassRoot.simTarget - compassRoot.simHeading;
                        diff = ((diff + 180) % 360 + 360) % 360 - 180;
                        compassRoot.simHeading += diff * 0.05;

                        // Small jitter to mimic GPS/sensor noise while driving straight
                        compassRoot.simHeading += (Math.random() - 0.5) * 1.2;
                        compassRoot.simHeading = ((compassRoot.simHeading % 360) + 360) % 360;
                    }
                }

                // Raw heading coming from the backend (0-359, wraps around)
                property real heading: testMode ? simHeading : currentLocation.heading

                // Animated value actually used for the needle rotation.
                // We track it imperatively so we can always take the
                // shortest path around the circle (e.g. 350 -> 5 should
                // rotate forward 15 degrees, not spin backwards 345).
                property real displayHeading: heading

                onHeadingChanged: {
                    var diff = compassRoot.heading - compassRoot.displayHeading;
                    diff = ((diff + 180) % 360 + 360) % 360 - 180;
                    compassRoot.displayHeading += diff;
                }

                Behavior on displayHeading {
                    NumberAnimation {
                        duration: 400
                        easing.type: Easing.InOutQuad
                    }
                }

                //? OUTER RING
                Rectangle {
                    id: outerCircle
                    anchors.centerIn: parent
                    width: 112
                    height: 112
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
                    width: 104
                    height: 104
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

                //? COMPASS FACE (fixed ticks + cardinal/intercardinal labels)
                Item {
                    id: compassFace
                    anchors.centerIn: innerCircle
                    width: innerCircle.width
                    height: innerCircle.height

                    // Tick marks every 5 degrees (72 ticks)
                    Repeater {
                        model: 72
                        delegate: Item {
                            anchors.fill: parent
                            rotation: index * 5

                            Rectangle {
                                property bool isMajor: index % 18 === 0   // N/E/S/W
                                property bool isMid: !isMajor && index % 9 === 0 // NE/SE/SW/NW

                                anchors.horizontalCenter: parent.horizontalCenter
                                y: 4
                                width: isMajor ? 2 : 1
                                height: isMajor ? 9 : (isMid ? 6 : 3)
                                radius: 0.5
                                color: isMajor ? BaseTheme.white
                                              : (isMid ? BaseTheme.gaugeTicksActive
                                                       : BaseTheme.gaugeTicksInactive)
                                antialiasing: true
                            }
                        }
                    }

                    // Cardinal / intercardinal letters, kept upright
                    Repeater {
                        model: [
                            { label: "N", angle: 0 },
                            { label: "NE", angle: 45 },
                            { label: "E", angle: 90 },
                            { label: "SE", angle: 135 },
                            { label: "S", angle: 180 },
                            { label: "SW", angle: 225 },
                            { label: "W", angle: 270 },
                            { label: "NW", angle: 315 }
                        ]
                        delegate: Item {
                            anchors.fill: parent
                            rotation: modelData.angle

                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                y: 16
                                rotation: -modelData.angle
                                text: modelData.label
                                color: modelData.label === "N" ? BaseTheme.danger : BaseTheme.white
                                font.pixelSize: modelData.label.length === 1 ? 12 : 8
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }
                }

                //? NEEDLE (rotates with heading)
                Item {
                    id: needleGroup
                    anchors.centerIn: innerCircle
                    width: innerCircle.width
                    height: innerCircle.height
                    rotation: compassRoot.displayHeading

                    // Main hand, pointing toward current heading
                    Rectangle {
                        x: parent.width / 2 - width / 2
                        y: parent.height / 2 - height
                        width: 3
                        height: 32
                        radius: 1.5
                        color: BaseTheme.danger
                    }

                    // Short counterweight tail, opposite the hand
                    Rectangle {
                        x: parent.width / 2 - width / 2
                        y: parent.height / 2
                        width: 3
                        height: 10
                        radius: 1.5
                        color: BaseTheme.danger
                    }

                    // Center pivot
                    Rectangle {
                        x: parent.width / 2 - width / 2
                        y: parent.height / 2 - height / 2
                        width: 8
                        height: 8
                        radius: width / 2
                        color: BaseTheme.danger
                        border.color: BaseTheme.white
                        border.width: 1
                    }
                }
            }

        }
    }
}