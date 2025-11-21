import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ClusterTheme 1.0
import "../components"

Rectangle {
    id: baseScreen
    color: BaseTheme.black
    anchors.fill: parent

    RowLayout {
        anchors.fill: parent
        spacing: 0

        NavBar {
            id: navBar
            onChangeScreen: function(page){
                pageLoader.source = page;
            }
        }

        Rectangle {
            id: contentBackground
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: BaseTheme.blackLight
            radius: 16
            clip: true

            Rectangle {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                width: 16
                color: parent.color
                radius: 0
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 0

                TopBar {
                    Layout.bottomMargin: 16
                }

                Loader {
                    id: pageLoader
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    source: "../screens/ClusterScreen.qml"
                }
            }
        }
    }
}
