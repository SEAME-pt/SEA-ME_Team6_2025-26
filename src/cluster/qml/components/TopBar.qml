import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ClusterTheme 1.0
import Cluster.Backend 1.0
import QtQuick.Effects

Item {
    id: root
    height: 48
    Layout.fillWidth: true
    Layout.margins: 0

    property var leftBlinkerActive: false
    property real magnetometerValue: 0
    property var rightBlinkerActive: false

    property real effectShadowOpacity: 0.8

    TimeProvider { id: clock }

    RowLayout {
        id: row
        anchors.fill: parent
        spacing: 0

        // Left BLinker
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Image {
                id: leftBlinker
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.top: parent.top
                anchors.topMargin: 32
                source: root.leftBlinkerActive ? "qrc:/assets/icons/left-arrow-active.png" : "qrc:/assets/icons/left-arrow.png"
                fillMode: Image.PreserveAspectFit
                smooth: true
                mipmap: true
                sourceSize.width: 32
                sourceSize.height: 32
                opacity: root.leftBlinkerActive ? 1 : 0.2

                layer.enabled: root.leftBlinkerActive
                layer.effect: MultiEffect {
                    id: leftBlinkerEffect
                    width: 40
                    height: 40
                    shadowEnabled: true
                    shadowColor: BaseTheme.vibrant
                    shadowBlur: 0.2
                    shadowScale: 1.35
                    shadowOpacity: root.effectShadowOpacity
                    shadowHorizontalOffset: 0
                    shadowVerticalOffset: 0
                }

                SequentialAnimation {
                    running: root.leftBlinkerActive
                    loops: Animation.Infinite

                    NumberAnimation {
                        target: leftBlinker
                        property: "opacity"
                        to: 1
                        duration: 0
                    }
                    PauseAnimation { duration: 500 }
                    NumberAnimation {
                        target: leftBlinker
                        property: "opacity"
                        to: 0.3
                        duration: 150
                    }
                    PauseAnimation { duration: 500 }
                }

                SequentialAnimation {
                    running: root.leftBlinkerActive
                    loops: Animation.Infinite

                    NumberAnimation {
                        target: root
                        property: "effectShadowOpacity"
                        to: 0.8
                        duration: 0
                    }
                    PauseAnimation { duration: 500 }
                    NumberAnimation {
                        target: root
                        property: "effectShadowOpacity"
                        to: 0
                        duration: 150
                    }
                    PauseAnimation { duration: 500 }
                }
            }
        }

        // Compass
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            RowLayout {
                anchors.centerIn: parent
                spacing: 4

                Image {
                    source: "qrc:/assets/icons/compass.svg"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    mipmap: true
                    sourceSize.width: 16
                    sourceSize.height: 16
                    opacity: 1
                }

                Text {
                    text: root.magnetometerValue + "°"
                    font.pixelSize: 12
                    color: "white"
                }
            }
        }

        // Right BLinker
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Image {
                id: rightBlinker
                anchors.right: parent.right
                anchors.rightMargin: 16
                anchors.top: parent.top
                anchors.topMargin: 32
                source: root.rightBlinkerActive ? "qrc:/assets/icons/right-arrow-active.png" : "qrc:/assets/icons/right-arrow.png"
                fillMode: Image.PreserveAspectFit
                smooth: true
                mipmap: true
                sourceSize.width: 32
                sourceSize.height: 32
                opacity: root.rightBlinkerActive ? 1 : 0.2

                layer.enabled: root.rightBlinkerActive
                layer.effect: MultiEffect {
                    id: rightBlinkerEffect
                    width: 40
                    height: 40
                    shadowEnabled: true
                    shadowColor: BaseTheme.vibrant
                    shadowBlur: 0.6
                    shadowScale: 1.35
                    shadowOpacity: root.effectShadowOpacity
                    shadowHorizontalOffset: 0
                    shadowVerticalOffset: 0
                }

                SequentialAnimation {
                    running: root.rightBlinkerActive
                    loops: Animation.Infinite

                    NumberAnimation {
                        target: rightBlinker
                        property: "opacity"
                        to: 1
                        duration: 0
                    }
                    PauseAnimation { duration: 500 }
                    NumberAnimation {
                        target: rightBlinker
                        property: "opacity"
                        to: 0.3
                        duration: 150
                    }
                    PauseAnimation { duration: 500 }
                }

                SequentialAnimation {
                    running: root.rightBlinkerActive
                    loops: Animation.Infinite

                    NumberAnimation {
                        target: root
                        property: "effectShadowOpacity"
                        to: 0.8
                        duration: 0
                    }
                    PauseAnimation { duration: 500 }
                    NumberAnimation {
                        target: root
                        property: "effectShadowOpacity"
                        to: 0
                        duration: 150
                    }
                    PauseAnimation { duration: 500 }
                }
            }
        }
    }
}