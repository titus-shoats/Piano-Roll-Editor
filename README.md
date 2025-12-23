# Piano-Roll-Editor
A simple piano roll editor written in C++, available in two versions:
- **JUCE Framework Version** (original) - Cross-platform using JUCE graphics
- **Win32 API Version** (new) - Native Windows implementation for Visual Studio

![alt text](img.png "prreview")

## JUCE Version

### Usage

Simple add the relevant files from the **Source** folder to your JUCE project. The main.cpp, mainComponent.h and mainComponent.cpp are shown as examples of how to setup and use the various piano roll editor components. 

The notes are represented using the Sequence class. Use an instance of this class to load the piano roll editor. The NoteGridComponent returns an instance of Sequence to convert the position of each note on the roll back into a Sequence of notes. You could then pass this to a MIDI sequencer or similar (depending on your application)

### Version Info

Built and tested with JUCE v5.4.1 (OSX)

## Win32 Version

A complete native Windows implementation using Win32 API, designed to run in Visual Studio 2022.

### Features
- Pure Win32 API implementation (no JUCE dependency)
- Visual Studio 2022 solution and project files
- Same piano roll functionality as JUCE version
- Native Windows look and feel
- Direct GDI rendering
- Smaller executable size

### Usage

See the [Win32Version/README.md](Win32Version/README.md) file for detailed documentation on building and using the Win32 version.

Quick start:
1. Open `Win32Version/PianoRollEditor.sln` in Visual Studio 2022
2. Build and run (F5)

### Requirements
- Windows 10 or later
- Visual Studio 2022 (or later)
- Windows 10 SDK
- C++17 or later
