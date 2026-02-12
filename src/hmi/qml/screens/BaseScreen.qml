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

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: '#4a8ca0'
        }

        Navbar {  }
    }
}
