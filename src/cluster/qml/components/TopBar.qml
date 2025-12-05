import QtQuick 6.7
import QtQuick.Controls 6.7
import QtQuick.Layouts 6.7
import ClusterTheme 1.0
import Cluster.Backend 1.0

Item {
    id: topBar
    Layout.preferredHeight: 40
    Layout.fillWidth: true

    TimeProvider {
        id: clock
    }

    Rectangle {
        id: base
        width: parent.width / 2
        height: 32
        anchors.horizontalCenter: parent.horizontalCenter
        color: BaseTheme.blackLight

        RowLayout {
            id: row
            anchors.fill: parent
            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "transparent"
                Text {
                    text: clock.currDate
                    font.pixelSize: 14
                    color: "white"
                    anchors.centerIn: parent
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "transparent"
                Text {
                    text: clock.currTime
                    font.pixelSize: 14
                    color: "white"
                    anchors.centerIn: parent
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "transparent"
            }
        }
    }
}
