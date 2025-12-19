import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ClusterTheme 1.0
import Cluster.Backend 1.0
import QtQuick.Effects

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
        bottomLeftRadius: 16
        bottomRightRadius: 16

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
                    font.pixelSize: 10
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

    MultiEffect {
        source: base
        anchors.fill: base
        shadowEnabled: true
        shadowColor: "#90ffffff"
        shadowScale: 1
        shadowOpacity: 0.8
        shadowVerticalOffset: 6
        shadowBlur: 1
    }

}
