# Quick Start Guide - Win32 Piano Roll Editor

This guide will help you get the Win32 Piano Roll Editor up and running in minutes.

## Prerequisites

Before you begin, ensure you have:
- ✅ Windows 10 or later
- ✅ Visual Studio 2022 (Community, Professional, or Enterprise)
- ✅ "Desktop development with C++" workload installed in Visual Studio

If you don't have Visual Studio 2022, download it from: https://visualstudio.microsoft.com/downloads/

## Step-by-Step Setup

### 1. Clone or Download the Repository

```bash
git clone https://github.com/titus-shoats/Piano-Roll-Editor.git
cd Piano-Roll-Editor/Win32Version
```

Or download and extract the ZIP file, then navigate to the `Win32Version` folder.

### 2. Open the Solution

Double-click `PianoRollEditor.sln` or:
1. Open Visual Studio 2022
2. Select "Open a project or solution"
3. Navigate to `Win32Version/PianoRollEditor.sln`
4. Click "Open"

### 3. Build the Project

**Option A: Using the UI**
1. Select "Debug" or "Release" from the configuration dropdown
2. Select "x64" or "Win32" from the platform dropdown
3. Press `Ctrl+Shift+B` or go to Build → Build Solution

**Option B: Using Developer Command Prompt**
```batch
cd Win32Version
msbuild PianoRollEditor.sln /p:Configuration=Release /p:Platform=x64
```

### 4. Run the Application

Press `F5` or click the green "Start" button in Visual Studio.

The Piano Roll Editor window should appear!

## First Steps

### Adding Notes

1. **Double-click** anywhere on the grid to add a note
2. The note will snap to the grid based on quantization settings
3. Notes appear as colored rectangles on the grid

### Selecting Notes

1. Hold **Shift** and click-drag to create a selection rectangle
2. All notes within the rectangle will be selected
3. Selected notes are highlighted

### Deleting Notes

1. Select one or more notes
2. Press **Delete** or **Backspace** key

### Navigating the Grid

- **Mouse Wheel**: Scroll vertically through notes
- **Horizontal Scrollbar**: Navigate through time (bars)
- **Vertical Scrollbar**: Navigate through pitch (notes)

### Viewing the Keyboard

The piano keyboard on the left shows which notes correspond to which pitch.
- White keys: Natural notes (C, D, E, F, G, A, B)
- Black keys: Sharps/flats (C#, D#, F#, G#, A#)
- Note names shown on C notes (C0, C1, C2, etc.)

### Playback Marker

The green vertical line shows the current playback position. It automatically loops through the bars.

## Customization

### Changing Grid Size

Edit the setup parameters in `src/main.cpp`:

```cpp
g_editor->setup(
    10,    // Number of bars (change this)
    900,   // Pixels per bar (wider = more space)
    20     // Note height (taller = easier to see)
);
```

### Changing Colors

Edit color constants in `include/PianoRollEditor.h`:

```cpp
static const int NOTE_COLOR = RGB(100, 150, 200);  // Blue notes
static const int BG_COLOR = RGB(40, 40, 40);       // Dark background
```

RGB values range from 0-255. Try:
- `RGB(255, 100, 100)` for red notes
- `RGB(100, 255, 100)` for green notes
- `RGB(200, 200, 255)` for light blue notes

### MIDI Integration

The application includes a MIDI callback in `src/main.cpp`:

```cpp
g_editor->sendChange = [](int note, int velocity) {
    std::cout << "MIDI send: " << note << " : " << velocity << "\n";
};
```

Replace the `std::cout` line with your MIDI library code to send actual MIDI messages.

## Common Issues

### Build Error: "Windows.h not found"

**Solution**: Install the Windows 10 SDK through Visual Studio Installer:
1. Open Visual Studio Installer
2. Click "Modify" on Visual Studio 2022
3. Select "Windows 10 SDK" under Individual components
4. Click "Modify" to install

### Build Error: "C++17 features not available"

**Solution**: The project is already configured for C++17, but if you see this error:
1. Right-click the project → Properties
2. C/C++ → Language
3. Set "C++ Language Standard" to "C++17" or later
4. Click OK and rebuild

### Application Doesn't Start

**Solution**: 
1. Ensure you built the project successfully (check Output window)
2. Try running as Administrator
3. Check that you selected the correct platform (x64 vs Win32)

### Notes Don't Appear

**Solution**:
1. Try double-clicking in different areas of the grid
2. Check that you're clicking to the right of the keyboard (gray area)
3. Zoom or scroll to see if notes are off-screen

## Next Steps

Now that you have the basic application running:

1. **Read the Full Documentation**: See `README.md` for detailed API reference
2. **Explore the Code**: 
   - `main.cpp` - Application setup and window handling
   - `PianoRollEditor.cpp` - Main editor logic and rendering
   - `NoteModel.cpp` - Data structures
3. **Integrate with Your Project**: The editor can be embedded in larger applications
4. **Customize**: Modify colors, sizes, and behavior to match your needs

## Getting Help

- Check the detailed [README.md](README.md) for comprehensive documentation
- Review the code comments in the source files
- Look at the original JUCE implementation for design inspiration

## Tips for Development

### Debugging

1. Set breakpoints in the window procedure (`WindowProc` in `main.cpp`)
2. Watch the message flow (WM_PAINT, WM_MOUSEMOVE, etc.)
3. Use `OutputDebugString()` to log to Visual Studio's Output window

### Performance

- The application uses double-buffering for smooth rendering
- Rendering is optimized to only draw visible notes
- Consider limiting the number of bars for better performance

### Extending Functionality

Common additions:
- Save/Load functionality (serialize `PRESequence` to file)
- Clipboard operations (copy/paste notes)
- Undo/Redo stack
- Velocity editing
- Note dragging
- Snap-to-grid options

Happy editing! 🎹🎵
