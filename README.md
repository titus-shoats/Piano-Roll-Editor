# Piano-Roll-Editor

A simple piano roll editor written in C++. Now available in two framework implementations:

1. **JUCE Framework** (original) - Production-ready implementation
2. **VSTGUI Framework** (new) - Recreated UI using VSTGUI

![alt text](img.png "prreview")

## Framework Versions

### JUCE Version (Original)

The original implementation using the JUCE framework. All source files without the `VSTGUI` prefix.

**Files:** `PianoRollEditorComponent.*`, `NoteGridComponent.*`, `KeyboardComponent.*`, etc.

**Usage:** See original usage instructions below.

### VSTGUI Version (New)

A complete recreation of the Piano Roll Editor UI using the VSTGUI framework. All source files with the `VSTGUI` prefix.

**Files:** `VSTGUIPianoRollEditorView.*`, `VSTGUINoteGridView.*`, `VSTGUIKeyboardView.*`, etc.

**Documentation:** 
- See [README_VSTGUI.md](README_VSTGUI.md) for VSTGUI-specific details
- See [MIGRATION_GUIDE.md](MIGRATION_GUIDE.md) for comparison between frameworks
- See [Source/VSTGUIIntegrationExample.hpp](Source/VSTGUIIntegrationExample.hpp) for integration examples

**Build:** Uses CMake - see [CMakeLists.txt](CMakeLists.txt)

## Features (Both Versions)

- Piano roll grid for note editing
- Visual piano keyboard with MIDI note numbers
- Timeline with bar and beat markers
- Mouse-based note creation (double-click)
- Note selection, dragging, and resizing
- Multi-note selection and editing
- Quantization support (1/32, 1/16, 1/8, 1/4)
- Playback position marker
- MIDI note triggering during editing
- Control panel for settings
- Framework-independent data model

## JUCE Version Usage

Simple add the relevant files from the **source** folder to your JUCE project. The main.cpp, mainComponent.h and mainComponent.cpp are shown as examples of how to setup and use the various piano roll editor components. 

The notes are represented using the Sequence class. Use an instance of this class to load the piano roll editor. The NoteGridComponent returns an instance of Sequence to convert the position of each note on the roll back into a Sequence of notes. You could then pass this to a MIDI sequencer or similar (depending on your application)

## Framework-Independent Data Model

Both JUCE and VSTGUI versions share the same data model:

- **NoteModel** - Represents a single MIDI note (pitch, velocity, start time, length)
- **PRESequence** - Collection of notes forming a musical sequence
- **NoteGridStyleSheet** - Visual styling configuration

This design allows you to:
- Switch between frameworks without changing your data
- Use the same MIDI import/export code for both versions
- Mix and match components if needed

## Choosing a Framework

**Use JUCE if:**
- You're already using JUCE in your project
- You need the complete JUCE ecosystem (audio, plugins, etc.)
- You want a mature, production-tested implementation

**Use VSTGUI if:**
- You're building VST3 plugins with the VST SDK
- You prefer a lighter-weight UI framework
- You want to learn cross-framework UI development

## Building

### JUCE Version
Add the JUCE source files to your JUCE project (see original instructions above)

### VSTGUI Version
```bash
mkdir build && cd build
cmake .. -DVSTGUI_PATH=/path/to/vstgui
cmake --build .
```

See [README_VSTGUI.md](README_VSTGUI.md) for detailed VSTGUI build instructions.

# Versions 

Built and tested with JUCE v5.4.1 (OSX)
