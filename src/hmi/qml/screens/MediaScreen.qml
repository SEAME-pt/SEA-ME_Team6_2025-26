import QtQuick
import QtMultimedia

Item {
    anchors.fill: parent
    anchors.margins: 16

    VideoOutput {
        id: videoOutput
        anchors.fill: parent
        Component.onCompleted: camera.setVideoSink(videoOutput.videoSink)
    }

    // Optional: connection status overlay
    Rectangle {
        id: noSignalOverlay
        anchors.fill: parent
        color: "#CC000000"
        visible: !videoOutput.videoSink

        Text {
            anchors.centerIn: parent
            text: "No camera signal"
            color: "white"
            font.pixelSize: 18
        }
    }
}