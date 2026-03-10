# QuickEditorWidgetBlueprint

A **Blueprint-friendly UI framework for Unreal Editor tools** inspired by **Unity’s EditorGUILayout**.

QuickEditorWidgetBlueprint allows you to create **custom editor windows entirely from Blueprint or C++**, using simple layout commands like:
BeginVertical
Button
TextField
EndVertical

without manually building complex UMG widget trees.

---

## Features

- Dynamic UI construction
- Callbacks for every control
- Object & class pickers
- Vector fields
- Runtime UI modification
- Layout stacks
- Modal windows, normal windows, and tabs

The system is designed for **rapid editor tool creation**.

---

# Core Concept

The system works similarly to **Unity EditorGUILayout**.

Instead of constructing a widget hierarchy manually, you describe the UI sequentially:
StartWindow(Title)

---

## Tab Window

Creates a **dockable editor tab**.
StartTab(Title)

This window behaves like any normal Unreal editor tab.

Best used for:

- Persistent tools  
- Inspectors  
- Asset editors  

---

## Modal Window

Creates a **blocking modal dialog**.

CreateModalWindow
ShowModalWindow

Modal windows are ideal for:

- Confirmation dialogs  
- Import tools  
- Setup wizards  

Example:
Handle = CreateModalWindow()

BeginVertical
Label("Create Actor")
Button("Create")
EndVertical

ShowModalWindow

---

# Layout System

The UI is built using a **layout stack**.

Layouts define how widgets are arranged.

---

## Vertical Layout

Stacks widgets vertically.
BeginVertical
Button
Button
EndVertical

Result:
Button
Button

---

## Horizontal Layout

Places widgets side-by-side.
BeginHorizontal
Button
Button
EndHorizontal

Result:
Button Button

---

## Nested Layouts

Layouts can be nested freely.
BeginVertical
Label
BeginHorizontal
    Button
    Button
EndHorizontal
EndVertical

---

## Layout Safety

If a layout is not closed, the system automatically normalizes the layout stack before showing the window.

Example mistake:
BeginVertical
Button

The system automatically closes the layout.

---

# Slot Rules

Each element supports layout sizing rules.
Fill
Auto

Example:
Button(Id, Text, Fill)
Button(Id, Text, Auto)

`Fill` elements stretch to occupy remaining space.

---

# UI Elements

## Label

Displays text.
Label("Hello World")

---

## Button

Clickable action button.
Button("CreateButton", "Create Actor")

Callbacks can be bound using:
BindButtonClicked

---

## Toggle (Checkbox)

Boolean control.

Toggle("EnableFeature", "Enable Feature", true)
Stored in:
Handle->BoolValues

Callback:
BindBoolChanged

---

## Text Field

Editable text input.
TextField("NameField", "Name", "Default")

Stored in:
Handle->StringValues

Callback:
BindStringChanged

---

## Enum Popup

Dropdown for enum values.
EnumPopup("Mode", "Mode", MyEnum)

Stored in:
Handle->StringValues

Callback:
BindStringChanged

---

## Object Picker

Allows selecting an object or asset.
AddObjectPicker("Mesh", "Mesh", StaticMeshClass)

Stored in:
Handle->ObjectValues

Callback:
BindObjectChanged

---

## Class Picker

Allows selecting a class derived from a base class.
AddClassPicker("ActorClass", "Actor Class", AActor)

Stored in:
Handle->ClassValues

Callback:
BindClassChanged

---

## Vector Field

Editable 3D vector.
AddVectorField("Location", "Location", FVector(0,0,0))

Stored in:
Handle->VectorValues

Callback:
BindVectorChanged

---

# Callbacks

Each control type supports **dedicated callbacks**.

Examples:
BindButtonClicked
BindBoolChanged
BindStringChanged
BindObjectChanged
BindClassChanged
BindVectorChanged

Callbacks fire whenever the value changes.

---

# Event System

All interactions generate events.

Events contain:
Type
Id
ValueType
Value

Example event types:
Clicked
ValueChanged
SelectionChanged

Events can be retrieved via:
PollEvents()

---

# Accessing Values

Each field stores its value inside the window handle.

Examples:
Handle->BoolValues
Handle->StringValues
Handle->ObjectValues
Handle->ClassValues
Handle->VectorValues


Example:
Handle->StringValues["NameField"]

BeginVertical("Inventory")

This allows widgets to be added dynamically to specific layouts.

---

# Initialization

To ensure callbacks fire once when the UI is built:

BroadcastCurrentValues(Handle)


This triggers callbacks using the current stored values.

---

# Internal Architecture

Key classes:
QEWB_Subsystem
QEWB_WindowHandle
QEWB_HostWidget
QEWB_InternalProxies


---

## Subsystem

Central API used by Blueprint or C++.

Responsible for:

- Creating windows  
- Building UI  
- Dispatching callbacks  

---

## Window Handle

Stores the complete state of a window:

Layouts
Widgets
Values
Callbacks
Events


All UI operations operate through the handle.

---

## Host Widget

Custom `EditorUtilityWidget` used as the root UI container.

Responsible for:

- Creating the root layout  
- Cleaning up when destroyed  

---

## Proxy Objects

Proxy objects are used to bind dynamic delegates.

They forward UI events into the window handle.

Examples:
ButtonProxy
VectorProxy
PropertyViewProxy

---

# Typical Example

Example tool window:
Handle = StartWindow()

BeginVertical
TextField("Name", "Actor Name", "NewActor")

AddClassPicker("ActorClass", "Actor Class", AActor)

AddVectorField("Location", "Location", (0,0,100))

Button("SpawnButton", "Spawn Actor")
EndVertical

BindButtonClicked("SpawnButton")


---

# Recommended Use Cases

QuickEditorWidgetBlueprint is ideal for:

- Batch asset tools  
- Import pipelines  
- Actor placement tools  
- Data editors  
- Debugging utilities  
- Custom editor dialogs  
- Content management tools  

---

# Design Goals

The plugin aims to provide:

- Extremely fast UI construction
- Minimal UMG boilerplate
- Blueprint-first workflows
- Unity-style layout building
- Dynamic editor tool interfaces
