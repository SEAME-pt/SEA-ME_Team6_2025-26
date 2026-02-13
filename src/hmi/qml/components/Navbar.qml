import QtQuick 6.7
import QtQuick.Controls 6.7
import QtQuick.Layouts 6.7
import QtQuick.Effects
import ClusterTheme 1.0
import Cluster.Backend 1.0

Rectangle {
    id: navbar
    color: BaseTheme.white
    Layout.preferredHeight: 64
    Layout.fillWidth: true
    Layout.margins: 0

    signal changeScreen(string page)

    function setActiveNavButton(button)
    {
        for (var i = 0; i < buttonRow.children.length; i++) {
            var child = buttonRow.children[i]
            if (child.hasOwnProperty("active"))
                child.active = (child === button)
        }
    }

    RowLayout {
        id: mainLayout
        anchors.fill: parent
        spacing: 0

        RowLayout {
            id: buttonRow
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            NavButton {
                id: homeButton
                iconActive: "qrc:/assets/icons/grid-active.svg"
                iconInactive: "qrc:/assets/icons/grid-inactive.svg"
                active: true
                Layout.fillWidth: true
                Layout.fillHeight: true
                onClicked: {
                    navbar.setActiveNavButton(this)
                    changeScreen("qrc:/qml/screens/HomeScreen.qml")
                }
            }

            NavButton {
                id: carButton
                iconActive: "qrc:/assets/icons/car-active.svg"
                iconInactive: "qrc:/assets/icons/car-inactive.svg"
                active: false
                Layout.fillWidth: true
                Layout.fillHeight: true
                onClicked: {
                    navbar.setActiveNavButton(this)
                    changeScreen("qrc:/qml/screens/CarScreen.qml")
                }
            }

            NavButton {
                id: controllerButton
                iconActive: "qrc:/assets/icons/controller-active.svg"
                iconInactive: "qrc:/assets/icons/controller-inactive.svg"
                active: false
                Layout.fillWidth: true
                Layout.fillHeight: true
                onClicked: {
                    navbar.setActiveNavButton(this)
                    changeScreen("qrc:/qml/screens/ControllerScreen.qml")
                }
            }

            NavButton {
                id: statisticsButton
                iconActive: "qrc:/assets/icons/statistics-active.svg"
                iconInactive: "qrc:/assets/icons/statistics-inactive.svg"
                active: false
                Layout.fillWidth: true
                Layout.fillHeight: true
                onClicked: {
                    navbar.setActiveNavButton(this)
                    changeScreen("qrc:/qml/screens/StatisticsScreen.qml")
                }
            }

            NavButton {
                id: settingsButton
                iconActive: "qrc:/assets/icons/settings-active.svg"
                iconInactive: "qrc:/assets/icons/settings-inactive.svg"
                active: false
                Layout.fillWidth: true
                Layout.fillHeight: true
                onClicked: {
                    navbar.setActiveNavButton(this)
                    changeScreen("qrc:/qml/screens/SettingsScreen.qml")
                }
            }
        }

        Rectangle {
            Layout.preferredWidth: 256
            Layout.fillHeight: true
            color: BaseTheme.white

            Text {
                anchors.centerIn: parent
                text: "WeatherAPI to be added"
                font.pixelSize: 12
                font.bold: true
                color: BaseTheme.black
            }
        }
    }

    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        z: 1
        color: "#E0E0E0"
    }
}
