import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ClusterTheme 1.0

Item {
    id: navBar
    Layout.preferredWidth: 80
    Layout.fillHeight: true

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 16

        Rectangle {
            id: homeButton
            width: 48
            height: 48
            radius: 8
            color: BaseTheme.white
            Layout.alignment: Qt.AlignHCenter
        }

        Rectangle {
            id: clusterButton
            width: 48
            height: 48
            radius: 8
            color: BaseTheme.white
            Layout.alignment: Qt.AlignHCenter
        }

        Rectangle {
            id: settingsButton
            width: 48
            height: 48
            radius: 8
            color: BaseTheme.white
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
