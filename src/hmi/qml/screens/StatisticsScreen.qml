import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ClusterTheme 1.0

Item {
    id: root
    anchors.fill: parent

    // ── Reusable card header badge ──────────────────────────────────────────
    component SectionBadge: Rectangle {
        property alias text: badgeText.text
        property color accent: BaseTheme.gaugeTicksActive
        width: badgeText.implicitWidth + 16
        height: 18
        radius: 3
        color: "transparent"
        border.color: accent
        border.width: 1
        Text {
            id: badgeText
            anchors.centerIn: parent
            color: parent.accent
            font.pixelSize: 10
            font.bold: true
            font.letterSpacing: 1.4
        }
    }

    // ── Reusable signal row: label left, value right, divider below ────────
    component SignalRow: ColumnLayout {
        id: signalRow
        property string label: ""
        property string value: ""
        property string unit: ""
        property color valueColor: BaseTheme.white
        Layout.fillWidth: true
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: 6
            Layout.bottomMargin: 6
            spacing: 0

            Text {
                text: signalRow.label
                color: BaseTheme.white
                opacity: 0.45
                font.pixelSize: 10
                font.bold: true
                font.letterSpacing: 0.4
            }
            Item { Layout.fillWidth: true }
            RowLayout {
                spacing: 4

                Text {
                    text: signalRow.value
                    color: signalRow.valueColor
                    font.pixelSize: 12
                    font.bold: true
                }
                Text {
                    text: signalRow.unit
                    visible: signalRow.unit.length > 0
                    color: signalRow.valueColor
                    opacity: 0.55
                    font.pixelSize: 10
                    font.bold: true
                }
            }
        }
        Rectangle { Layout.fillWidth: true; height: 1; color: BaseTheme.gaugeTicksInactive; opacity: 0.2 }
    }

    // ── Reusable stat card shell ─────────────────────────────────────────────
    component StatCard: Rectangle {
        id: statCard
        default property alias content: cardCol.data
        property alias badgeText: badge.text
        property color accent: BaseTheme.gaugeTicksActive

        Layout.fillWidth: true
        Layout.fillHeight: true
        clip: true
        color: BaseTheme.sportBlack
        radius: 4
        border.color: BaseTheme.gaugeTicksInactive
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 14
            spacing: 0

            SectionBadge {
                id: badge
                accent: statCard.accent
            }

            Item { Layout.preferredHeight: 6 }
            Rectangle { Layout.fillWidth: true; height: 1; color: BaseTheme.gaugeTicksInactive; opacity: 0.35 }
            Item { Layout.preferredHeight: 3 }

            ColumnLayout {
                id: cardCol
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 0
            }
        }
    }

    // ── Root layout: 3x2 dashboard grid ─────────────────────────────────────
    GridLayout {
        anchors.fill: parent
        anchors.margins: 24
        columns: 3
        rows: 2
        columnSpacing: 20
        rowSpacing: 20

        // ── Vehicle & Trip ───────────────────────────────────────────────────
        StatCard {
            badgeText: "VEHICLE & TRIP"
            accent: BaseTheme.gaugeTicksActive

            SignalRow { label: "Speed";                value: vehicle.speed;     unit: "m/h" }
            SignalRow { label: "Average Speed";        value: vehicle.averageSpeed;     unit: "km/h" }
            SignalRow { label: "Max Speed (Trip)";      value: vehicle.maxSpeedTrip;    unit: "km/h" }
            SignalRow { label: "Traveled Distance";     value: vehicle.traveledDistance; unit: "km" }
            SignalRow { label: "Distance (Trip)";       value: vehicle.traveledDistanceTrip;  unit: "km" }
        }

        // ── Powertrain & Battery ─────────────────────────────────────────────
        StatCard {
            badgeText: "POWERTRAIN & BATTERY"
            accent: BaseTheme.gaugeBattery

            SignalRow { label: "Battery Voltage";       value: powertrain.batteryVoltage; unit: "V" }
            SignalRow { label: "Motor Speed";           value: powertrain.motorSpeed; unit: "rpm" }
            SignalRow { label: "Energy / km (Trip)";    value: vehicle.energyPerKmTrip;  unit: "kWh/km" }
            SignalRow { label: "Energy Used (Trip)";    value: vehicle.totalEnergyTrip;  unit: "kWh" }
            SignalRow {
                label: "Battery Level"
                value: powertrain.isBatteryLow
                valueColor: BaseTheme.gaugeBattery
            }
            SignalRow {
                label: "Battery Critical"
                value: powertrain.isBatteryCritical
                valueColor: BaseTheme.gaugeTicksActive
            }
        }

        // ── Exterior & Location ──────────────────────────────────────────────
        StatCard {
            badgeText: "EXTERIOR & LOCATION"
            accent: BaseTheme.white

            SignalRow { label: "Air Temperature";      value: exterior.airTemperature;    unit: "°C" }
            SignalRow { label: "Heading";               value: currentLocation.heading;  unit: "NW" }
            SignalRow { label: "Steering Angle";        value: chassis.steeringWheelAngle }
        }

        // ── ADAS Sensors ──────────────────────────────────────────────────────
        StatCard {
            badgeText: "ADAS SENSORS"
            accent: BaseTheme.danger

            SignalRow { label: "Front Obstacle Distance"; value: adas.frontDistance ; unit: "cm" }
            SignalRow { label: "Lane Lateral Deviation";  value: adas.lateralDeviation; unit: "cm" }
            SignalRow {
                label: "Lane Status"
                value: adas.laneStatus
                valueColor: BaseTheme.gaugeTicksActive
            }
            SignalRow { label: "Speed Limit (Detected)";  value: adas.speedLimit; unit: "km/h" }
            SignalRow {
                label: "Traffic Light"
                value: adas.trafficLight
                valueColor: BaseTheme.gaugeTicksActive
            }
        }

        // ── OTA Status ────────────────────────────────────────────────────────
        StatCard {
            badgeText: "OTA STATUS"
            accent: BaseTheme.gaugeBattery

            SignalRow { label: "Installed Version";     value: ota.installedVersion }
            SignalRow {
                label: "Pending Version"
                value: ota.pendingVersion
                valueColor: BaseTheme.gaugeBattery
            }
            SignalRow {
                label: "Update Available"
                value: ota.isUpdateAvailable
                valueColor: BaseTheme.gaugeBattery
            }
        }

        // ── Extras / Misc ────────────────────────────────────────────────────
        StatCard {
            badgeText: "EXTRAS"
            accent: BaseTheme.gaugeTicksActive

            SignalRow { label: "LKA Status";       value: adas.LKAStatus }
            SignalRow { label: "CC Status";          value: adas.CCStatus }
            SignalRow { label: "AEB Status";          value: adas.AEBStatus }
            SignalRow { label: "TSR Status";          value: adas.TSRStatus }
            SignalRow { label: "Trip Duration";         value: vehicle.durationTrip; unit: "s" }
        }
    }
}