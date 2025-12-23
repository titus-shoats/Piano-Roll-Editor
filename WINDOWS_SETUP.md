# Windows Setup Guide - VSTGUI Piano Roll Editor

Complete step-by-step guide for setting up the VSTGUI Piano Roll Editor on Windows.

## Prerequisites

### Required Software

1. **Visual Studio 2019 or 2022** (Community Edition is free)
   - Download from: https://visualstudio.microsoft.com/downloads/
   - During installation, select "Desktop development with C++"
   - Make sure to include: C++ CMake tools, C++ Clang tools

2. **CMake 3.15 or newer**
   - Download from: https://cmake.org/download/
   - During installation, select "Add CMake to system PATH"
   - Or use: `winget install Kitware.CMake`

3. **Git for Windows**
   - Download from: https://git-scm.com/download/win
   - Or use: `winget install Git.Git`

## Step-by-Step Setup

### Method 1: Using Visual Studio (Recommended for Beginners)

#### Step 1: Get VSTGUI

Open Git Bash or Command Prompt:

```cmd
cd C:\Projects
git clone https://github.com/steinbergmedia/vstgui.git
git clone https://github.com/titus-shoats/Piano-Roll-Editor.git
```

#### Step 2: Open in Visual Studio

1. Open Visual Studio
2. Select "Open a local folder"
3. Navigate to `C:\Projects\Piano-Roll-Editor`
4. Visual Studio will automatically detect CMakeLists.txt

#### Step 3: Configure CMake

1. In Visual Studio, go to **Project → CMake Settings**
2. Add a CMake variable:
   - Name: `VSTGUI_PATH`
   - Value: `C:/Projects/vstgui` (use forward slashes)
3. Click "Save and generate CMake cache"

#### Step 4: Build

1. In Visual Studio, select **Build → Build All**
2. Or press `Ctrl+Shift+B`

The executable will be in: `out/build/x64-Debug/PianoRollEditor_VSTGUI.exe`

### Method 2: Using Command Line (For Advanced Users)

#### Step 1: Get VSTGUI

```cmd
cd C:\Projects
git clone https://github.com/steinbergmedia/vstgui.git
git clone https://github.com/titus-shoats/Piano-Roll-Editor.git
cd Piano-Roll-Editor
```

#### Step 2: Configure with CMake

```cmd
mkdir build
cd build
cmake .. -DVSTGUI_PATH=C:/Projects/vstgui -G "Visual Studio 17 2022"
```

For Visual Studio 2019, use:
```cmd
cmake .. -DVSTGUI_PATH=C:/Projects/vstgui -G "Visual Studio 16 2019"
```

#### Step 3: Build

```cmd
cmake --build . --config Release
```

Or for Debug build:
```cmd
cmake --build . --config Debug
```

The executable will be in: `build/Release/PianoRollEditor_VSTGUI.exe`

### Method 3: Using CMake GUI (Visual Interface)

1. Open CMake GUI
2. Set "Where is the source code" to: `C:/Projects/Piano-Roll-Editor`
3. Set "Where to build the binaries" to: `C:/Projects/Piano-Roll-Editor/build`
4. Click "Configure"
5. Select your Visual Studio version
6. Add entry: `VSTGUI_PATH` = `C:/Projects/vstgui`
7. Click "Configure" again
8. Click "Generate"
9. Click "Open Project" to open in Visual Studio
10. Build using Visual Studio

## Integration into Your Project

### VST3 Plugin (Windows)

```cpp
// In your VST3 plugin editor class
#include "VSTGUIPianoRollEditorView.hpp"

class MyPluginEditor : public Steinberg::Vst::VSTGUIEditor {
public:
    bool PLUGIN_API open(void* parent, const PlatformType& platformType) override {
        CRect size(0, 0, 800, 600);
        frame = new CFrame(size, this);
        
        auto* editor = new PianoRollEditorView(size);
        editor->setup(10, 900, 20);
        
        editor->sendChange = [this](int note, int velocity) {
            // Send MIDI to your processor
        };
        
        frame->addView(editor);
        return frame->open(parent, platformType);
    }
};
```

