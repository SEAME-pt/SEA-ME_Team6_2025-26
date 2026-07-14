import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ClusterTheme 1.0

Item {
    id: root
    anchors.fill: parent

    // ── OTA State ────────────────────────────────────────────────────────────
    QtObject {
        id: otaState
        property bool   updateAvailable: true
        property string updateVersion:   ota.pendingVersion
        property string updateSize:      "312 MB"
        property bool   updateQueued:    false
    }

    // ── ADAS State (ListModel holds static metadata only — ListElement can't
    //    contain bindings, so `enabled` is looked up live via adasEnabledFor()) ──
    ListModel {
        id: adasModel
        ListElement {
            label: "Lane Keep Assist"
            sub: "Corrects steering drift toward lane edges"
            statusProp: "LKAStatus"
            vssPath: "Vehicle.ADAS.LaneKeepAssist.IsEnabled"
        }
        ListElement {
            label: "Adaptive Cruise Control"
            sub: "Adjusts speed to maintain safe following distance"
            statusProp: "CCStatus"
            vssPath: "Vehicle.ADAS.CruiseControl.IsEnabled"
        }
        ListElement {
            label: "Autonomous Emergency Braking"
            sub: "Applies full brakes when collision is imminent"
            statusProp: "AEBStatus"
            vssPath: "Vehicle.ADAS.AEB.IsEnabled"
        }
        ListElement {
            label: "Traffic Sign Recognition"
            sub: "Reads and displays road signs in real time"
            statusProp: "TSRStatus"
            vssPath: "Vehicle.ADAS.ObjectDetection.IsEnabled"
        }
    }

    function adasEnabledFor(statusProp) {
        switch (statusProp) {
            case "LKAStatus": return adas.LKAStatus
            case "CCStatus":  return adas.CCStatus
            case "AEBStatus": return adas.AEBStatus
            case "TSRStatus": return adas.TSRStatus
        }
        return false
    }

    function setAllAdas(value) {
        for (var i = 0; i < adasModel.count; i++) {
            adas.triggerUpdateADASFeature(adasModel.get(i).vssPath, value)
        }
    }

    function enabledAdasCount() {
        var count = 0
        for (var i = 0; i < adasModel.count; i++) {
            if (root.adasEnabledFor(adasModel.get(i).statusProp))
                count++
        }
        return count
    }

    function anyAdasActive() {
        return root.enabledAdasCount() > 0
    }

    // Manual escape hatch: lets the user unlock the toggles from "Assisted"
    // even while all 4 features happen to be on, without switching any of
    // them off. Cleared again as soon as autonomous is (re)engaged.
    property bool assistedOverride: false

    // Derived from the actual feature states — "autonomous" only when every
    // ADAS feature is currently enabled AND the user hasn't manually asked
    // to stay in assisted mode.
    readonly property string adasMode:
        (!root.assistedOverride && root.enabledAdasCount() === adasModel.count) ? "autonomous" : "assisted"

    // Quick actions behind the mode switch.
    function goAutonomous() {
        root.assistedOverride = false
        root.setAllAdas(true)
    }
    function goAssisted() {
        // Just unlock — leave every feature's current state untouched so the
        // user can deactivate whichever ones they want by hand.
        root.assistedOverride = true
    }

    // ── Root layout ──────────────────────────────────────────────────────────
    RowLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 24

        // ── LEFT PANEL: OTA Updates ──────────────────────────────────────────
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * 0.38
            color: BaseTheme.sportBlack
            radius: 4
            border.color: BaseTheme.gaugeTicksInactive
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 0

                // Badge
                Rectangle {
                    width: otaBadgeText.implicitWidth + 16
                    height: 18
                    radius: 3
                    color: "transparent"
                    border.color: BaseTheme.gaugeBattery
                    border.width: 1
                    Text {
                        id: otaBadgeText
                        anchors.centerIn: parent
                        text: "OTA UPDATES"
                        color: BaseTheme.gaugeBattery
                        font.pixelSize: 10
                        font.bold: true
                        font.letterSpacing: 1.4
                    }
                }

                Item { Layout.preferredHeight: 20 }

                // Installed version row
                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        text: "Installed"
                        color: BaseTheme.white; opacity: 0.45
                        font.pixelSize: 11; font.bold: true; font.letterSpacing: 0.8
                    }
                    Item { Layout.fillWidth: true }
                    Text {
                        text: ota.installedVersion
                        color: BaseTheme.white
                        font.pixelSize: 11; font.bold: true
                    }
                }

                Item { Layout.preferredHeight: 8 }
                Rectangle { Layout.fillWidth: true; height: 1; color: BaseTheme.gaugeTicksInactive; opacity: 0.35 }
                Item { Layout.preferredHeight: 16 }

                // Available update card
                Rectangle {
                    Layout.fillWidth: true
                    height: updateCardCol.implicitHeight + 24
                    radius: 3
                    color: otaState.updateAvailable ? Qt.rgba(1, 0.72, 0.18, 0.07) : Qt.rgba(1,1,1,0.04)
                    border.color: otaState.updateAvailable ? Qt.rgba(1, 0.72, 0.18, 0.25) : BaseTheme.gaugeTicksInactive
                    border.width: 1

                    ColumnLayout {
                        id: updateCardCol
                        anchors { left: parent.left; right: parent.right; top: parent.top; margins: 14 }
                        spacing: 6

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            Text {
                                text: otaState.updateAvailable ? otaState.updateVersion : "Up to date"
                                color: otaState.updateAvailable ? BaseTheme.gaugeBattery : BaseTheme.white
                                font.pixelSize: 18; font.bold: true
                            }
                            Item { Layout.fillWidth: true }

                            Rectangle {
                                width: pillLabel.implicitWidth + 12; height: 16; radius: 8
                                color: otaState.updateAvailable ? Qt.rgba(1,0.72,0.18,0.15) : Qt.rgba(0.2,0.9,0.4,0.12)
                                border.color: otaState.updateAvailable ? BaseTheme.gaugeBattery : BaseTheme.gaugeTicksActive
                                border.width: 1
                                Text {
                                    id: pillLabel
                                    anchors.centerIn: parent
                                    text: otaState.updateAvailable ? "AVAILABLE" : "CURRENT"
                                    color: otaState.updateAvailable ? BaseTheme.gaugeBattery : BaseTheme.gaugeTicksActive
                                    font.pixelSize: 8; font.bold: true; font.letterSpacing: 1.2
                                }
                            }
                        }

                        Text {
                            visible: otaState.updateAvailable
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                            text: "A new software version is ready to install."
                            color: BaseTheme.white; opacity: 0.65
                            font.pixelSize: 12
                        }

                        Item { Layout.preferredHeight: 2; visible: otaState.updateAvailable }

                        RowLayout {
                            visible: otaState.updateAvailable
                            spacing: 6
                            Text {
                                text: otaState.updateSize
                                color: BaseTheme.white; opacity: 0.4; font.pixelSize: 10; font.bold: true; font.letterSpacing: 0.4
                            }
                            Rectangle { width: 3; height: 3; radius: 1.5; color: BaseTheme.white; opacity: 0.3 }
                            Text {
                                text: "Released 2 days ago"
                                color: BaseTheme.white; opacity: 0.4; font.pixelSize: 10; font.bold: true; font.letterSpacing: 0.4
                            }
                        }
                    }
                }

                Item { Layout.preferredHeight: 16 }

                // Queued notice
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    visible: otaState.updateQueued

                    Rectangle { width: 4; height: 4; radius: 2; color: BaseTheme.gaugeBattery; opacity: 0.7 }
                    Text {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: "Update queued — it will install the next time the car reboots."
                        color: BaseTheme.gaugeBattery
                        font.pixelSize: 11; font.bold: true
                    }
                }

                Item { Layout.fillHeight: true }

                // Install button
                Rectangle {
                    Layout.fillWidth: true; height: 40; radius: 3
                    opacity: (!otaState.updateAvailable || otaState.updateQueued) ? 0.3 : 1.0
                    color: "transparent"
                    border.color: otaState.updateAvailable ? BaseTheme.gaugeBattery : BaseTheme.gaugeTicksInactive
                    border.width: 1
                    Behavior on opacity { NumberAnimation { duration: 200 } }

                    Text {
                        anchors.centerIn: parent
                        text: otaState.updateQueued ? "Queued for Next Reboot"
                            : (otaState.updateAvailable ? "Install Update" : "No Update Available")
                        color: otaState.updateAvailable ? BaseTheme.gaugeBattery : BaseTheme.white
                        font.pixelSize: 12; font.bold: true; font.letterSpacing: 1.0
                        opacity: otaState.updateAvailable ? 1.0 : 0.45
                    }

                    MouseArea {
                        anchors.fill: parent
                        enabled: otaState.updateAvailable && !otaState.updateQueued
                        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                        onClicked: {
                            otaState.updateQueued = true
                            ota.triggerUpdate()
                        }
                    }
                }
            }
        }

        // ── RIGHT PANEL: ADAS Features ───────────────────────────────────────
        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: BaseTheme.sportBlack
            radius: 4
            border.color: BaseTheme.gaugeTicksInactive
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 0

                // Badge
                Rectangle {
                    width: adasBadgeText.implicitWidth + 16; height: 18; radius: 3
                    color: "transparent"
                    border.color: BaseTheme.danger; border.width: 1
                    Text {
                        id: adasBadgeText
                        anchors.centerIn: parent
                        text: "ADAS FEATURES"
                        color: BaseTheme.danger
                        font.pixelSize: 10; font.bold: true; font.letterSpacing: 1.4
                    }
                }

                Item { Layout.preferredHeight: 8 }
                Rectangle { Layout.fillWidth: true; height: 1; color: BaseTheme.gaugeTicksInactive; opacity: 0.35 }
                Item { Layout.preferredHeight: 8 }

                //  ADAS toggle rows
                Repeater {
                    model: adasModel

                    delegate: ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 0

                        RowLayout {
                            Layout.fillWidth: true
                            Layout.topMargin: 10
                            Layout.bottomMargin: 10
                            spacing: 0

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 3
                                Text {
                                    text: model.label
                                    color: BaseTheme.white
                                    font.pixelSize: 13; font.bold: true
                                }
                                Text {
                                    text: model.sub
                                    color: BaseTheme.white; opacity: 0.4; font.pixelSize: 10
                                }
                            }

                            Item { Layout.preferredWidth: 16 }

                            // Toggle — enabled state is read live from ADASProvider via
                            // root.adasEnabledFor(); clicking only sends the write request,
                            // the switch itself updates once the provider confirms the change.
                            Rectangle {
                                id: track
                                width: 40; height: 22; radius: 11

                                property bool isEnabled: root.adasEnabledFor(model.statusProp)
                                property bool locked: root.adasMode === "autonomous"

                                opacity: locked ? 0.5 : 1.0
                                color:        isEnabled ? Qt.rgba(0.2,0.9,0.4,0.18) : Qt.rgba(1,1,1,0.08)
                                border.color: isEnabled ? BaseTheme.gaugeTicksActive : BaseTheme.gaugeTicksInactive
                                border.width: 1
                                Behavior on opacity       { NumberAnimation { duration: 200 } }
                                Behavior on color        { ColorAnimation { duration: 200 } }
                                Behavior on border.color { ColorAnimation { duration: 200 } }

                                Rectangle {
                                    width: 16; height: 16; radius: 8
                                    anchors.verticalCenter: parent.verticalCenter
                                    x: track.isEnabled ? parent.width - width - 3 : 3
                                    color: track.isEnabled ? BaseTheme.gaugeTicksActive : BaseTheme.gaugeTicksInactive
                                    Behavior on x     { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad } }
                                    Behavior on color { ColorAnimation  { duration: 200 } }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    enabled: !track.locked
                                    cursorShape: track.locked ? Qt.ArrowCursor : Qt.PointingHandCursor
                                    onClicked: {
                                        var newValue = !track.isEnabled
                                        console.log("[ADAS] Toggling", model.vssPath, "->", newValue)
                                        adas.triggerUpdateADASFeature(model.vssPath, newValue)
                                    }
                                }
                            }
                        }

                        Rectangle { Layout.fillWidth: true; height: 1; color: BaseTheme.gaugeTicksInactive; opacity: 0.2 }
                    }
                }

                Item { Layout.fillHeight: true }

                // Manual-mode warning — shown in Assisted mode when every ADAS
                // feature is currently switched off.
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 30
                    Layout.bottomMargin: 10
                    visible: root.adasMode === "assisted" && !root.anyAdasActive()
                    radius: 3
                    color: Qt.rgba(1, 0.72, 0.18, 0.08)
                    border.color: Qt.rgba(1, 0.72, 0.18, 0.3)
                    border.width: 1

                    Text {
                        anchors.centerIn: parent
                        text: "MANUAL MODE"
                        color: BaseTheme.gaugeBattery
                        font.pixelSize: 10; font.bold: true; font.letterSpacing: 0.6
                    }
                }

                // Autonomous / Assisted mode switch
                Rectangle {
                    Layout.fillWidth: true
                    height: 38
                    radius: 4
                    color: Qt.rgba(1,1,1,0.05)
                    border.color: BaseTheme.gaugeTicksInactive
                    border.width: 1

                    Item {
                        anchors.fill: parent
                        anchors.margins: 3

                        // Sliding highlight behind the active option
                        Rectangle {
                            id: modeHighlight
                            width: parent.width / 2
                            height: parent.height
                            radius: 3
                            x: root.adasMode === "autonomous" ? 0 : parent.width / 2
                            color: root.adasMode === "autonomous"
                                   ? Qt.rgba(1, 0.72, 0.18, 0.18)
                                   : Qt.rgba(0.2, 0.9, 0.4, 0.18)
                            border.color: root.adasMode === "autonomous"
                                          ? BaseTheme.gaugeBattery
                                          : BaseTheme.gaugeTicksActive
                            border.width: 1
                            Behavior on x     { NumberAnimation { duration: 200; easing.type: Easing.InOutQuad } }
                            Behavior on color { ColorAnimation { duration: 200 } }
                            Behavior on border.color { ColorAnimation { duration: 200 } }
                        }

                        RowLayout {
                            anchors.fill: parent
                            spacing: 0

                            Rectangle {
                                Layout.fillWidth: true; Layout.fillHeight: true
                                color: "transparent"
                                Text {
                                    anchors.centerIn: parent
                                    text: "Autonomous"
                                    color: root.adasMode === "autonomous" ? BaseTheme.gaugeBattery : BaseTheme.white
                                    opacity: root.adasMode === "autonomous" ? 1.0 : 0.5
                                    font.pixelSize: 11; font.bold: true; font.letterSpacing: 0.6
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: root.goAutonomous()
                                }
                            }

                            Rectangle {
                                Layout.fillWidth: true; Layout.fillHeight: true
                                color: "transparent"
                                Text {
                                    anchors.centerIn: parent
                                    text: "Assisted"
                                    color: root.adasMode === "assisted" ? BaseTheme.gaugeTicksActive : BaseTheme.white
                                    opacity: root.adasMode === "assisted" ? 1.0 : 0.5
                                    font.pixelSize: 11; font.bold: true; font.letterSpacing: 0.6
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: root.goAssisted()
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}