import QtQuick 6.7
import QtQuick.Controls 6.7
import QtQuick.Layouts 6.7
import ClusterTheme 1.0
import Cluster.Backend 1.0

Item {
    id: bottomBar
    Layout.preferredHeight: 48
    Layout.fillWidth: true
    Layout.margins: 0

    TimeProvider { id: clock }

    RowLayout {
        id: row
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Text {
                text: "67 Km"
                font.pixelSize: 12
                color: "white"
                anchors.centerIn: parent
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Text {
                text: clock.currTime
                font.pixelSize: 14
                color: "white"
                anchors.centerIn: parent
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            RowLayout {
                anchors.centerIn: parent
                spacing: 8

                Image {
                    source: "qrc:/assets/icons/danger.svg"
                    fillMode: Image.PreserveAspectFit
                    Layout.preferredWidth: 16
                    Layout.preferredHeight: 16
                    opacity: 0.1
                }

                Text {
                    text: "67 C"
                    font.pixelSize: 12
                    color: "white"
                }
            }
        }
    }
}