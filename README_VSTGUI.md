# Piano-Roll-Editor

A simple piano roll editor written in C++. Now available in two versions:
1. **JUCE Framework** (original) - Files in `Source/` with JUCE prefix
2. **VSTGUI Framework** (new) - Files in `Source/` with VSTGUI prefix

![alt text](img.png "prreview")

## VSTGUI Version (New)

The VSTGUI version recreates the UI of the original JUCE-based Piano Roll Editor using the VSTGUI framework.

### Features
- Complete piano roll editor with note editing capabilities
- Timeline with bar/beat markers
- Piano keyboard visualization
- Note selection, dragging, and resizing
- Quantization support
- Control panel for editor settings
- Playback marker visualization

### Building the VSTGUI Version

#### Prerequisites
1. CMake 3.15 or higher
2. C++17 compatible compiler
3. VSTGUI library (https://github.com/steinbergmedia/vstgui)
   - **Note:** This implementation is compatible with VSTGUI 4.10+ (tested with latest master branch)
   - The code uses the newer VSTGUI API with `CScrollView` constructor changes

#### Windows-Specific Prerequisites
- **Visual Studio 2019 or 2022** (Community Edition is free)
  - Download from: https://visualstudio.microsoft.com/downloads/
  - During installation, select "Desktop development with C++"
- **CMake 3.15+** - Download from https://cmake.org/download/
- **Git for Windows** - Download from https://git-scm.com/download/win

#### Build Instructions

##### Windows (Visual Studio)

```cmd
# Clone VSTGUI and this repository
cd C:\Projects
git clone https://github.com/steinbergmedia/vstgui.git
git clone https://github.com/titus-shoats/Piano-Roll-Editor.git

# Build with Visual Studio 2022
cd Piano-Roll-Editor
mkdir build
cd build
cmake .. -DVSTGUI_PATH=C:/Projects/vstgui -G "Visual Studio 17 2022"
cmake --build . --config Release

# Run
Release\PianoRollEditor_VSTGUI.exe
```

**For Visual Studio 2019**, use `-G "Visual Studio 16 2019"` instead.

**Alternative: Using Visual Studio GUI**
1. Open Visual Studio
2. Select "Open a local folder" → Navigate to `Piano-Roll-Editor`
3. Go to **Project → CMake Settings**
4. Add CMake variable: `VSTGUI_PATH` = `C:/Projects/vstgui`
5. Click "Save and generate CMake cache"
6. Build with **Build → Build All** or `Ctrl+Shift+B`

See [WINDOWS_SETUP.md](WINDOWS_SETUP.md) for detailed Windows setup guide including troubleshooting and integration examples.

##### Linux / macOS

```bash
# Clone or download VSTGUI
git clone https://github.com/steinbergmedia/vstgui.git ../vstgui

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DVSTGUI_PATH=/path/to/vstgui

# Build
cmake --build .

# Run
./PianoRollEditor_VSTGUI
```

### VSTGUI Components

The VSTGUI version consists of the following components:

- **VSTGUIPianoRollEditorView** - Main editor view (equivalent to PianoRollEditorComponent)
- **VSTGUINoteGridView** - Grid for note editing (equivalent to NoteGridComponent)
- **VSTGUIKeyboardView** - Piano keyboard display (equivalent to KeyboardComponent)
- **VSTGUITimelineView** - Timeline with bar markers (equivalent to TimelineComponent)
- **VSTGUINoteView** - Individual note representation (equivalent to PNoteComponent)
- **VSTGUIControlPanelView** - Control panel (equivalent to NoteGridControlPanel)

### VSTGUI Usage

```cpp
#include "VSTGUIPianoRollEditorView.hpp"

// Create the editor
PianoRollEditorView* editor = new PianoRollEditorView(CRect(0, 0, 800, 600));

// Setup: 10 bars, 900 pixels per bar, 20 pixels per note height
editor->setup(10, 900, 20);

// Set MIDI output callback
editor->sendChange = [](int note, int velocity) {
    // Handle MIDI output
    std::cout << "MIDI: " << note << " : " << velocity << "\n";
};

// Load/save sequences
PRESequence sequence = editor->getSequence();
editor->loadSequence(sequence);
```

## JUCE Version (Original)

### Usage

Simple add the relevant files from the **source** folder to your JUCE project. The main.cpp, mainComponent.h and mainComponent.cpp are shown as examples of how to setup and use the various piano roll editor components. 

The notes are represented using the Sequence class. Use an instance of this class to load the piano roll editor. The NoteGridComponent returns an instance of Sequence to convert the position of each note on the roll back into a Sequence of notes. You could then pass this to a MIDI sequencer or similar (depending on your application)

### Versions 

Built and tested with JUCE v5.4.1 (OSX)

## Data Model

Both versions share a common data model that is framework-independent:

- **NoteModel** - Represents a single MIDI note with pitch, velocity, start time, and length
- **PRESequence** - Collection of notes forming a musical sequence
- **NoteGridStyleSheet** - Visual styling configuration

## Differences Between JUCE and VSTGUI Versions

| Aspect | JUCE | VSTGUI |
|--------|------|--------|
| Base Class | Component | CView |
| Drawing | Graphics | CDrawContext |
| Colors | Colour | CColor |
| Mouse Events | MouseEvent callbacks | CMouseEventResult methods |
| Containers | Component hierarchy | CViewContainer |
| Scrolling | Viewport | CScrollView |

## License

See LICENSE file for details.

## Contributing

Both JUCE and VSTGUI versions are maintained. When contributing:
- Bug fixes should be applied to both versions if applicable
- New features should consider both frameworks
- Keep the data model framework-independent

## Credits

Original JUCE version by Samuel Hunt
VSTGUI port created to demonstrate cross-framework UI capabilities
