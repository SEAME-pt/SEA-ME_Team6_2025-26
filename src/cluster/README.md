# 🚀 Instrument Cluster

## 📚 Index
  - [👋 Introduction](#sec-intro)
  - [🧠 Core Ideas](#sec-core-ideas)
  - [🧰 Development Setup](#sec-development-setup)
  - [⚙️ QML & C++ Integration](#sec-integration)
  - [🎨 UI Design & Components](#sec-ui-components)
  - [🤔 QtWidgets vs QtQuick](#sec-qt-vs-qml)
  - [🔗 Links](#sec-links)

---

<a id="sec-intro"></a>
## 👋 Introduction

An instrument cluster is a collection of dials and gauges that typically sits behind the steering wheel, giving the driver a range of information about the vehicle's status. All vehicles have an instrument cluster or some instrumentation in other locations within the driver's eye line.  
Aside from regular driving activities, the driver must also monitor various aspects of the vehicle - such as current speed, fuel level, or battery capacity in an EV. The instrument cluster enables this, ensuring the driver can manage fuel usage and adhere to speed limits. It also provides additional feedback, such as warning lights for oil pressure, tire pressure, or ADS (Advanced Driver Assistance Systems) alerts.  

**Typical Gauges and Indicators:**
- Odometer
- Speedometer
- Fuel level / Battery capacity
- Rev counter
- Power / Regeneration level
- Drive mode or gear selection
- Turn indicators
- Headlight status (including main beams)
- External temperature
- Engine fault warnings
- Tire pressure
- ADAS alerts
- Engine temperature

Modern vehicle's collect this data through the ECU (Electronic Control Unit) and other real-time subsystems. These values are distributed over vehicle networks and reflected in the cluster display, enabling live updates for metrics like trip distance, fuel economy, or average speed.

This project **simulates a digital instrument cluster** using **Qt and QtQuick**, demonstrating how real-time data visualization and interactive UI elements can be built using modern declarative programming techniques.

---

<a id="sec-core-ideas"></a>
## 🧠 Core Ideas

- **Instrument Cluster:** A digital dashboard displaying critical vehicle information such as speed, fuel level and system alerts in real time.
- **Simulation Goal:** Recreate the behavior and appearance of a real vehicle cluster through software, focusing on responsiveness and visual clarity.
- **Data Flow:** Represent how sensor data (speed, temperature, tire pressure, etc) is gathered and fed into visual components that update dynamically.
- **Real-Time Updates:** Use bindings and signals to reflect changes instantly in the user interface.
- **Qt Framework:** C++ based cross-platform framework providing the foundation for the application, enabling efficient rendering and modular design.
- **QtQuick / QML:** A declarative UI language built on Qt that allows for designing animated, GPU-accelerated interfaces - ideal for dashboards and gauges.
- **C++ Backend Integration:** Logic, data processing and simulated ECU signals are handled in C++, then exposed to QML for real-time display.
- **Component Based UI:** Reusable QML Components for modularity and maintainability.
- **Declarative Programming:** Define what the UI should look like and how it reacts to data changes, instead of managing each change imperatively.
- **Scalability:** Design principles that allow easy adaptation of the cluster layout for various resolutions, form factors, or additional vehicle indicators.
- **Data Binding & Signals:** QML automatically updates UI elements when data from the backend changes, C++ emits signals and QML onPropertyChanged handlers react accordingly.
- **Resource Management:** Qt's resource system `.qrc` to embed QML, icons and fonts for easy deployment and portability.
- **Extensibility:** Designed to integrate with real sensor data via CAN bus without major architectural changes.
- **MVC/MVVM Architecture:** Separate logic from presentation - data models expose observable properties and the QML view layer simply binds to them.
- **CMake:** Describes what the project needs, and then produces the build instructions for the compiler

---

<a id="sec-development-setup"></a>
## 🧰 Development Setup

This project demonstrates how to build a QtQuick based vehicle instrument cluster. The following steps describe how to set up the development environment and run the application

**Requirements:**
- **Qt 6.x** with the following modules:
  - Qt6::Core
  - Qt6::Gui
  - Qt6::Qml
  - Qt6::Quick
  - Qt6::DBus
- CMake 3.5+
- C++11 or later
- Qt Creator (recommended IDE)
- Git (version control)

**Project Structure: (Not definitive nor implemented yet)**

```bash
cluster/
├── CMakeLists.txt      #  Root build script
├── src/                #  C++ Backend source files
│   ├── main.cpp
│   ├── backend/        #  Classes handling logic / simulation
│   │   ├── a.cpp
│   │   └── a.hpp
│   └── utils/
│       └── logger.cpp  #  Helper modules
├── qml/                #  QML UI Code
│   ├── main.qml
│   ├── components/     #  Reusable UI parts
│   │   └── a.qml
│   ├── screens/        #  Multi-view layouts (Cluster, Settings)
│   └── themes/         #  Styling / Color schemes
│       └── base.qml
├── assets/
│   ├── icons/
│   ├── images/
│   └── fonts/
└── qml.qrc             #  Resource file bundling Qml + assets
```

**Setup & Build:**

```bash
git clone git@github.com:SEAME-pt/SEA-ME_Team6_2025-26.git
cd SEA-ME_Team6_2025-26/src/cluster
```

Via terminal:

```bash
mkdir build && cd build

cmake ..
cmake --build .

./HelloQt6Qml
```

Via Qt Creator:

- Open `CMakeLists.txt` in Qt Creator.
- Select the Qt Kit (e.g., Desktop Qt 6.6.0 GCC 64-bit).
- Configure and run the project.

On Raspberry Pi:  
More information on how to build and deploy the cluster on the Raspberry Pi on `../cross-compiler`

---
<a id="sec-integration"></a>
## ⚙️ QML & C++ Integration

QtQuick applications combine QML for UI and C++ for logic, creating a powerful separation between presentation and data processing.  
In this project, C++ handles the application startup, logic and data models, while QML handles rendering and user interaction.

### Why C++ Handles Logic and QML Handles UI?
- **Performance and Efficiency**  
  C++ is a compiled, low-level language - it provides:
  - **High performance** for computations, data processing or simulations.
  - **Tight memory control**, ideal for embedded systems.
  - Access to system APIs and hardware interfaces (CAN bus, sockets, etc).

  So anything involving data acquisition, calculations or system-level communication is best done in C++.
- **UI Responsiveness and Flexibility**
  QML is a declarative language built on top of the QtQuick scene graph (GPU accelerated). It is designed for:
  - **Describing interfaces**, animations and transitions in a high-level, readable way.
  - Automatically updating visuals when data changes - through bindings and property observers.
  - Rapid prototyping - designers and developers can modify the UI without recompiling C++.

  So QML is used for "what the user sees", while C++ is used for "how the data behaves".

### Architecture Overview

- **C++ Backend**
  - Initializes the application (QGuiApplication) and loads the QML engine (QQmlApplicationEngine).
  - Manages system logic such as data simulation, signal handling or communication (e.g. via CAN).
  - Exposes classes, objects or properties to QML through the QT meta-object system (Q_PROPERTY, Q_INVOKABLE, Q_SIGNAL).

- **QML Frontend**
  - Declares the layout, animations and visual elements of the instrument cluster.
  - Binds directly to C++ properties and reacts to emitted signals automatically.
  - Uses declarative syntax for UI updates - no manual refresh logic is required.


---
<a id="sec-ui-components"></a>
## 🎨 UI Design & Components

?

---
<a id="sec-qt-vs-qml"></a>
## 🤔 QtWidgets vs QtQuick

Qt offers two main approaches to building graphical user interfaces: **QtWidgets** and **QtQuick (QML)**.  
Both are part of the Qt Framework, but they are different in architecture, design and intended use cases.

### QtWidgets - Traditional UI Framework

QtWidgets is the classic, object-oriented UI framework that has existed since the earliest versions of Qt. It is based entirely on C++ and uses CPU to render graphical components.

**Characteristics:**
  - Every button, label and slider is a QWidget - a rectangular UI element drawn on the CPU.
  - Interfaces are built imperatively, meaning developers describe how to build and modify the UI step by step.
  - Layouts, signals and slots are managed in C++.
  - Rendering is CPU-bound, relying on QPainter to draw widgets, which limits animation performance.

**When to use:**
  - Classic desktop applications (e.g. editors, file browsers, IDEs).
  - Form based, static UIs that rarely change layout or animation.
  - Environments where hardware acceleration is unavailable or unnecessary.

### QtQuick (QML) - Modern Declarative UI

QtQuick is the newer framework, introduced with Qt 5 and fully optimized in Qt 6. It uses QML (Qt Modeling Language) for UI declaration and the Qt Scene Graph for rendering - which leverages GPU acceleration.

**Characteristics:**
  - he UI is declarative - you describe what the UI should look like, not how to draw it.
  - Interfaces are made of lightweight QML elements, all rendered by the GPU, enabling smooth transitions and effects.
  - Bindings and signals make the UI reactive: when data changes, the visuals update automatically.
  - C++ integration remains powerful, it can extend QML with custom C++ classes, models and performance-critical logic.
  - Designed for touch interfaces, embedded systems and animated dashboards.

**When to use:**
  - Applications requiring dynamic, fluid and animated UIs.
  - Embedded systems ou automotive displays.
  - Prototyping or iterative UI design.
  - Projects needing scalable and resolution-independent layouts.

### Why QtQuick makes sense for an instrument cluster

An instrument cluster is not a static interface, it's a dynamic, real-time display with continuously updating data, animations, transitions and indicators.  
For such requirements, QtQuick outperforms QtWidgets in every relevant dimension.

**Key Reasons:**
- QtQuick Scene Graph uses the GPU to render animations smoothly - ideal for moving gauges, fading alerts, and rotating needles.
- QML makes it simple to define animations, bindings and state transitions directly in the markup.
- When C++ emits new sensor values, QML automatically updates the visual components, no manual refresh logic, timers, or redraws required.
- C++ backend simulates the ECU and provides data, QML focuses on rendering and interactivity - keeping the architecture modular and maintainable.
- QtQuick is part of Qt for MCUs and Qt Automotive Suite, making it industry-standard for digital clusters and HMIs.

---


<a id="sec-links"></a>
## 🔗 Links

https://blackberry.qnx.com/en/ultimate-guides/software-defined-vehicle/instrument-cluster  
https://tree.nathanfriend.com  
https://doc.qt.io/qt-6/qtqml-cppintegration-overview.html  
https://felgo.com/doc/qt5/qtqml-cppintegration-topic/  
https://doc.qt.io/qtforpython-6/faq/whatisqt.html  
https://somcosoftware.com/en/blog/qml-vs-qt-widgets-detailed-comparison  
https://extenly.com/2024/05/06/qt-widgets-vs-qt-quick-deciding-the-best-approach-for-your-project/  