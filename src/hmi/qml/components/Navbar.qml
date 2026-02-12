import QtQuick 6.7
import QtQuick.Controls 6.7
import QtQuick.Layouts 6.7
import QtQuick.Effects
import ClusterTheme 1.0
import Cluster.Backend 1.0

Rectangle {
    id: navbar
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
                iconActive: "qrc:/assets/icons/grid.svg"
                iconInactive: "qrc:/assets/icons/grid.svg"
                active: false
                Layout.fillWidth: true
                Layout.fillHeight: true
                onClicked: {
                    navbar.setActiveNavButton(this)
                    changeScreen("../screens/HomeScreen.qml")
                }
            }
            
            NavButton {
                id: carButton
                iconActive: "qrc:/assets/icons/car.svg"
                iconInactive: "qrc:/assets/icons/car.svg"
                active: false
                Layout.fillWidth: true
                Layout.fillHeight: true
                onClicked: {
                    navbar.setActiveNavButton(this)
                    changeScreen("../screens/CarScreen.qml")
                }
            }
            
            NavButton {
                id: controllerButton
                iconActive: "qrc:/assets/icons/controller.svg"
                iconInactive: "qrc:/assets/icons/controller.svg"
                active: false
                Layout.fillWidth: true
                Layout.fillHeight: true
                onClicked: {
                    navbar.setActiveNavButton(this)
                    changeScreen("../screens/ControllerScreen.qml")
                }
            }
            
            NavButton {
                id: statisticsButton
                iconActive: "qrc:/assets/icons/statistics.svg"
                iconInactive: "qrc:/assets/icons/statistics.svg"
                active: false
                Layout.fillWidth: true
                Layout.fillHeight: true
                onClicked: {
                    navbar.setActiveNavButton(this)
                    changeScreen("../screens/StatisticsScreen.qml")
                }
            }
        }
        
        Rectangle {
            Layout.preferredWidth: 256
            Layout.fillHeight: true
            color: '#65b14e'
        }
    }
}
