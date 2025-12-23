# JUCE to VSTGUI Migration Guide

This document provides a detailed comparison between the JUCE and VSTGUI implementations of the Piano Roll Editor.

## Component Mapping

### Main Editor Component

| JUCE | VSTGUI | Description |
|------|--------|-------------|
| `PianoRollEditorComponent` | `PianoRollEditorView` | Main container for the piano roll editor |
| Inherits from `Component` | Inherits from `CViewContainer` | Base class for containers |
| `addAndMakeVisible()` | `addView()` | Adding child views |
| `setBounds()` | `setViewSize()` | Setting view dimensions |
| `repaint()` | `invalid()` | Triggering redraw |

### Grid Component

| JUCE | VSTGUI | Description |
|------|--------|-------------|
| `NoteGridComponent` | `NoteGridView` | Piano roll grid with notes |
| `Component` | `CView` | Base class |
| `Graphics& g` | `CDrawContext* context` | Drawing context |
| `g.fillAll()` | `context->drawRect(..., kDrawFilled)` | Fill rectangle |
| `g.drawLine()` | `context->drawLine()` | Draw line |
| `g.setColour()` | `context->setFillColor()` / `setFrameColor()` | Set colors |

### Individual Note Component

| JUCE | VSTGUI | Description |
|------|--------|-------------|
| `PNoteComponent` | `NoteView` | Individual note representation |
| `ComponentDragger` | Manual mouse tracking | Dragging functionality |
| `ResizableEdgeComponent` | Manual resize in mouse events | Resizing functionality |
| `MouseEvent` | `CPoint&, CButtonState&` | Mouse event parameters |

### Keyboard Component

| JUCE | VSTGUI | Description |
|------|--------|-------------|
| `KeyboardComponent` | `KeyboardView` | Piano keyboard visualization |
| `paint(Graphics&)` | `draw(CDrawContext*)` | Drawing method |

### Timeline Component

| JUCE | VSTGUI | Description |
|------|--------|-------------|
| `TimelineComponent` | `TimelineView` | Bar/beat timeline |
| `setup(bars, pixels)` | `setup(bars, pixels)` | Configuration method (same) |

### Viewport/Scrolling

| JUCE | VSTGUI | Description |
|------|--------|-------------|
| `Viewport` | `CScrollView` | Scrollable container |
| `CustomViewport` | `CustomScrollView` | Custom scroll with callbacks |
| `visibleAreaChanged()` | `valueChanged()` | Scroll position callback |
| `getViewPositionX/Y()` | `getScrollOffset()` | Get scroll position |
| `setViewPosition()` | `setScrollOffset()` | Set scroll position |

### Control Panel

| JUCE | VSTGUI | Description |
|------|--------|-------------|
| `NoteGridControlPanel` | `NoteGridControlPanelView` | Control panel UI |
| `Slider` | Custom control (simplified) | Slider control |
| `ComboBox` | Custom control (simplified) | Dropdown control |
| `ToggleButton` | Custom control (simplified) | Toggle button |

## Data Model (Framework Independent)

These components are identical in both versions:

| Class | Description |
|-------|-------------|
| `NoteModel` | MIDI note with pitch, velocity, start time, length |
| `PRESequence` | Collection of notes forming a sequence |
| `NoteGridStyleSheet` | Visual styling configuration |

## Color System

### JUCE
```cpp
Colours::darkgrey
Colours::lightgrey
Colour(255, 0, 0)
colour.withAlpha(0.5f)
```

### VSTGUI
```cpp
PRE::Colors::DarkGrey
PRE::Colors::LightGrey
CColor(255, 0, 0, 255)
CColor(255, 0, 0, 128) // Alpha in color constructor
```

## Mouse Event Handling

### JUCE
```cpp
void mouseDown(const MouseEvent& e) override;
void mouseDrag(const MouseEvent& e) override;
void mouseUp(const MouseEvent& e) override;
void mouseEnter(const MouseEvent& e) override;
```

### VSTGUI
```cpp
CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;
CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override;
CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override;
CMouseEventResult onMouseEntered(CPoint& where, const CButtonState& buttons) override;
```

