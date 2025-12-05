import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0
import Cluster.Backend 1.0

Item {
    id: centerScreen
    Layout.fillWidth: true
    Layout.fillHeight: true

    Rectangle {
        anchors.centerIn: parent
        width: 60
        height: 60
        radius: 10
        color: "steelblue"
        id: normalRect
    }

    MultiEffect {
        source: normalRect
        anchors.fill: normalRect
        shadowEnabled: true
        shadowColor: "#90ffffff"
        shadowScale: 1
        shadowOpacity: 0.8
        shadowVerticalOffset: 6
        shadowBlur: 1
    }
}
