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
        Layout.fillWidth: true
        Layout.fillHeight: true
        spacing: 0

        LeftScreen {}

        CenterScreen {}

        RightScreen {}
    }
}
