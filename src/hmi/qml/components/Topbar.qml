import QtQuick 6.7
import QtQuick.Controls 6.7
import QtQuick.Layouts 6.7
import QtQuick.Effects
import ClusterTheme 1.0
import Cluster.Backend 1.0

Item {
    id: topbar
    Layout.preferredHeight: 32
    Layout.fillWidth: true

    TimeProvider { id: clock }

    RowLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.rightMargin: 32
        anchors.leftMargin: 32
        spacing: 16

        RowLayout {
            Layout.alignment: Qt.AlignLeft
            spacing: 8

            Text {
                text: clock.currTime
                font.pixelSize: 12
                font.bold: true
                color: BaseTheme.black
            }

            Text {
                text: clock.currDate
                font.pixelSize: 10
                color: BaseTheme.black
                opacity: 0.8
            }
        }

        Item {
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight
            spacing: 16

            Image {
                source: "qrc:/assets/icons/bluetooth.svg"
                width: 16
                height: 16
                sourceSize.width: 16
                sourceSize.height: 16
                fillMode: Image.PreserveAspectFit
                smooth: true
                mipmap: true
            }

            Image {
                source: "qrc:/assets/icons/wifi.svg"
                width: 16
                height: 16
                sourceSize.width: 16
                sourceSize.height: 16
                fillMode: Image.PreserveAspectFit
                smooth: true
                mipmap: true
            }

            Image {
                source: "qrc:/assets/icons/battery.svg"
                width: 16
                height: 16
                sourceSize.width: 16
                sourceSize.height: 16
                fillMode: Image.PreserveAspectFit
                smooth: true
                mipmap: true
            }
        }
    }
}