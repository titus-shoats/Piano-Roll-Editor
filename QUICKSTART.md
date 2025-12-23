# Quick Start Guide - VSTGUI Piano Roll Editor

This guide helps you quickly get started with the VSTGUI version of the Piano Roll Editor.

## What You Need

1. **VSTGUI Library** - https://github.com/steinbergmedia/vstgui
2. **C++17 Compiler** - GCC, Clang, or MSVC
3. **CMake 3.15+** - Build system

## 5-Minute Setup

### Step 1: Get VSTGUI

```bash
# Clone VSTGUI next to this project
cd ..
git clone https://github.com/steinbergmedia/vstgui.git
cd Piano-Roll-Editor
```

### Step 2: Build

```bash
# Create build directory
mkdir build
cd build

# Configure (replace path if VSTGUI is elsewhere)
cmake .. -DVSTGUI_PATH=../../vstgui

# Build
cmake --build .
```

### Step 3: Use in Your Project

See the integration example below or check `Source/VSTGUIIntegrationExample.hpp` for detailed examples.

## Basic Usage

### Creating the Editor

```cpp
#include "VSTGUIPianoRollEditorView.hpp"

// In your window/frame initialization:
auto* editor = new PianoRollEditorView(CRect(0, 0, 800, 600));

// Setup: 10 bars, 900 pixels per bar, 20 pixels per note
editor->setup(10, 900, 20);

// Add to your window
yourWindow->addView(editor);
```

### Handling MIDI Output

```cpp
editor->sendChange = [](int note, int velocity) {
    std::cout << "MIDI Note: " << note 
              << " Velocity: " << velocity << "\n";
    // Send to your MIDI output here
};
```

### Loading/Saving Sequences

```cpp
// Create a sequence
PRESequence sequence;

// Add notes
NoteModel note1(60, 100, 0, 480, {});      // C4
NoteModel note2(64, 100, 480, 480, {});    // E4
sequence.events.push_back(note1);
sequence.events.push_back(note2);

// Load into editor
editor->loadSequence(sequence);

// Later, get edited sequence
PRESequence edited = editor->getSequence();
```

## Integration Examples

### VST3 Plugin

```cpp
#include "public.sdk/source/vst/vstguieditor.h"
#include "VSTGUIPianoRollEditorView.hpp"

class MyPluginEditor : public Steinberg::Vst::VSTGUIEditor {
public:
    bool PLUGIN_API open(void* parent, const PlatformType& type) override {
        CRect size(0, 0, 800, 600);
        frame = new CFrame(size, this);
        
        auto* editor = new PianoRollEditorView(size);
        editor->setup(10, 900, 20);
        
        frame->addView(editor);
        return frame->open(parent, type);
    }
};
```

### Standalone Application

```cpp
#include "vstgui/vstgui.h"
#include "VSTGUIPianoRollEditorView.hpp"

int main() {
    // Create window (platform-specific)
    CRect windowSize(0, 0, 800, 600);
    auto* frame = new CFrame(windowSize, nullptr);
    
    // Add piano roll editor
    auto* editor = new PianoRollEditorView(windowSize);
    editor->setup(10, 900, 20);
    frame->addView(editor);
    
    // Open window (platform-specific)
    frame->open(nullptr);
    
    // Event loop here...
    
    frame->forget();
    return 0;
}
```

## Common Tasks

### Change Grid Size

```cpp
// Resize to 20 bars
editor->updateBars(20);
```

### Set Quantization

```cpp
editor->getControlPanel().setQuantisation(PRE::eQuantisationValue1_16);
```

### Disable Editing (View Only)

```cpp
editor->disableEditing(true);
```

### Update Playback Position

```cpp
int currentTick = 480; // One quarter note
editor->setPlaybackMarkerPosition(currentTick, true);
```

### Get Selected Notes

```cpp
auto selected = editor->getSelectedNoteModels();
for (auto* note : selected.models) {
    std::cout << "Note: " << (int)note->getNote() << "\n";
}
// Call update after modifying
selected.update();
```

## Understanding the Data Model

### Timing

- **Resolution:** 480 ticks per quarter note (MIDI standard)
- **Bar Length:** 1920 ticks (4/4 time signature)
- **Note Position:** Measured in ticks from start

### NoteModel Fields

```cpp
NoteModel note(
    60,      // MIDI note number (0-127)
    100,     // Velocity (0-127)
    0,       // Start time (ticks)
    480,     // Length (ticks)
    {}       // Flags (optional)
);
```

### Quantization Values

- `eQuantisationValueNone` - No quantization
- `eQuantisationValue1_32` - 1/32 note
- `eQuantisationValue1_16` - 1/16 note
- `eQuantisationValue1_8` - 1/8 note
- `eQuantisationValue1_4` - 1/4 note

## Troubleshooting

### "VSTGUI not found"

Make sure VSTGUI_PATH points to your VSTGUI installation:

```bash
cmake .. -DVSTGUI_PATH=/correct/path/to/vstgui
```

### Linking Errors

Ensure platform libraries are installed:
- **Linux:** `libx11-dev`, `libfreetype6-dev`
- **macOS:** Xcode command line tools
- **Windows:** Visual Studio with C++ support

### Visual Issues

Check that you're calling:
```cpp
editor->setup(bars, pixelsPerBar, noteHeight);
```
before displaying the editor.

## Next Steps

1. **Read the Migration Guide** - `MIGRATION_GUIDE.md` for JUCE comparison
2. **Check Integration Examples** - `Source/VSTGUIIntegrationExample.hpp`
3. **Review Implementation** - `IMPLEMENTATION_SUMMARY.md`
4. **Full Documentation** - `README_VSTGUI.md`

## Support

For issues or questions:
1. Check the documentation files
2. Review the example code
3. Compare with JUCE version for reference
4. Consult VSTGUI documentation at https://steinbergmedia.github.io/vst3_doc/vstgui/html/

## License

Same license as the original Piano Roll Editor project.
