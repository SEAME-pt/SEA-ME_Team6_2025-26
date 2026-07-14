import QtQuick
import QtQuick.Controls
import QtMultimedia
import QtQuick.Layouts
import ClusterTheme 1.0

Item {
    anchors.fill: parent
    anchors.margins: 16

    Button {
        text: "Update Now"

        onClicked: {
            console.log("[Button] Clicked")
            ota.triggerUpdate()
        }
    }
}
