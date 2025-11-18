import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ClusterTheme 1.0

Item {
    id: navBar
    Layout.preferredWidth: 64
    Layout.fillHeight: true

    signal changeScreen(string page)

    function setActiveNavButton(button) {
        for (var i = 0; i < layout.children.length; i++) {
            var child = layout.children[i]
            if (child.hasOwnProperty("active")) {
                child.active = (child === button)
            }
        }
    }

    ColumnLayout {
        id: layout
        anchors.fill: parent
        anchors.topMargin: 16
        anchors.bottomMargin: 16
        anchors.leftMargin: 4
        anchors.rightMargin: 8
        spacing: 16

        NavButton {
            id: homeBtn
            iconInactive: "qrc:/assets/icons/house-solid-full.svg"
            iconActive: "qrc:/assets/icons/house-active-solid-full.svg"
            active: false
            onClicked: {
                navBar.setActiveNavButton(this)
                changeScreen("../screens/HomeScreen.qml")
            }
        }

        NavButton {
            id: clusterBtn
            iconInactive: "qrc:/assets/icons/car-solid-full.svg"
            iconActive: "qrc:/assets/icons/car-active-solid-full.svg"
            active: true
            onClicked: {
                navBar.setActiveNavButton(this)
                changeScreen("../screens/ClusterScreen.qml")
            }
        }

        Item {
            Layout.fillHeight: true
        }

        NavButton {
            id: settingsBtn
            iconInactive: "qrc:/assets/icons/gear-solid-full.svg"
            iconActive: "qrc:/assets/icons/gear-active-solid-full.svg"
            active: false
            onClicked: {
                navBar.setActiveNavButton(this)
                changeScreen("../screens/SettingsScreen.qml")
            }
        }
    }
}
