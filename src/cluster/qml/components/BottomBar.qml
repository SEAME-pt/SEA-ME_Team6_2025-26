import QtQuick 6.7
import QtQuick.Controls 6.7
import QtQuick.Layouts 6.7
import ClusterTheme 1.0
import Cluster.Backend 1.0

Item {
    id: bottomBar
    Layout.preferredHeight: 40
    Layout.fillWidth: true
    Layout.margins: 0

    TimeProvider {
        id: clock
    }

    Rectangle {
        id: base
        width: parent.width / 2
        height: 32
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        color: BaseTheme.blackLight

        RowLayout {
            id: row
            anchors.fill: parent
            spacing: 12

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "transparent"
                Image {
                    source: "qrc:/assets/images/critical-techworks-logo.png"
                    sourceSize.width: 22
                    sourceSize.height: 22
                    anchors.centerIn: parent
                    fillMode: Image.PreserveAspectFit
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "transparent"
                Image {
                    source: "qrc:/assets/images/seame-logo.png"
                    sourceSize.width: 16
                    sourceSize.height: 16
                    anchors.centerIn: parent
                    fillMode: Image.PreserveAspectFit
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "transparent"
                Image {
                    source: "qrc:/assets/images/eclipse-fundation-logo.png"
                    sourceSize.width: 16
                    sourceSize.height: 16
                    anchors.centerIn: parent
                    fillMode: Image.PreserveAspectFit
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "transparent"
                Image {
                    source: "qrc:/assets/images/brisa-logo.png"
                    sourceSize.width: 22
                    sourceSize.height: 22
                    anchors.centerIn: parent
                    fillMode: Image.PreserveAspectFit
                }
            }
        }
    }
}
