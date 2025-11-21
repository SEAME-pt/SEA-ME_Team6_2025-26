# 🚀 QML Guide

## 📚 Index
 - [👋 Introduction](#sec-intro)
 - [🧠 Core Ideas](#sec-core-ideas)
 - [🧱 Visual Blocks](#sec-core-visual-blocks)
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
## 🧱 Visual Blocks

### Item
`Item` is the invisible base class for almost all visual QML elements.  
You can think of it as a container or a positioning box - it doesn't draw anything itself, but defines where and how things appear.

#### Common Properties

- **x, y** - Position relative to the parent.
- **width, height** - Size of the item.
- **visible** - Controls if the item is shown on screen.
- **opacity** - How transparent the item is (1.0 = fully visible, 0.0 = invisible).
- **rotation, scale** - Transformation properties.
- **anchors** - Layout system to align items relative to other items.
- **parent** - Reference to the parent item in the visual tree.

### Anchors
`Anchors` are QML's built-in layout system for aligning and positioning elements relative to each other.  
They replace manual `x` and `y` coordinates with logical relationships, making your UI much more flexible.

#### Common Properties

- **anchors.fill: parent** - Fills the parent entirely.
- **anchors.centerIn: parent** - Centers inside parent.
- **anchors.top, anchors.bottom, anchors.left, anchors.right** - Attach edges.
- **anchors.margins** - Add spacing from attached edges.

---

<a id="sec-qml-elements"></a>
## 📚 QML Elements

### ApplicationWindow

**🌐 Module** - `QtQuick.Controls`  
**🔗 Reference** - [Qt Documentation - Application Window](https://doc.qt.io/qt-6/qml-qtquick-controls-applicationwindow.html)

#### Overview
`ApplicationWindow` is the root visual window for QML applications that use the `QtQuick.Controls` module.  
It acts as the top-level container for all UI elements and provides built-in support for headers, footers, menus and toolbars.  
It provides the base surface where layouts, components and visual themes are rendered.  

**Example:**
```qml
ApplicationWindow {
    visible: true
    width: 800
    height: 480
    title: "Instrument Cluster"

    Rectangle {
        anchors.fill: parent
        color: "#1e1e1e"
    }
}
```

#### Basic Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `visible` | `bool` | Determines if the window is displayed on screen. Usually set to `true` so your app appears. | `visible: true` |
| `width` | `int` | Sets the window’s width in pixels. | `width: 800` |
| `height` | `int` | Sets the window’s height in pixels. | `height: 480` |
| `color` | `color` | Defines the background color. | `color: "#ffffff"` |
| `title` | `string` | Text shown in the OS title bar. | `title: "Instrument Cluster"` |
| `flags` | `Qt.WindowFlags` | Controls window behavior — frameless, fullscreen, etc. | `flags: Qt.FramelessWindowHint` |

---

#### Layout & Structure Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `header` | `Item` | Optional area at the top for navigation or titles. | `header: ToolBar {}` |
| `footer` | `Item` | Optional area at the bottom for status info or controls. | `footer: Rectangle { height: 50 }` |
| `contentItem` | `Item (read-only)` | Main container where your app’s layout goes. | — |
| `background` | `Item` | Visual element behind everything (color, gradient, or image). | `background: Rectangle { color: "#000" }` |
| `menubar` | `Item` | Optional area for menus (mostly for desktop apps). | — |

---

#### Functional Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `activeFocusControl` | `Control (read-only)` | The control (like a text field or button) that currently has keyboard focus. Useful for tracking user interaction. | `if (window.activeFocusControl) console.log("focused")` |
| `font` | `font` | Default font used by all controls (e.g. `Label`, `Button`) inside the window. | `font.family: "Roboto"` |
| `locale` | `Locale` | Defines the language and number/date formatting settings for the window. | `locale: Qt.locale("en_US")` |
| `window` | `ApplicationWindow` | Lets any item access its parent window. | `window.title = "My App"` |


#### Tips & Notes

- In production or embedded setups, replace `visible: true` with `visibility: "FullScreen"` for a cleaner UI.
- Keep only one `ApplicationWindow` per app - additional windows should use the `Window``type if needed.
- Avoid heavy logic or animations in the root window; delegate those to child components.


### Loader

**🌐 Module** - `QtQuick`  
**🔗 Reference** - [Qt Documentation - Loader](https://doc.qt.io/qt-6/qml-qtquick-loader.html)

#### Overview
`Loader` is a QML type that dynamically loads other QML components at runtime.  
Instead of declaring everything upfront, you can use `Loader` to load (and unload) visual items as needed - improving performance and flexibility in large UIs.  
Think of it like a "placeholder" that you can tell what to display later.  
It's especially useful when switching screens, lazy-loading content, or managing memory efficiently.  

**Example:**
```qml
Loader {
    id: dynamicLoader
    source: "Dashboard.qml"
    active: true
}
```

#### Basic Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `source` | `url` | Path or URL of the QML file to load. | `source: "MyComponent.qml"` |
| `sourceComponent` | `Component` | Loads a QML `Component` instead of a file. | `sourceComponent: myComponent` |
| `active` | `bool` | Controls whether the loader should actually load its source. Set `active: false` to unload. | `active: true` |
| `asynchronous` | `bool` | If `true`, loads the component in the background (non-blocking). | `asynchronous: true` |
| `visible` | `bool` | Visibility of the loader item itself. *(Does not affect the visibility of the loaded object.)* | `visible: true` |
| `opacity` | `real` | Sets the opacity of the loader and its loaded item. | `opacity: 0.8` |
| `anchors, x, y, width, height` | — | Standard layout properties for positioning the loaded item. | `anchors.centerIn: parent` |

---

#### Layout & Structure Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `item` | `Item (read-only)` | Reference to the loaded QML object. You can access its properties. | `loader.item.color = "red"` |
| `progress` | `real (read-only)` | Progress value between `0.0` and `1.0` for asynchronous loading. | `console.log(loader.progress)` |
| `status` | `int` | Current loader state: `Loader.Null`, `Loader.Ready`, `Loader.Loading`, or `Loader.Error`. | `if (loader.status === Loader.Error)` |
| `activeFocus` | `bool` | Indicates if the loaded item or loader currently has keyboard focus. | `activeFocus: true` |

---

#### Functional Properties

| **Property / Signal / Method** | **Type** | **Description** | **Example** |
|--------------------------------|-----------|------------------|--------------|
| `onLoaded` | `signal` | Triggered when the source finishes loading successfully. | `onLoaded: console.log("Loaded!")` |
| `onStatusChanged` | `signal` | Emitted when the loader's status changes (e.g., loading → ready). | `onStatusChanged: handleStatus(loader.status)` |
| `setSource(url)` | `method` | Dynamically sets a new source file to load. | `loader.setSource("Settings.qml")` |
| `setSourceComponent(component)` | `method` | Dynamically changes the loaded component. | `loader.setSourceComponent(myComponent)` |
| `sourceComponent.destroy()` | `method` | Destroys the currently loaded item (frees memory). | `loader.sourceComponent.destroy()` |


#### Tips & Notes

- Set `active: false` initially, and then enable it when you need the component. This avoids long startup times.
- You can dynamically change screens by calling `setSource("AnotherPage.qml")`.
- Setting `active: false` unloads the current item, freeing up resources - perfect for embedded systems or dashboards.
- Use `asynchronous: true` for smoother transitions when loading large QML files or complex UIs.
- Monitor `status` and `onStatusChanged` to detect loading errors and handle them gracefully.

### Rectangle

**🌐 Module** - `QtQuick`  
**🔗 Reference** - [Qt Documentation - Rectangle](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

#### Overview
The `Rectangle` is one of the most common and fundamental QML elements.  
It's a simple visual item that draws a rectangle on the screen - often used as backgrounds, containers, or decorative shapes for other components.  
It's a great building block for almost any visual layout - buttons, card, panels, etc.  

**Example:**
```qml
Rectangle {
    width: 200
    height: 100
    color: "#4CAF50"
    radius: 12
}
```

#### Basic Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `color` | `color` | Sets the rectangle's fill color. | `color: "#ffffff"` |
| `radius` | `real` | Rounds the corners of the rectangle. | `radius: 8` |
| `border` | `group` | Defines border styling. | `border.color: "black"` / `border.width: 2` |
| `gradient` | `Gradient` | Fills the rectangle with a smooth color transition. | `gradient: Gradient { GradientStop { position: 0; color: "red" } GradientStop { position: 1; color: "blue" } }` |
| `antialiasing` | `bool` | Enables smooth edges (useful for rounded rectangles). | `antialiasing: true` |

---

#### Layout & Structure Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `x, y` | `real` | Position of the rectangle relative to its parent. | `x: 150; y: 50` |
| `width, height` | `int` | Size of the rectangle in pixels. | `width: 200; height: 100` |
| `anchors` | `group` | Used for aligning the rectangle relative to other items. | `anchors.centerIn: parent` |
| `visible` | `bool` | Controls whether the rectangle is shown or hidden. | `visible: false` |
| `opacity` | `real` | Sets the rectangle's opacity. | `opacity: 0.8` |
| `rotation` | `real` | Rotates the rectangle in degrees. | `rotation: 45` |
| `scale` | `real` | Scales the rectangle size proportionally. | `scale: 1.2` |

---

#### Functional Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `clip` | `bool` | When `true`, child items are clipped to the rectangle's boundaries (useful for masking overflow). | `clip: true` |
| `smooth` | `bool` | Alias for antialiasing in older Qt versions. | `smooth: true` |
| `implicitWidth, implicitHeight` | `real` | Natural size of the rectangle if no explicit size is set. | — |


#### Tips & Notes

- Don't overuse `antialiasing` on many items - it can impact rendering speed on low-power devices.
- Many higher-level QML controls internally use `Rectangle` for their visual background.

### RowLayout

**🌐 Module** - `QtQuick.Layouts`  
**🔗 Reference** - [Qt Documentation - RowLayout](https://doc.qt.io/qt-6/qml-qtquick-layouts-rowlayout.html)

#### Overview
`RowLayout` is a layout manager that arranges its child items horizontally in a row.  
Instead of manually positioning items with x or anchors, RowLayout automatically handles sizing, spacing and alignment - making your UI adaptive and clean.  

**Example:**
```qml
RowLayout {
    width: 300
    spacing: 10

    Rectangle { color: "lightblue"; Layout.preferredWidth: 50; height: 40 }
    Rectangle { color: "lightgreen"; Layout.fillWidth: true; height: 40 }
    Rectangle { color: "tomato"; Layout.preferredWidth: 80; height: 40 }
}
```

#### Basic Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `spacing` | `int` | Defines the space (in pixels) between child items. | `spacing: 12` |
| `enabled` | `bool` | Controls whether the layout is active. If false, children won't be arranged automatically. | `enabled: true` |
| `visible` | `bool` | Controls whether the layout(and its children) are visible. | `visible: true` |


#### Layout & Structure Properties
Each item inside a `RowLayout` can define how it behaves within the layout using special `Layout.*` properties.  

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `Layout.preferredWidth` | `real` | Preferred width of the item in the layout. | `Layout.preferredWidth: 80` |
| `Layout.minimumWidth` | `real` | The minimum allowed width before shrinking. | `Layout.minimumWidth: 40: ` |
| `Layout.maximumWidth` | `real` | The maximum width allowed before stretching. | `Layout.maximumWidth: 150` |
| `Layout.fillWidth` | `bool` | If true, the item expands to take remaining space. | `Layout.fillWidth: true` |
| `Layout.alignment` | `enumeration` | Aligns the item vertically or horizontally within its cell. Use constants like `Qt.AlignLeft`, `Qt.AlignVCenter`. | `Layout.alignment: Qt.AlignHCenter` |
| `Layout.margins` | `int` | Sets outer spacing around the item. | `Layout.margins: 8` |
| `Layout.row, Layout.column` | `int` | Used when the layout is nested within a grid - not usually needed for `RowLayout`. | `Layout.row: 0` |

#### Functional Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `Automatic Resizing` | - | If a child uses `Layout.fillWidth: true`, it stretches when the parent resizes. | A progress bar expanding across the screen. |
| `Nested Layouts` | - | You can place a `RowLayout` inside a `ColumnLayout` to build responsive designs. | `ColumnLayout { RowLayout { ... }}` |
| `Mirroring` | `bool` | Automatically mirrors layout directions for right-to-left locales. | `LayoutMirroring.enabled: true` |

#### Tips & Notes

- `RowLayout` is ideal when you want automatic spacing and resizing without anchors.
- Combine with `ColumnLayout` for complex dashboard-style UIs.
- Use `Layout.fillWidth` and `Layout.preferredWidth` instead of fixed widths for a responsive design.
- For maximum performance, avoid mixing `anchors` and `Layout` properties on the same item.
- You can use invisible `Item { width: x }` inside a layout as a spacer between controls.

### ColumnLayout

**🌐 Module** - `QtQuick.Layouts`  
**🔗 Reference** - [Qt Documentation - ColumnLayout](https://doc.qt.io/qt-6/qml-qtquick-layouts-columnlayout.html)

#### Overview
`ColumnLayout` is a QML type module that arranges its child items vertically in a single column.  
It's part of the layout system that helps you position and size multiple elements without having to manually calculate coordinates.  

**Example:**  
```qml
ColumnLayout {
    anchors.fill: parent
    spacing: 12

    Rectangle { color: "#2196F3"; Layout.fillWidth: true; Layout.preferredHeight: 50 }
    Rectangle { color: "#4CAF50"; Layout.fillWidth: true; Layout.preferredHeight: 100 }
    Rectangle { color: "#FFC107"; Layout.fillWidth: true; Layout.preferredHeight: 75 }
}
```

#### Basic Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `spacing` | `int` | Defines the space (in pixels) between child items. | `spacing: 12` |
| `enabled` | `bool` | Controls whether the layout is active. If false, children won't be arranged automatically. | `enabled: true` |
| `visible` | `bool` | Controls whether the layout(and its children) are visible. | `visible: true` |

#### Layout & Structure Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `anchors` | `group` | Allows you to anchor the layout within its parent. | `anchors.fill: parent ` |
| `Layout.fillWidth` | `bool` | Makes a child expand horizontally to fill available width. | `Layout.fillWidth: true` |
| `Layout.fillHeight` | `bool` | Makes a child expand vertically (only meaningful in nested layouts). | `Layout.fillHeight: true` |
| `Layout.preferredWidth` | `real` | Preferred width of the item in the layout. | `Layout.preferredWidth: 80` |
| `Layout.preferredHeight` | `real` | Preferred height of the item in the layout. | `Layout.preferredHeight: 120` |
| `Layout.alignment` | `enumeration` | Aligns the item vertically or horizontally within its cell. Use constants like `Qt.AlignLeft`, `Qt.AlignVCenter`. | `Layout.alignment: Qt.AlignHCenter` |

#### Functional Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `children` | `list<item>` | The visual items that are stacked vertically. | (implicit) add items inside `ColumnLayout: { ... } ` |
| `count` | `int` | Number of child items currently managed by the layout. | `console.log(column.count)` |
| `addItem(item)` | `method` | Adds a child dynamically at runtime. | `column.addItem(Rectangle {})` |
| `removeItem(item)` | `method` | Removes a child from the layout. | `column.removeItem(rect)` |

#### Tips & Notes

- Use `ColumnLayout` inside an `ApplicationWindow` or another layout to easily create responsive UIs.
- Combine with `RowLayout` for flexible, grid-like structures.
- Avoid mixing `anchors` and layout properties (`Layout.*`) on the same child.
- Ideal for toolbars, stacked panels, or from layouts.

### Text

**🌐 Module** - `QtQuick`  
**🔗 Reference** - [Qt Documentation - Text](https://doc.qt.io/qt-6/qml-qtquick-text.html)

#### Overview
`Text` is one of the most commonly used QML elements - it displays a pice of text on screen.  
It's simple but powerful, supporting features like font customization, color styling and alignment formatting.  
You'll often use `Text` to show labels, data values, or dynamic content that changes with backend properties.  

**Example:**
```qml
Text {
  text: "Speed: 120 km/h"
  color: "white"
  font.pixelSize: 32
  anchors.centerIn: parent
}
```

#### Basic Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `text` | `string` | The actual text to display. | `text: "Hello QML!"` |
| `color` | `color` | Text color. | `color: "#ffffff"` |
| `font.family` | `string` | The font family name. | `font.family: "Roboto"` |
| `font.pixelSize` | `int` | Font size in pixels. | `font.pixelSize: 24` |
| `font.bold` | `bool` | Makes the text bold. | `font.bold: true` |
| `font.italic` | `bool` | Italicizes the text. | `font.italic: true` |
| `horizontalAlignment` | `enumeration` | Aligns text horizontally: `Text.AlignLeft`, `Text.AlignHCenter`, `Text.AlignRight`. | `horizontalAlignment: Text.AlignHCenter` |
| `verticalAlignment` | `enumeration` | Aligns text vertically: `Text.AlignTop`, `Text.AlignVCenter`, `Text.AlignBottom`. | `verticalAlignment: Text.AlignVCenter` |
| `visible` | `bool` | Controls whether the text is visible. | `visible: true` |

#### Layout & Structure Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `x, y` | `real` | Position of the text item in its parent. | `x: 50; y: 20` |
| `width, height` | `int` | Defines the bounding box of the text. Use within wrapping or clipping. | `width: 200` |
| `anchors` | `group` | Position text relative to its parent(e.g, center, top, etc). | `anchors.centerIn: parent` |
| `wrapMode` | `enumeration` | Controls how long text wraps: `Text.NoWrap`, `Text.WordWrap`, `Text.WrapAnywhere` . | `wrapMode: Text.WordWrap` |
| `elide` | `enumeration` | If text is too long, shows "...": `Text.ElideRight`, `Text.ElideLeft`. | `elide: Text.ElideRight` |
| `opacity` | `real` | Transparency level from `0.0` to `1.0`. | `opacity: 0.8` |
| `rotation` | `real` | Rotates the text by a number of degrees. | `rotation: 15` |

#### Functional Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `textFormat` | `enumeration` | Specifies the text format: `Text.PlainText`, `Text.RichText` or `Text.StyledText`. | `textFormat: Text.RichText` |
| `renderType` | `enumeration` | Rendering method for text(e.g, `Text.NativeRendering`, `Text.QtRendering`). | `renderType: Text.QtRendering` |
| `lineHeight` | `real` | Adjusts spacing between lines when wrapping. | `lineHeight: 1.2` |
| `clip` | `bool` | If true, hides text outside the bounding box. | `clip: true` |
| `maximumLineCount` | `int` | Limits the number of lines shown. | `maximumLineCount: 2` |
| `linkActivated(link)` | `signal` | Emitted when the user clicks a hyperlink(when using rich text). | `onLinkActivated: console.log(link) ` |

#### Tips & Notes

- `Text` doesn't automatically wrap - remember to set `wrapMode: Text.WordWrap` if you expect long lines.
- Use `Text.RichText` to include simple HTML formatting like `<b>`, `<i>`, `<font color='red'>`.
- When animating color, opacity, or rotation, consider using `Behavior` or `NumberAnimation` for smooth effects.
- For performance with rapidly changing text (like FPS or speed readouts), avoid unnecessary `Text.RichText` - it's heavier than plain text.

<!--
### x

**🌐 Module** - `x`  
**🔗 Reference** - [Qt Documentation - x](x)

#### Overview

#### Basic Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `x` | `x` | x. | `x: ` |

#### Layout & Structure Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `x` | `x` | x. | `x: ` |

#### Functional Properties

| **Property** | **Type** | **Description** | **Example** |
|---------------|-----------|------------------|--------------|
| `x` | `x` | x. | `x: ` |

#### Tips & Notes

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
    "code_comments": false,
    "properties_display": {
      "type": "table",
      "columns": ["Property", "Type", "Description", "Example"],
      "group_by": ["basic", "layout", "functional"],
      "collapsible_sections": true,
      "show_icons": true,
      "markdown_format": true
    }
  }
}
-->



<a id="sec-links"></a>
## 🔗 Links

- https://doc.qt.io/qt-6/qml-qtquick-item.html
- https://doc.qt.io/qt-6/qml-qtquick-controls-applicationwindow.html
- https://doc.qt.io/qt-6/qml-qtquick-loader.html
- https://doc.qt.io/qt-6/qml-qtquick-rectangle.html
- https://doc.qt.io/qt-6/qml-qtquick-layouts-rowlayout.html
- https://doc.qt.io/qt-6/qml-qtquick-layouts-columnlayout.html
- https://doc.qt.io/qt-6/qml-qtquick-text.html
