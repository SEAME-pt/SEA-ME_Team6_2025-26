import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ClusterTheme 1.0
import "../components"

Item {
    id: baseScreen
    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#D9E9FB" }
            GradientStop { position: 0.3; color: "#DCE6F8" }
            GradientStop { position: 0.5; color: "#E5EDF7" }
            GradientStop { position: 0.7; color: "#EDF2F8" }
            GradientStop { position: 1.0; color: BaseTheme.white }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Topbar {
            id: topbar
        }

        Loader {
            id: pageLoader
            Layout.fillWidth: true
            Layout.fillHeight: true
            source: "qrc:/qml/screens/HomeScreen.qml"
        }

        Navbar {
            id: navbar
            onChangeScreen: function (page) {
                pageLoader.source = page;
            }
        }
    }
}