cmake_minimum_required(VERSION 3.14)
project(team6-cluster LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/Qt6")
endif()

# find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick QuickControls2)

# 🔹 Todos os módulos que o .pro usava
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Widgets)

add_executable(team6-cluster
    main.cpp
    headers/cluster.h
    qml.qrc
)


target_link_libraries(team6-cluster
    PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickControls2 Qt6::Widgets
)

set_target_properties(team6-cluster PROPERTIES
    QT_QML_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}"
)
