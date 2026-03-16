import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0
import Cluster.Backend 1.0
import "../components"

Item {
    id: root
    property string activeBlinkerSource: ""
    property string inactiveBlinkerSource: ""
    property bool isActive: false
    property real effectShadowOpacity: 0.8

    Image {
        id: blinker
        source: root.isActive ? root.activeBlinkerSource : root.inactiveBlinkerSource
        fillMode: Image.PreserveAspectFit
        smooth: true
        mipmap: true
        sourceSize.width: 32
        sourceSize.height: 32
        opacity: root.isActive ? 1 : 0.2

        layer.enabled: root.isActive
        layer.effect: MultiEffect {
            id: rightBlinkerEffect
            width: 36
            height: 36
            shadowEnabled: true
            shadowColor: BaseTheme.vibrant
            shadowBlur: 0.6
            shadowScale: 1.35
            shadowOpacity: root.effectShadowOpacity
            shadowHorizontalOffset: 0
            shadowVerticalOffset: 0
        }
    }

    SequentialAnimation {
        running: root.isActive
        loops: Animation.Infinite

        NumberAnimation {
            target: blinker
            property: "opacity"
            to: 1
            duration: 0
        }
        PauseAnimation { duration: 500 }
        NumberAnimation {
            target: blinker
            property: "opacity"
            to: 0.3
            duration: 150
        }
        PauseAnimation { duration: 500 }
    }

    SequentialAnimation {
        running: root.isActive
        loops: Animation.Infinite

        NumberAnimation {
            target: root
            property: "effectShadowOpacity"
            to: 0.8
            duration: 0
        }
        PauseAnimation { duration: 500 }
        NumberAnimation {
            target: root
            property: "effectShadowOpacity"
            to: 0
            duration: 150
        }
        PauseAnimation { duration: 500 }
    }
}