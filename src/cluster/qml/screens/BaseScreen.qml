import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ClusterTheme 1.0
import "../components"

Rectangle {
    id: baseScreen
    color: BaseTheme.black
    anchors.fill: parent

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Top bar
        TopBar {}

        // Middle content
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            LeftScreen {}

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Rectangle {
                    anchors.fill: parent
                    color: baseScreen.color
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Rectangle {
                    anchors.fill: parent
                    color: baseScreen.color
                }
            }
        }

        // Bottom bar
        BottomBar {}
    }
}