## Drawing Operations

### Filling Rectangles

**JUCE:**
```cpp
g.setColour(Colours::darkgrey);
g.fillRect(0, 0, width, height);
```

**VSTGUI:**
```cpp
context->setFillColor(CColor(64, 64, 64, 255));
CRect rect(0, 0, width, height);
context->drawRect(rect, kDrawFilled);
```

### Drawing Lines

**JUCE:**
```cpp
g.setColour(Colours::white);
g.drawLine(x1, y1, x2, y2, thickness);
```

**VSTGUI:**
```cpp
context->setFrameColor(CColor(255, 255, 255, 255));
context->setLineWidth(thickness);
context->drawLine(CPoint(x1, y1), CPoint(x2, y2));
```

### Drawing Text

**JUCE:**
```cpp
g.setColour(Colours::white);
g.drawText("Hello", x, y, width, height, Justification::left);
```

**VSTGUI:**
```cpp
context->setFontColor(CColor(255, 255, 255, 255));
context->setFont(kNormalFont);
CRect textRect(x, y, x + width, y + height);
context->drawString("Hello", textRect, kLeftText);
```

## Coordinate Systems

### JUCE
- Uses `Rectangle<int>` or `Rectangle<float>`
- `setBounds(x, y, width, height)`
- Separate position and size

### VSTGUI
- Uses `CRect`
- `CRect(left, top, right, bottom)` - coordinates, not width/height!
- `setViewSize(CRect(...))`
- Access via `.left`, `.top`, `.right`, `.bottom`
- Get dimensions via `.getWidth()`, `.getHeight()`

## Memory Management

### JUCE
- Automatic with `Component` hierarchy
- Use `std::unique_ptr` for owned objects
- `addAndMakeVisible()` takes ownership

### VSTGUI
- Reference counting with `remember()` / `forget()`
- `addView()` calls `remember()` automatically
- `removeView()` calls `forget()` automatically
- Manual `forget()` when removing from containers

## Key Differences Summary

1. **Coordinate System**: JUCE uses (x, y, width, height), VSTGUI uses (left, top, right, bottom)

2. **Color Alpha**: JUCE uses separate alpha methods, VSTGUI includes alpha in color constructor

3. **Drawing Context**: JUCE passes by reference, VSTGUI passes by pointer

4. **Mouse Events**: JUCE uses method names with "mouse" prefix, VSTGUI uses "onMouse" prefix with return values

5. **Memory Management**: JUCE uses smart pointers, VSTGUI uses reference counting

6. **View Hierarchy**: Both use parent-child relationships, but VSTGUI is more explicit with `remember()`/`forget()`

## Migration Tips

1. **Start with data model**: Keep `NoteModel` and `PRESequence` unchanged - they're framework-independent

2. **Convert coordinate systems carefully**: When translating `setBounds(x, y, w, h)` to `setViewSize()`, remember to convert to `CRect(x, y, x+w, y+h)`

3. **Handle reference counting**: Always `forget()` views you create but don't add to containers

4. **Mouse event return values**: Return `kMouseEventHandled` or `kMouseEventNotHandled` from VSTGUI mouse methods

5. **Drawing context**: Check for null before using `CDrawContext*`

6. **Colors**: Remember VSTGUI `CColor(r, g, b, alpha)` includes alpha as 4th parameter (0-255)

## Example: Converting a Simple Component

### JUCE Version
```cpp
class MyComponent : public Component {
    void paint(Graphics& g) override {
        g.fillAll(Colours::darkgrey);
        g.setColour(Colours::white);
        g.drawRect(getLocalBounds());
    }
};
```

### VSTGUI Version
```cpp
class MyView : public CView {
    void draw(CDrawContext* context) override {
        context->setFillColor(CColor(64, 64, 64, 255));
        context->drawRect(getViewSize(), kDrawFilled);
        context->setFrameColor(CColor(255, 255, 255, 255));
        context->drawRect(getViewSize(), kDrawStroked);
    }
};
```
