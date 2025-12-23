//
//  VSTGUIIntegrationExample.hpp
//  PianoRollEditor - VSTGUI
//
//  Example showing how to integrate the Piano Roll Editor into a VSTGUI application
//

#ifndef VSTGUIIntegrationExample_hpp
#define VSTGUIIntegrationExample_hpp

/*
 * This file demonstrates how to integrate the VSTGUI Piano Roll Editor
 * into your own VSTGUI-based application, plugin, or standalone window.
 *
 * INTEGRATION STEPS:
 *
 * 1. Include the necessary headers:
 */

#include "VSTGUIPianoRollEditorView.hpp"

/*
 * 2. In your window/frame/plugin editor, create the piano roll editor:
 */

class MyVSTGUIWindow : public VSTGUI::CFrame
{
public:
    MyVSTGUIWindow(const VSTGUI::CRect& size) : VSTGUI::CFrame(size, nullptr)
    {
        // Create the piano roll editor
        pianoRollEditor = new PianoRollEditorView(VSTGUI::CRect(0, 0, 800, 600));
        
        // Setup: bars, pixels per bar, note height
        pianoRollEditor->setup(10, 900, 20);
        
        // Show or hide control panel
        pianoRollEditor->showControlPanel(true);
        
        // Set MIDI output callback
        pianoRollEditor->sendChange = [](int note, int velocity) {
            // Send MIDI note on/off here
            // For example, trigger your synthesizer or MIDI output
        };
        
        // Set edit callback
        pianoRollEditor->onEdit = []() {
            // Called when the piano roll is edited
            // You can use this to mark your document as modified, etc.
        };
        
        // Add to your window
        addView(pianoRollEditor);
    }
    
    ~MyVSTGUIWindow()
    {
        // Cleanup is automatic with VSTGUI reference counting
    }
    
    // Example: Load a sequence from MIDI data
    void loadMIDIData(/* your MIDI data */)
    {
        PRESequence sequence;
        
        // Convert your MIDI data to PRESequence format
        // Example:
        NoteModel note1(60, 100, 0, 480, {}); // C4, velocity 100, at tick 0, length 480
        NoteModel note2(64, 100, 480, 480, {}); // E4, velocity 100, at tick 480, length 480
        NoteModel note3(67, 100, 960, 480, {}); // G4, velocity 100, at tick 960, length 480
        
        sequence.events.push_back(note1);
        sequence.events.push_back(note2);
        sequence.events.push_back(note3);
        
        sequence.tsLow = 4;
        sequence.tsHight = 4;
        
        // Load into editor
        pianoRollEditor->loadSequence(sequence);
    }
    
    // Example: Save the current sequence
    void saveMIDIData()
    {
        PRESequence sequence = pianoRollEditor->getSequence();
        
        // Convert PRESequence to your MIDI format
        for (const auto& note : sequence.events) {
            int midiNote = note.getNote();
            int velocity = note.getVelocity();
            unsigned int startTime = note.getStartTime();
            unsigned int length = note.getNoteLegnth();
            
            // Save to your MIDI file or sequencer
        }
    }
    
    // Example: Update playback position
    void updatePlaybackPosition(int ticks)
    {
        pianoRollEditor->setPlaybackMarkerPosition(ticks, true);
    }
    
private:
    PianoRollEditorView* pianoRollEditor;
};

/*
 * 3. For VST3 Plugin integration:
 */

#if 0 // Example code, not compiled

#include "public.sdk/source/vst/vstguieditor.h"

class MyVSTPluginEditor : public Steinberg::Vst::VSTGUIEditor
{
public:
    MyVSTPluginEditor(void* controller) 
        : VSTGUIEditor(controller)
    {
    }
    
    bool PLUGIN_API open(void* parent, const PlatformType& type) override
    {
        // Create the frame
        CRect size(0, 0, 800, 600);
        frame = new CFrame(size, this);
        
        // Create piano roll editor
        pianoRollEditor = new PianoRollEditorView(CRect(0, 0, 800, 600));
        pianoRollEditor->setup(10, 900, 20);
        
        // Connect to your plugin's MIDI processor
        pianoRollEditor->sendChange = [this](int note, int velocity) {
            // Send MIDI event to your plugin processor
        };
        
        frame->addView(pianoRollEditor);
        
        // Open the frame
        return frame->open(parent, type);
    }
    
private:
    PianoRollEditorView* pianoRollEditor;
};

#endif

/*
 * 4. Customization examples:
 */

void customizationExamples(PianoRollEditorView* editor)
{
    // Disable editing (view-only mode)
    editor->disableEditing(true);
    
    // Change number of bars
    editor->updateBars(20);
    
    // Scroll to specific position
    editor->setScroll(0.5, 0.3); // 50% horizontal, 30% vertical
    
    // Access control panel for additional settings
    NoteGridControlPanelView& panel = editor->getControlPanel();
    panel.setQuantisation(PRE::eQuantisationValue1_16);
    
    // Get selected notes for external editing
    auto externalEditor = editor->getSelectedNoteModels();
    for (auto* model : externalEditor.models) {
        // Modify note properties
        model->setVelocity(120);
    }
    // Call update when done
    externalEditor.update();
}

#endif /* VSTGUIIntegrationExample_hpp */
