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

        // Total Distance
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                anchors.centerIn: parent
                spacing: 4

                Text {
                    text: "Total"
                    font.pixelSize: 12
                    font.bold: true
                    color: BaseTheme.white
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    text: "67 Km"
                    font.pixelSize: 12
                    color: "white"
                    Layout.alignment: Qt.AlignHCenter
                }
            }
        }

        // Time
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                anchors.centerIn: parent
                spacing: 4

                Text {
                    text: "Time"
                    font.pixelSize: 12
                    font.bold: true
                    color: BaseTheme.white
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    text: clock.currTime
                    font.pixelSize: 14
                    color: "white"
                    Layout.alignment: Qt.AlignHCenter
                }
            }
        }

        // Temperature
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                anchors.centerIn: parent
                spacing: 4

                Text {
                    text: "Temp"
                    font.pixelSize: 12
                    font.bold: true
                    color: BaseTheme.white
                    Layout.alignment: Qt.AlignHCenter
                }

                RowLayout {
                    spacing: 4
                    Layout.alignment: Qt.AlignHCenter

                    Image {
                        source: "qrc:/assets/icons/temperature.svg"
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                        mipmap: true
                        sourceSize.width: 22
                        sourceSize.height: 22
                        opacity: 1
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
}