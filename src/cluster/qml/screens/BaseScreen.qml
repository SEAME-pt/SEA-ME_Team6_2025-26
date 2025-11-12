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

        NavBar { }

        ColumnLayout {
            Layout.fillHeight: true; Layout.fillWidth: true
            Layout.rightMargin: 16
            spacing: 0

            TopBar { }

            Loader {
                id: pageLoader
                Layout.fillWidth: true; Layout.fillHeight: true
                source: "../screens/ClusterScreen.qml"
            }

            Rectangle {
                Layout.preferredHeight: 32
                Layout.fillWidth: true
                opacity: 0
            }
        }
    }
}
