import QtQuick 6.7
import QtQuick.Controls 6.7
import QtQuick.Layouts 6.7
import QtQuick.Effects
import ClusterTheme 1.0
import Cluster.Backend 1.0

Item {
    id: root
    Layout.preferredHeight: 64
    Layout.fillWidth: true
    Layout.margins: 0

    signal changeScreen(string page)

    function setActiveNavButton(button) {
        for (var i = 0; i < buttonRow.children.length; i++) {
            var child = buttonRow.children[i]
            if (child.hasOwnProperty("active"))
                child.active = (child === button)
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 0

        RowLayout {
            id: buttonRow
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            NavButton {
                id: mediaScreen
                iconSrc: "qrc:/assets/icons/camera.svg"
                active: true
                Layout.fillWidth: true
                Layout.fillHeight: true
                onClicked: {
                    root.setActiveNavButton(this)
                    changeScreen("qrc:/qml/screens/MediaScreen.qml")
                }
            }

            NavButton {
                id: statisticsScreen
                iconSrc: "qrc:/assets/icons/statistics.svg"
                active: false
                Layout.fillWidth: true
                Layout.fillHeight: true
                onClicked: {
                    root.setActiveNavButton(this)
                    changeScreen("qrc:/qml/screens/StatisticsScreen.qml")
                }
            }
        }
    }
}