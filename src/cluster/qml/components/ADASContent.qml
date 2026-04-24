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
    property var currActiveStates: [1, 1, 0, 0]
    //? Helpers
    property var activeStatesIcon: ["qrc:/assets/icons/lane-active.svg", "qrc:/assets/icons/object-active.svg", "qrc:/assets/icons/collision-active.svg", "qrc:/assets/icons/stop-active.svg"]
    property var inactiveStatesIcon: ["qrc:/assets/icons/lane-inactive.svg", "qrc:/assets/icons/object-inactive.svg", "qrc:/assets/icons/collision-inactive.svg", "qrc:/assets/icons/stop-inactive.svg"]

    function getStatesColor(index) {
        if (!root.currActiveStates[index])
            return BaseTheme.gaugeTicksInactive
        if (index == 0)
            return BaseTheme.success
        else if (index == 1)
            return BaseTheme.warning
        else
            return BaseTheme.danger
    }

    function getStatesOpacity(index) {
        if (root.currActiveStates[index])
            return 1
        return 0.45
    }

    function getStatesIcon(index) {
        if (!root.currActiveStates[index])
            return root.inactiveStatesIcon[index]
        return root.activeStatesIcon[index]
    }


    Column {
        anchors.centerIn: parent
        spacing: 10

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: actionText.width + 40
            height: 24
            radius: 16
            color: "transparent"
            border.color: BaseTheme.success
            border.width: 1

            Rectangle {
                anchors.left: parent.left
                anchors.leftMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                color: BaseTheme.success
                width: 8
                height: 8
                radius: 8
            }

            Text {
                id: actionText
                anchors.leftMargin: 24
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: "DEMO"
                color: BaseTheme.success
                font.pixelSize: 12
                font.bold: true
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
            spacing: 48

            Column {
                spacing: 6
                width: 52
                opacity: getStatesOpacity(0)

                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 32
                    height: 32
                    radius: 6
                    color: "transparent"
                    border.color: getStatesColor(0)
                    border.width: 1

                    Image {
                        source: getStatesIcon(0)
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        width: 24
                        height: 24
                        sourceSize.width: 24
                        sourceSize.height: 24
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                        mipmap: true
                    }
                }

                Text {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    text: "LANE"
                    color: getStatesColor(0)
                    font.pixelSize: 12
                    font.bold: true
                }
            }

            Column {
                spacing: 6
                width: 52
                opacity: getStatesOpacity(1)

                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 32
                    height: 32
                    radius: 6
                    color: "transparent"
                    border.color: getStatesColor(1)
                    border.width: 1

                    Image {
                        source: getStatesIcon(1)
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        width: 24
                        height: 24
                        sourceSize.width: 24
                        sourceSize.height: 24
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                        mipmap: true
                    }
                }

                Text {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    text: "OBJECT"
                    color: getStatesColor(1)
                    font.pixelSize: 12
                    font.bold: true
                }
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 48

            Column {
                spacing: 6
                width: 52
                opacity: getStatesOpacity(2)

                Rectangle {
                    id: collisionWarning
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 32
                    height: 32
                    radius: 6
                    color: "transparent"
                    border.color: getStatesColor(2)
                    border.width: 1

                    Image {
                        source: getStatesIcon(2)
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        width: 24
                        height: 24
                        sourceSize.width: 24
                        sourceSize.height: 24
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                        mipmap: true
                    }
                }

                Text {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    text: "IMPACT"
                    color: getStatesColor(2)
                    font.pixelSize: 12
                    font.bold: true
                }
            }

            Column {
                spacing: 6
                width: 52
                opacity: getStatesOpacity(3)

                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 32
                    height: 32
                    radius: 6
                    color: "transparent"
                    border.color: getStatesColor(3)
                    border.width: 1

                    Image {
                        source: getStatesIcon(3)
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        width: 24
                        height: 24
                        sourceSize.width: 24
                        sourceSize.height: 24
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                        mipmap: true
                    }
                }

                Text {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    text: "STOP"
                    color: getStatesColor(3)
                    font.pixelSize: 12
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
            spacing: 32

            Column {
                spacing: 2

                Text {
                    text: "FPS"
                    font.pixelSize: 10
                    font.bold: true
                    color: BaseTheme.gaugeMainTextInformation
                }

                Text {
                    text: "67"
                    color: BaseTheme.white
                    font.pixelSize: 18
                    font.bold: true
                }
            }

            Column {
                spacing: 2

                Text {
                    text: "ºC"
                    font.pixelSize: 10
                    font.bold: true
                    color: BaseTheme.gaugeMainTextInformation
                }

                Text {
                    text: "67"
                    color: BaseTheme.white
                    font.pixelSize: 18
                    font.bold: true
                }
            }
        }
    }

    SequentialAnimation {
        running: root.currActiveStates[2]
        loops: Animation.Infinite

        NumberAnimation {
            target: collisionWarning
            property: "opacity"
            to: 1
            duration: 200
        }
        PauseAnimation { duration: 600 }
        NumberAnimation {
            target: collisionWarning
            property: "opacity"
            to: 0.45
            duration: 200
        }
        PauseAnimation { duration: 600 }
    }
}