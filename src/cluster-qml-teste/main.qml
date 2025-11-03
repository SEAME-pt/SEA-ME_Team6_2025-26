import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Imagine
import QtQuick.Window


ApplicationWindow {
    visible: true
    title: qsTr("SEA:ME Cluster Team 6")

    Column {
        anchors.centerIn: parent
        spacing: 10

        Text {
            text: "SEA:ME Cluster Team 6"
            color: "black"
            font.pixelSize: 20
        }

        Row {
            spacing: 10

            Text {
                id: dateItem
                font.pixelSize: 12
                color: "black"
            }

            Text {
                id: timeItem
                font.pixelSize: 12
                font.bold: true
                color: "black"
            }
        }

        Timer {
            running: true
            repeat: true
            interval: 1000
            triggeredOnStart: true
            onTriggered: {
                var weekday = new Date().toLocaleDateString(Qt.locale("pt_PT"), "dddd");
                var date = new Date().toLocaleDateString(Qt.locale("pt_PT"), Locale.ShortFormat);
                dateItem.text = weekday + " " + date;
                timeItem.text = new Date().toLocaleTimeString(Qt.locale("pt_PT"), Locale.ShortFormat);
            }
        }
    }
}

//400x1028 