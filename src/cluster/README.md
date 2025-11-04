# 🚀 Instrument Cluster

## 📚 Index
  - [👋 Introduction](#sec-intro)
  - [🧠 Core Ideas](#sec-core-ideas)
  - [🤔 QtWidgets vs QtQuick](#sec-qt-vs-qml)
  - [⚙️ QML & C++ Integration](#sec-integration)
  - [ℹ️ Qt setup](#sec-qt-setup)
  - [🧰 Development Setup](#sec-development-setup)
  - [🎨 UI Design & Components](#sec-ui-components)
  - [🚨 Issues Log](#sec-issues)
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
  - The UI is declarative - you describe what the UI should look like, not how to draw it.
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

<a id="sec-qt-setup"></a>
## ℹ️ Qt setup

Qt provides all the tools needed to build modern graphic applications - including the framework, build tools and IDE integration.  
Before running or developing this project, you'll need to set up your local Qt environment.

**Install Qt**  
The easiest way to install Qt is through the [Qt Online Installer](https://www.qt.io/download-dev).  
 - **Steps:**
    -  Download the Qt Online Installer for your OS (Windows, macOS or Linux).
    - Log in with a Qt account.
    - During installation:
      - Select the version of Qt you want to work on.
      - Select the `Build Tools` option to install the other necessary requirements.
      - **[Optional]** You can also select the `Qt Creator` option to download the Qt IDE, but more on that later.

**Install Qt Creator (IDE)**  
Although Qt projects can be built entirely from command line, Qt Creator simplifies everything:  
  - Integrated code editor for C++, QML and UI forms.
  - Built-in CMake support.
  - Integrated debugger, profiler and QML inspector.
  - One-click build and run for multiple kits.  

After installation (follow the Optional step on the `Install Qt` section):
  - Open Qt Creator.
  - Go to Tools → Options → Kits to verify the installed Qt versions and compilers.
  - Ensure you have a working Desktop Qt 6.x GCC/Clang/MSVC kit.

**Verify Installation**  
To confirm everything is configured correctly, you should check for your qmake, cmake and compiler:
```bash
$ qmake --version
$ cmake --version
$ g++ --version
```
If you encounter any erros such as `Command 'qmake' not found`, `Command 'cmake' not found` or `Command 'g++' not found`, refer to the [🚨 Issues Log](#sec-issues) for troubleshooting, specifically:

  - [Issue #1 - QMake not found after installation](#issue-1)
  - [Issue #2 - CMake not found after installation](#issue-2)
  - [Issue #3 - g++ not found after installation](#issue-3)

**Create a new QtQuick Project (Optional Test)**  
Let's try creating a simple example to verify the setup:
```bash
$ mkdir test && cd test
```

Create 4 files named: `main.qml`, `main.cpp`, `CMakeLists.txt` and `qml.qrc`.
```bash
$ touch main.qml main.cpp CMakeLists.txt qml.qrc
```

Now insert the following code blocks on the files respectively:
```qml
import QtQuick
import QtQuick.Controls

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: "Qt Quick Test"

    Text {
        anchors.centerIn: parent
        text: "Hello QtQuick!"
        font.pixelSize: 32
    }
}
```

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}

```

```cmake
cmake_minimum_required(VERSION 3.5)
project(QtQuickTest LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 11)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

add_executable(QtQuickTest main.cpp qml.qrc)
target_link_libraries(QtQuickTest PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick)

```

```html
<RCC>
  <qresource prefix="/">
    <file>main.qml</file>
  </qresource>
</RCC>>
```

After all this, build and run the project:
```bash
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
$ ./QtQuickTest
```

If a window with "Hello QtQuick!" appears this means the setup is complete.

If you encounter any erros such as `Could NOT find OpenGL & Could NOT find WrapOpenGL`, `CMake Error at CMakeLists.txt:9 (find_package)`, refer to the [🚨 Issues Log](#sec-issues) for troubleshooting, specifically:

  - [Issue #4 - Qt6Gui or WrapOpenGL not found when running cmake ..](#issue-4)
  - [Issue #5 - Could not find package configuration file provided by "Qt6"](#issue-5)

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

<a id="sec-ui-components"></a>
## 🎨 UI Design & Components

?

---

<a id="sec-issues"></a>
## 🚨 Issues Log

This section tracks common issues encountered during development, testing or deployment of the instrument cluster.  
Each issue includes its symptoms, probable cause and recommended fix or workaround.

<a id="issue-1"></a>

### Issue #1 - QMake not found after installation
**- Error Example:**
```bash
$ qmake --version

Command 'qmake' not found, but can be installed with:
sudo apt install qtchooser
```
**- Cause:**  
The terminal doesn't know where the Qt Tools and compiler are located.  
When you install Qt using the Qt Online Installer / Qt Creator, it installs Qt inside its own directory, but doesn't automatically add it to the system PATH

**- Solution:**  
You can check if `qmake` exists in this path:  

```bash
$ ls ~/Qt/*/gcc_64/bin/qmake
```    
The Qt is installed correctly if it returns something like:  
```bash
/home/[user]/Qt/x.x.x/gcc_64/bin/qmake
```

You can now tell the terminal where to find it and temporarily add it to the PATH. 
```bash
$ export PATH=$PATH:~/Qt/x.x.x/gcc_64/bin
```  
After all those steps if you test again you should see something like:
```bash
QMake version 3.1
Using Qt version x.x.x in /home/[user]/Qt/x.x.x/gcc_64/lib
```  
To avoid retyping the PATH every time, edit the `.bashrc` or `.bash_profile`:
```bash
$ nano ~/.bashrc

# Add this line at the end of the file:
export PATH=$PATH:~/Qt/x.x.x/gcc_64/bin
```

Then apply the change:
```bash
$ source ~/.bashrc
```  

<a id="issue-2"></a>

### Issue #2 - CMake not found after installation
**- Error Example:**
```bash
$ cmake --version

Command 'cmake' not found, but can be installed with:
sudo apt install cmake
```
**- Cause:**

CMake is not installed or not added to your system PATH.  
Even if Qt Creator is installed, its bundled CMAKE is not available globally in your terminal.

**- Solution:**

Install CMake manually using the package manager:  

```bash
$ sudo apt update
$ sudo apt install cmake
```  

Then verify installation:

```bash
$ cmake --version

cmake version x.x.x
```  

If you still get an error, restart the terminal or ensure `/usr/bin` is in your PATH:
```bash
$ echo $PATH
``` 

<a id="issue-3"></a>

### Issue #3 - g++ not found after installation
**- Error Example:**
```bash
$ g++ --version

Command 'g++' not found, but can be installed with:
sudo apt install g++
```
**- Cause:**

You don't have the compiler and other developers tools installed.

**- Solution:**

You can install it with the `build-essential` package that includes gcc, g++, make and other developer tools  

```bash
$ sudo apt update
$ sudo apt install build-essential
```  

After installation check again:

```bash
$ g++ --version

g++ (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
```  

<a id="issue-4"></a>

### Issue #4 - Qt6Gui or WrapOpenGL not found when running cmake ..
**- Error Example:**
```bash
-- Could NOT find OpenGL (missing: OPENGL_opengl_LIBRARY OPENGL_glx_LIBRARY OPENGL_INCLUDE_DIR)
-- Could NOT find WrapOpenGL (missing: WrapOpenGL_FOUND)
CMake Warning at /home/[user]/Qt/x.x.x/gcc_64/lib/cmake/Qt6/Qt6Config.cmake:235 (find_package):
  Found package configuration file:
    /home/[user]/Qt/x.x.x/gcc_64/lib/cmake/Qt6Gui/Qt6GuiConfig.cmake
  but it set Qt6Gui_FOUND to FALSE so package "Qt6Gui" is considered to be
  NOT FOUND.  Reason given by package:
  Qt6Gui could not be found because dependency WrapOpenGL could not be found.
```
**- Cause:** 

Your system is missing the OpenGL development libraries that Qt needs to render graphics.  
QtQuick and QtGui rely on OpenGL or compatible rendering libraries to build the GPU-accelerated UI.  
This happens frequently on new Linux installations because OpenGL dev packages are not installed by default.

**- Solution:**

Install the required OpenGL and Mesa development packages:

```bash
$ sudo apt update
$ sudo apt install build-essential libgl1-mesa-dev libglu1-mesa-dev libgles2-mesa-dev
```
  - `build-essential`: Compilers and base dev tools.
  - `libgl1-mesa-dev`: Software implementation of OpenGL.
  - `libglu1-mesa-dev`: OpenGL utility library.
  - `libgles2-mesa-dev`: OpenGL ES support.

Clean and rebuild the project:
```bash
$ rm -rf build
$ mkdir build && cd build
$ cmake ..

-- Found OpenGL: /usr/lib/x86_64-linux-gnu/libOpenGL.so
-- Found Qt6Gui
-- Configuring done
-- Generating done
```

<a id="issue-5"></a>

### Issue #5 - Could not find package configuration file provided by "Qt6"
**- Error Example:**
```bash
CMake Error at CMakeLists.txt:9 (find_package):
  By not providing "FindQt6.cmake" in CMAKE_MODULE_PATH this project has
  asked CMake to find a package configuration file provided by "Qt6", but
  CMake did not find one.

  Could not find a package configuration file provided by "Qt6" with any of
  the following names:

    Qt6Config.cmake
    qt6-config.cmake

  Add the installation prefix of "Qt6" to CMAKE_PREFIX_PATH or set "Qt6_DIR"
  to a directory containing one of the above files.
```

**- Cause:** 

CMake can't locate the Qt installation.
This usually happens when:
  - Qt isn't added to the system's `CMAKE_PREFIX_PATH`, so CMake doesn't know where to look for it.
  - You installed Qt using the Qt Online Installed, but didn't set up the environment variables.
  - The Qt Kit is installed, but you are running `cmake` manually outside of Qt Creator, which normally sets up paths automatically.

**- Solution:**

There are two main ways to fix this, depending on whether we are using Qt Creator or the terminal.

 - **Option 1 - Use Qt Creator**
  Qt Creator automatically configures `CMAKE_PREFIX_PATH` for you.
    - Open Qt Creator.
    - Go to File → Open File or Project → `CMakeLists.txt`.
    - When prompted, select a Qt x.x.x Kit.
    - Click Configure Project and then Run CMake

 - **Option 2 - Configure Manually in Terminal**
  If you installed via the Qt Installed the path for the cmake it's usally here: `~/Qt/x.x.x/gcc_64/lib/cmake`
  Run CMake with the correct prefix:
    ```bash
    $ mkdir build && cd build
    $ cmake .. -DCMAKE_PREFIX_PATH=$HOME/Qt/x.x.x/gcc_64/lib/cmake
    
    -- Found Qt6: /home/[user]/Qt/x.x.x/gcc_64/lib/cmake/Qt6 (found version "x.x.x")
    -- Configuring done
    -- Generating done
    ```  

    Alternatively, set the Qt6_DIR environment variable explicitly:
    ```bash
    $ export Qt6_DIR=$HOME/Qt/x.x.x/gcc_64/lib/cmake/Qt6
    ```  

<!-- <a id="issue-x"></a>

### Issue #x - 
**- Error Example:**

**- Cause:** 

**- Solution:** -->

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
https://www.qt.io   
