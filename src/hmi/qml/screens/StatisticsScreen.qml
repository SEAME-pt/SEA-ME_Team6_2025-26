import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import ClusterTheme 1.0

Item {

    GridLayout {
        anchors.fill: parent
        anchors.leftMargin: 32
        anchors.rightMargin: 32
        anchors.bottomMargin: 32
        columns: 3
        rows: 2
        columnSpacing: 16
        rowSpacing: 16

        // Card 1 - Top Left (1x1)
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: BaseTheme.white
            radius: 8

            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#20000000"
                shadowBlur: 0.3
                shadowHorizontalOffset: 0
                shadowVerticalOffset: 1
            }

            // Your content here
        }

        // Card 2 - Middle (1x2 tall)
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.rowSpan: 2
            color: BaseTheme.white
            radius: 8

            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#20000000"
                shadowBlur: 0.3
                shadowHorizontalOffset: 0
                shadowVerticalOffset: 1
            }

            // Your content here
        }

        // Card 3 - Right (1x2 tall)
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.rowSpan: 2
            color: BaseTheme.white
            radius: 8

            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#20000000"
                shadowBlur: 0.3
                shadowHorizontalOffset: 0
                shadowVerticalOffset: 1
            }

            // Your content here
        }

        // Card 4 - Bottom Left (1x1)
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: BaseTheme.white
            radius: 8

            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowColor: "#20000000"
                shadowBlur: 0.3
                shadowHorizontalOffset: 0
                shadowVerticalOffset: 1
            }

            // Your content here
        }
    }
}