### Standalone Application (Windows)

```cpp
#include "vstgui/vstgui.h"
#include "vstgui/lib/platform/win32/win32frame.h"
#include "VSTGUIPianoRollEditorView.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    // Initialize COM
    CoInitialize(nullptr);
    
    // Create window
    CRect windowSize(0, 0, 800, 600);
    auto* frame = new CFrame(windowSize, nullptr);
    
    // Add piano roll editor
    auto* editor = new PianoRollEditorView(windowSize);
    editor->setup(10, 900, 20);
    frame->addView(editor);
    
    // Open window
    Win32Frame::Config config;
    config.hInstance = hInstance;
    frame->open(nullptr, PlatformType::kHWND, &config);
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    frame->forget();
    CoUninitialize();
    return 0;
}
```

## Common Windows Issues & Solutions

### Issue: "VSTGUI not found"

**Solution:**
Make sure the path uses forward slashes:
```cmd
cmake .. -DVSTGUI_PATH=C:/Projects/vstgui
```

### Issue: "CMake is not recognized"

**Solution:**
Add CMake to your PATH:
1. Open System Properties → Advanced → Environment Variables
2. Edit "Path" variable
3. Add: `C:\Program Files\CMake\bin`
4. Restart Command Prompt

### Issue: "No generator found"

**Solution:**
Specify the generator explicitly:
```cmd
cmake .. -G "Visual Studio 17 2022" -DVSTGUI_PATH=C:/Projects/vstgui
```

### Issue: Link errors (unresolved externals)

**Solution:**
Ensure you have Windows SDK installed via Visual Studio Installer:
1. Open Visual Studio Installer
2. Modify your installation
3. Check "Windows 10 SDK" or "Windows 11 SDK"

### Issue: "Cannot open include file 'windows.h'"

**Solution:**
Install Windows SDK through Visual Studio Installer.

## Directory Structure

After setup, your directories should look like:

```
C:\Projects\
├── vstgui\
│   ├── vstgui\
│   │   ├── vstgui.h
│   │   └── ...
│   └── ...
└── Piano-Roll-Editor\
    ├── Source\
    │   ├── VSTGUI*.cpp
    │   ├── VSTGUI*.hpp
    │   └── ...
    ├── CMakeLists.txt
    └── build\
        ├── Release\
        │   └── PianoRollEditor_VSTGUI.exe
        └── Debug\
            └── PianoRollEditor_VSTGUI.exe
```

## Building for Release

For optimized release build:

```cmd
cd build
cmake --build . --config Release
```

This creates an optimized executable in `build/Release/`

## Next Steps

1. **Test the build:** Run `PianoRollEditor_VSTGUI.exe`
2. **Integrate into your project:** See integration examples above
3. **Read documentation:**
   - `QUICKSTART.md` - Basic usage
   - `VSTGUIIntegrationExample.hpp` - Code examples
   - `MIGRATION_GUIDE.md` - Framework details

## Additional Resources

- **VSTGUI Documentation:** https://steinbergmedia.github.io/vst3_doc/vstgui/html/
- **VST3 SDK:** https://github.com/steinbergmedia/vst3sdk
- **CMake Documentation:** https://cmake.org/documentation/
- **Visual Studio C++ Docs:** https://docs.microsoft.com/en-us/cpp/

## Support

If you encounter issues:
1. Check this guide's troubleshooting section
2. Review the documentation files
3. Verify all prerequisites are installed
4. Check VSTGUI and CMake versions

## Quick Command Reference

```cmd
# Clone repositories
git clone https://github.com/steinbergmedia/vstgui.git
git clone https://github.com/titus-shoats/Piano-Roll-Editor.git

# Configure and build (Visual Studio 2022)
cd Piano-Roll-Editor
mkdir build && cd build
cmake .. -DVSTGUI_PATH=C:/Projects/vstgui -G "Visual Studio 17 2022"
cmake --build . --config Release

# Run
Release\PianoRollEditor_VSTGUI.exe
```
