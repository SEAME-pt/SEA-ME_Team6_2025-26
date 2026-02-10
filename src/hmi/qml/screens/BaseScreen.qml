import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ClusterTheme 1.0
// import "../components"

Rectangle {
    id: baseScreen
    color: BaseTheme.black
    anchors.fill: parent

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Top bar
        //TopBar {}

        // Middle content
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            Text {
                text: "HMI bonito para caralho"
                font.pixelSize: 12
                color: "white"
                anchors.centerIn: parent
            }
        }

        // Bottom bar
        // BottomBar {}
    }
}
