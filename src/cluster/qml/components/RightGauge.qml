import QtQuick
import QtQuick.Controls
import QtQuick.Shapes
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0

Item {
    id: root
    width: 300
    height: 300

    property int currPage: 0
    property int maxPages: 3

    //? OUTER RING
    Rectangle {
        id: outerCircle
        anchors.centerIn: parent
        width: 300
        height: 300
        radius: width / 2
        color: BaseTheme.sportBlack
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: BaseTheme.white
            shadowBlur: 1.0
            shadowOpacity: 0.6
            shadowHorizontalOffset: 0
            shadowVerticalOffset: 0
            shadowScale: 1.01
        }
    }

    Repeater {
        id: pageIndications
        model: root.totalPages

        Rectangle {
            id: indicatorDot
            width: 8
            height: 8
            radius: 4

            property real angle: -45 + (index * 15)
            property real distance: 145

            x: parent.width / 2
        }
    }

    //? GAUGE CONTENT AREA
    Rectangle {
        id: innerCircle
        anchors.centerIn: parent
        width: 275
        height: 275
        radius: width / 2
        color: BaseTheme.darkBlack
        border.color: BaseTheme.blackboard
        border.width: 2
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: BaseTheme.white
            shadowBlur: 1.0
            shadowOpacity: 0.9
            shadowHorizontalOffset: 0
            shadowVerticalOffset: 0
            shadowScale: 0.98
        }
    }
}