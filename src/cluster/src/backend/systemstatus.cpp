#include "systemstatus.hpp"

SystemStatus::SystemStatus(QObject *parent): QObject(parent) {
    updateBatteryLevel();
    connect(&_timer, &QTimer::timeout, this, &SystemStatus::updateBatteryLevel);
    _timer.start(30000); // 30s
}

int SystemStatus::readBatteryLevelFromSystem() const {
    QFile file("/sys/class/power_supply/BAT0/capacity");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;
    QTextStream in(&file);
    int level;
    in >> level;
    return level;
}

void SystemStatus::updateBatteryLevel() {
    int newLevel = readBatteryLevelFromSystem();
    if (newLevel != _currBatteryLvl) {
        _currBatteryLvl = newLevel;
        emit batteryChanged();
    }
}

QString SystemStatus::batteryIconSource() const {
    if (_currBatteryLvl <= 10)          return "qrc:/assets/icons/battery-empty-solid-full.svg";          // 0–10%
    else if (_currBatteryLvl <= 25)     return "qrc:/assets/icons/battery-quarter-solid-full.svg";        // 11–25%
    else if (_currBatteryLvl <= 50)     return "qrc:/assets/icons/battery-half-solid-full.svg";           // 26–50%
    else if (_currBatteryLvl <= 75)     return "qrc:/assets/icons/battery-three-quarters-solid-full.svg"; // 51–75%
    return "qrc:/assets/icons/battery-full-solid-full.svg";         // 76–100%
}


int SystemStatus::currBatteryLvl() const { return _currBatteryLvl; }
