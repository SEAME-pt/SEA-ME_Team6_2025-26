import QtQuick
import QtQuick.Controls
import QtMultimedia
import QtQuick.Layouts
import ClusterTheme 1.0

Item {
    anchors.fill: parent
    anchors.margins: 16

    VideoOutput {
        id: videoOutput
        anchors.fill: parent
        Component.onCompleted: camera.setVideoSink(videoOutput.videoSink)
    }
}

/*

sudo apt install v4l2loopback-dkms ffmpeg

sudo modprobe -r v4l2loopback

sudo modprobe v4l2loopback \
  devices=1 \
  video_nr=2 \
  card_label="FakeCamera"

ffmpeg -stream_loop -1 -re -i video-teste.mp4 \
  -vf scale=1280:720 \
  -pix_fmt yuv420p \
  -f v4l2 /dev/video2

*/