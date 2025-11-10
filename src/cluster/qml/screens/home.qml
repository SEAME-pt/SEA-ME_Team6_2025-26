import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ClusterTheme 1.0
import Cluster.Backend 1.0

Rectangle {
    id: root
    anchors.fill: parent
    color: BaseTheme.black

    TimeProvider {
        id: clock
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        //? Navbar
        ColumnLayout {
            Layout.preferredWidth: 80
            Layout.fillHeight: true
            spacing: 16

            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                width: 48
                height: 48
                radius: 8
                color: BaseTheme.white
            }
            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                width: 48
                height: 48
                radius: 8
                color: BaseTheme.white
            }
            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                width: 48
                height: 48
                radius: 8
                color: BaseTheme.white
            }
            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                width: 48
                height: 48
                radius: 8
                color: BaseTheme.white
            }
        }

        //? Main Content
        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 0
            Layout.rightMargin: 16
            //? Top-Bar
            Rectangle {
                Layout.preferredHeight: 32
                Layout.fillWidth: true
                color: "transparent"
                Text {
                    anchors.centerIn: parent
                    text: clock.currTime
                    color: BaseTheme.white
                    font.bold: true
                    font.pixelSize: 14
                }
            }
            //? Content
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: BaseTheme.white
                radius: 8
            }
            //? Bottom-Bar
            Rectangle {
                Layout.preferredHeight: 32
                Layout.fillWidth: true
                opacity: 0
            }
        }
    }
}
