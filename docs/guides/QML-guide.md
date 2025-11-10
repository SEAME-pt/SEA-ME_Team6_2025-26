# 🚀 QML Guide

## 📚 Index
 - [👋 Introduction](#sec-intro)
 - [🧠 Core Ideas](#sec-core-ideas)
 - [🧱 Core Visual Building Blocks](#sec-core-visual-blocks)
 - [📚 QML Elements](#sec-qml-elements)
 - [🔗 Links](#sec-links)

 ---

<a id="sec-intro"></a>
## 👋 Introduction

The QML Guide serves as a companion to the main Instrument Cluster documentation, focusing on the frontend layer - the part responsible for visuals, interaction and dynamic data binding.  
While the main README explains the overall system, this document dives into QtQuick, QML syntax and the UI components that make up the cluster interface.  
QML (Qt Modeling Language) is a declarative language designed for building modern, fluid and responsive user interfaces.  
Instead of managing widget hierarchies and manual layouts (like in QtWidgets), QML lets you describe your UI as a tree of visual elements - each element (or QML type) defines what appears on screen and how it behaves.  
Every visual element you see - from the background `Rectangle`, to the data `Text`, to the main `ApplicationWindow` - is a QML Type, defined in the QtQuick or QtQuick.Controls modules.  
These types can be composed, extended, and dynamically bound to backend data written in C++.  
This guide provides a detailed overview of:
- The core QML elements used throughout the project (`Rectangle`, `Text`, `Image`, `ApplicationWindow`, etc).
- ADD ACCORDINGLY

Use this document as a reference when working on new screens, refactoring components, or connecting new backend data.

---

<a id="sec-core-ideas"></a>
## 🧠 Core Ideas

---

<a id="sec-core-visual-blocks"></a>
## 🧱 Core Visual Building Blocks

##### Item
`Item` is the invisible base class for almost all visual QML elements.  
You can think of it as a container or a positioning box - it doesn't draw anything itself, but defines where and how things appear.

###### Common Properties

- **x, y** - Position relative to the parent.
- **width, height** - Size of the item.
- **visible** - Controls if the item is shown on screen.
- **opacity** - How transparent the item is (1.0 = fully visible, 0.0 = invisible).
- **rotation, scale** - Transformation properties.
- **anchors** - Layout system to align items relative to other items.
- **parent** - Reference to the parent item in the visual tree.

##### Anchors
`Anchors` are QML's built-in layout system for aligning and positioning elements relative to each other.  
They replace manual `x` and `y` coordinates with logical relationships, making your UI much more flexible.

###### Common Properties

- **anchors.fill: parent** - Fills the parent entirely.
- **anchors.centerIn: parent** - Centers inside parent.
- **anchors.top, anchors.bottom, anchors.left, anchors.right** - Attach edges.
- **anchors.margins** - Add spacing from attached edges.

---

<a id="sec-qml-elements"></a>
## 📚 QML Elements

##### ApplicationWindow

###### Overview
`ApplicationWindow` is the root visual window for QML applications that use the `QtQuick.Controls` module.  
It acts as the top-level container for all UI elements and provides built-in support for headers, footers, menus and toolbars.  
It provides the base surface where layouts, components and visual themes are rendered.

###### Basic Properties

- **visible** | bool  
Determines if the window is displayed on screen. You usually set this to `true` so your app appears.

- **width** | int  
Sets the window's width in pixels. Example: `width: 800`.

- **height** | int  
Sets the window's height in pixels. Example: `height: 480`.

- **color** | color  
Defines the background color. Example: `color: "#ffffff"`.

- **title** | string  
The text shown in the OS title bar. Example: `title: "Instrument Cluster"`.

- **flags** | Qt.WindowFlags  
Controls how the window behaves - frameless, fullscreen, etc. Example: `flags: Qt.FramelessWindowHint`.

###### Layout & Structure Properties

- **header** | Item  
An optional area at the top for navigation or titles.

- **footer** | Item  
An optional area at the bottom for status info or controls.

- **contentItem** | Item (read-only)  
The main container where your app's layout goes.

- **background** | Item  
The background visual behind everything (color, gradient or image).

- **menubar** | Item  
Optional area for menus (mostly for desktop).

###### Functional Properties

- **activeFocusControl** | Control (read-only)  
The control (like a text field or button) that currently has keyboard focus. Useful if you want to check which item the user is interacting with.

- **font** | font  
The default font used by all controls (like `Label`, `Button`, etc) inside the window.

- **locale** | Locale  
Defines the language and number/date formatting settings for the window.

- **window** | ApplicationWindow  
Let's any item inside your app access its parent window - for example, `window.title = "My app"`.

###### Tips & Notes

- In production or embedded setups, replace `visible: true` with `visibility: "FullScreen"` for a cleaner UI.
- Keep only one `ApplicationWindow` per app - additional windows should use the `Window``type if needed.
- Avoid heavy logic or animations in the root window; delegate those to child components.


##### Loader

###### Overview
`Loader` is a QML type that dynamically loads other QML components at runtime.  
Instead of declaring everything upfront, you can use `Loader` to load (and unload) visual items as needed - improving performance and flexibility in large UIs.  
Think of it like a "placeholder" that you can tell what to display later.  
It's especially useful when switching screens, lazy-loading content, or managing memory efficiently.

###### Basic Properties

- **source** | url  
The path or URL of the QML file to load. Example: `source: "MyComponent.qml"`.

- **sourceComponent** | Component  
Lets you load a QML `Component` instead of a file. Example: `sourceComponent: myComponent`.

- **active** | bool  
Controls whether the loader should actually load its source. Set `active: false` to unload.

- **asynchronous** | bool  
If `true`, loads the component in the background (non-blocking).

- **visible** | bool  
Visibility of the loader item itself.  
Note: it doesn't affect the visibility of the loaded object.

- **opacity** | real  
Sets the opacity of the loader (and its loaded item). Example: `opacity: 0.8`.

- **anchors, x, y, width, height** | Standard layout properties for positioning the loaded item.  

###### Layout & Structure Properties

- **item** | Item (read-only)  
Reference to the loaded QML object. You can access its properties. Example: `loader.item.color = "red"`.

- **progress** | real (read-only)  
Progress value between `0.0` and `1.0` for asynchronous loading.

- **status** | int  
Current state of the loader: `Loader.Null`, `Loader.Ready`, `Loader.Loading` or `Loader.Error`.

- **activeFocus** | bool  
When the loaded item or loader currently has keyboard focus.

###### Functional Properties

- **onLoaded** | signal  
Triggered when the source finished loading successfully.

- **onStatusChanged** | signal  
Emitted when the loader's status changes (for example, from loading to ready).

- **setSource(url)** | method  
Dynamically sets a new source file to load. Example: `loader.setSource("Settings.qml")`.

- **setSourceComponent(component)** | method  
Changes the loaded component dynamically.

- **sourceComponent.destroy()** | method  
Destroys the currently loaded item (useful for freeing memory).

###### Tips & Notes

- Set `active: false` initially, and then enable it when you need the component. This avoids long startup times.
- You can dynamically change screens by calling `setSource("AnotherPage.qml")`.
- Setting `active: false` unloads the current item, freeing up resources - perfect for embedded systems or dashboards.
- Use `asynchronous: true` for smoother transitions when loading large QML files or complex UIs.
- Monitor `status` and `onStatusChanged` to detect loading errors and handle them gracefully.

##### Rectangle

###### Overview
The `Rectangle` is one of the most common and fundamental QML elements.  
It's a simple visual item that draws a rectangle on the screen - often used as backgrounds, containers, or decorative shapes for other components.  
It's a great building block for almost any visual layout - buttons, card, panels, etc.

###### Basic Properties

- **color** | color  
Sets the rectangle's fill color. Example: `color: "#ffffff"`.

- **radius** | real  
Rounds the corners of the rectangle. Example: `radius: 8`

- **border** | group  
Defines border styling. Example: `border.color: "black"` and `border.width: 2`.

- **gradient** | Gradient  
Fills the rectangle with a smooth color transition. Example: `gradient: Gradient { GradientStop { position: 0; color: "red" } GradientStop { position: 1; color: "blue" } }`.

- **antialiasing** | bool  
Enables smooth edges (useful for rounded rectangles). Example: `antialiasing: true`.

###### Layout & Structure Properties

- **x, y** | real  
Position of the rectangle relative to its parent. Example: `x: 150; y:50`.

- **width, height** | int  
Size of the rectangle in pixels. Example: `width: 200; height: 100`.

- **anchors** | group  
Used for aligning the rectangle relative to other items. Example: `anchors.centerIn: parent`.

- **visible** | bool  
Controls whether the rectangle is shown or hidden. Example: `visible: false`.

- **opacity** | real  
Sets the opacity of the loader (and its loaded item). Example: `opacity: 0.8`.

- **rotation** | real  
Rotates the rectangle in degrees. Example: `rotation: 45`.

- **scale** | real  
Scales the rectangle size proportionally. Example: `scale: 1.2`.


###### Functional Properties

- **clip** | bool  
When `true`, child items are clipped to the rectangle's boundaries. Useful for masking overflow.

- **smooth** | bool  
Alias for antialiasing in older versions.

- **implicitWidth / implicitHeight** | real  
Natural size of the rectangle if no explicit size is set.


###### Tips & Notes

- Don't overuse `antialiasing` on many items - it can impact rendering speed on low-power devices.
- Many higher-level QML controls internally use `Rectangle` for their visual background.

<!--
##### x

###### Overview

###### Basic Properties

- **x** | x  
x.

###### Layout & Structure Properties

- **x** | x  
x.

###### Functional Properties

- **x** | x  
x.

###### Tips & Notes

- x
-->


---

<!--
{
  "element": "ApplicationWindow",
  "module": "QtQuick.Controls",
  "level": "beginner",
  "goal": "Explain this QML element in a beginner-friendly, documentation-style format.",
  "source_url": "https://doc.qt.io/qt-6/qml-qtquick-controls-applicationwindow.html",
  "sections": {
    "overview": true,
    "basic_properties": true,
    "layout_properties": true,
    "functional_properties": true,
    "tips_and_notes": true
  },
  "style": {
    "tone": "friendly",
    "format": "markdown",
    "grouped_sections": true,
    "inline_examples": true,
    "code_comments": false
  }
}
} -->



<a id="sec-links"></a>
## 🔗 Links

- https://doc.qt.io/qt-6/qml-qtquick-item.html
- https://doc.qt.io/qt-6/qml-qtquick-controls-applicationwindow.html#activeFocusControl-prop
- https://doc.qt.io/qt-6/qml-qtquick-loader.html
- https://doc.qt.io/qt-6/qml-qtquick-rectangle.html