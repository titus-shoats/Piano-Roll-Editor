# Win32 Piano Roll Editor

A native Win32 API implementation of the Piano Roll Editor, converted from the original JUCE-based version.

![Win32 Piano Roll Editor](../img.png)

## Overview

This is a complete reimplementation of the Piano Roll Editor using pure Win32 API, designed to run in Visual Studio 2022 (and future versions). The application provides the same piano roll editing functionality as the original JUCE version, but uses native Windows APIs for rendering and interaction.

## Features

- **Piano Roll Grid**: Visual representation of musical notes with time and pitch axes
- **Keyboard Display**: Piano keyboard visualization on the left side showing note positions
- **Timeline**: Bar and beat markers at the top
- **Note Editing**: 
  - Double-click to add notes
  - Click and drag to select multiple notes
  - Delete key to remove selected notes
  - Quantization support for precise timing
- **Playback Marker**: Visual indicator showing current playback position
- **Scrolling**: Full horizontal and vertical scrolling with mouse wheel support
- **MIDI Integration**: Callback system for MIDI note events

## Requirements

- **Operating System**: Windows 10 or later
- **Development Environment**: Visual Studio 2022 (or later)
- **SDK**: Windows 10 SDK
- **C++ Standard**: C++17 or later

## Building the Project

### Visual Studio 2022

1. Open `PianoRollEditor.sln` in Visual Studio 2022
2. Select your desired configuration (Debug or Release)
3. Select your platform (Win32 or x64)
4. Build the solution (Ctrl+Shift+B)
5. Run the application (F5)

### Command Line (MSBuild)

```batch
# For x64 Release build
msbuild PianoRollEditor.sln /p:Configuration=Release /p:Platform=x64

# For x64 Debug build
msbuild PianoRollEditor.sln /p:Configuration=Debug /p:Platform=x64
```

## Project Structure

```
Win32Version/
├── PianoRollEditor.sln          # Visual Studio solution file
├── PianoRollEditor.vcxproj      # Visual Studio project file
├── README.md                    # This file
├── include/                     # Header files
│   ├── PConstants.h            # Constants and type definitions
│   ├── NoteModel.h             # Data model for musical notes
│   └── PianoRollEditor.h       # Main editor class
└── src/                        # Source files
    ├── main.cpp                # Application entry point and window procedure
    ├── NoteModel.cpp           # Note model implementation
    └── PianoRollEditor.cpp     # Main editor implementation
```

## Usage

### Basic Setup

```cpp
// In your window procedure WM_CREATE handler
g_editor = new PianoRollEditor(hwnd);
g_editor->setup(10, 900, 20);  // 10 bars, 900 pixels per bar, 20 pixels per note

// Set up MIDI callback
g_editor->sendChange = [](int note, int velocity) {
    // Send MIDI note on/off messages here
    std::cout << "MIDI: Note " << note << " Velocity " << velocity << "\n";
};
```

### Loading and Saving Sequences

```cpp
// Create a sequence
PRESequence sequence;

// Add notes
NoteModel note1(60, 100, 0, PRE::defaultResolution);  // Middle C
NoteModel note2(64, 100, PRE::defaultResolution, PRE::defaultResolution);  // E
sequence.events.push_back(note1);
sequence.events.push_back(note2);

// Load into editor
g_editor->loadSequence(sequence);

// Get sequence back
PRESequence currentSequence = g_editor->getSequence();
```

### Playback

```cpp
// Update playback position (typically from a timer)
g_editor->setPlaybackMarkerPosition(currentTick, true);
```

## API Reference

### PianoRollEditor Class

#### Constructor
```cpp
PianoRollEditor(HWND hwnd)
```
Creates a new piano roll editor instance for the specified window.

#### Methods

- `void setup(int bars, int pixelsPerBar, int noteHeight)`
  - Initialize the grid with specified parameters
  
- `void loadSequence(PRESequence sequence)`
  - Load a sequence of notes into the editor
  
- `PRESequence getSequence()`
  - Get the current sequence of notes
  
- `void setPlaybackMarkerPosition(st_int ticks, bool isVisible = true)`
  - Update the playback marker position
  
- `void setScroll(int x, int y)`
  - Programmatically set scroll position

#### Callbacks

- `std::function<void()> onEdit`
  - Called when the note grid is edited
  
- `std::function<void(int note, int velocity)> sendChange`
  - Called when notes are triggered (for MIDI output)

### NoteModel Class

Represents a single musical note with:
- `note` (0-127): MIDI note number
- `velocity` (0-127): Note velocity
- `startTime`: Start time in ticks
- `noteLegnth`: Duration in ticks

### PRESequence Class

Container for a sequence of notes with metadata:
- `events`: Vector of NoteModel objects
- `tsLow`, `tsHigh`: Time signature (e.g., 4/4)
- `lowNote`, `highNote`: Range of notes in the sequence

## Architecture

The Win32 implementation replaces JUCE components with native Windows equivalents:

| JUCE Component | Win32 Equivalent |
|----------------|------------------|
| JUCEApplication | WinMain + Window Procedure |
| Component | Custom rendering in WM_PAINT |
| Graphics | GDI (BitBlt, CreatePen, etc.) |
| Viewport | ScrollBar + manual clipping |
| Timer | WM_TIMER messages |
| MouseEvent | WM_MOUSE* messages |
| KeyListener | WM_KEYDOWN messages |

## Controls

- **Double-Click**: Add a new note at the clicked position
- **Shift + Click + Drag**: Select multiple notes
- **Delete/Backspace**: Delete selected notes
- **Mouse Wheel**: Scroll vertically
- **Scroll Bars**: Navigate horizontally and vertically

## Customization

### Colors

Edit the color constants in `PianoRollEditor.h`:

```cpp
static const int GRID_COLOR = RGB(60, 60, 60);
static const int BG_COLOR = RGB(40, 40, 40);
static const int NOTE_COLOR = RGB(100, 150, 200);
static const int NOTE_SELECTED_COLOR = RGB(150, 200, 255);
```

### Grid Settings

Modify the default setup parameters in `main.cpp`:

```cpp
g_editor->setup(
    10,    // Number of bars
    900,   // Pixels per bar
    20     // Height of each note in pixels
);
```

## Differences from JUCE Version

1. **Native Windows Look**: Uses standard Windows controls and GDI rendering
2. **No External Dependencies**: Only requires Windows SDK
3. **Simplified Build**: No JUCE framework setup needed
4. **Direct Win32 API**: More control over Windows-specific features
5. **Lightweight**: Smaller executable size

## Troubleshooting

### Build Errors

- Ensure you have Windows 10 SDK installed
- Check that C++17 language standard is enabled
- Verify include directories are set correctly

### Runtime Issues

- If notes don't appear, check the setup() parameters
- For scrolling issues, verify scroll bar ranges
- Check console output for MIDI callback messages

## Future Enhancements

Potential improvements for future versions:

- [ ] Direct2D rendering for smoother graphics
- [ ] Note velocity editing via mouse drag
- [ ] Multi-note dragging and resizing
- [ ] Undo/Redo functionality
- [ ] Copy/paste support
- [ ] MIDI file import/export
- [ ] Customizable color schemes
- [ ] Snap-to-grid options
- [ ] Different time signatures

## License

This Win32 implementation follows the same license as the original JUCE-based Piano Roll Editor project. See the LICENSE file in the root directory.

## Acknowledgments

- Original JUCE implementation by Samuel Hunt
- Converted to Win32 API for broader accessibility and Visual Studio integration

## Support

For issues, questions, or contributions related to this Win32 version, please refer to the main repository's issue tracker.
