//
//  VSTGUIMain.cpp
//  PianoRollEditor - VSTGUI
//
//  Main application entry point
//

#include "vstgui/vstgui.h"
#include "vstgui/lib/platform/platform_x11.h"
#include "VSTGUIPianoRollEditorView.hpp"
#include <iostream>

using namespace VSTGUI;

class PianoRollEditorWindow : public CFrame
{
public:
    PianoRollEditorWindow(const CRect& size) : CFrame(size, nullptr)
    {
        setBackgroundColor(CColor(32, 32, 32, 255));
        
        // Create the piano roll editor
        editor = new PianoRollEditorView(CRect(0, 0, size.getWidth(), size.getHeight()));
        addView(editor);
        
        editor->setup(10, 900, 20); // 10 bars, 900 pixels per bar, 20 pixels per note
        editor->showControlPanel(true);
        
        editor->sendChange = [](int note, int velocity) {
            std::cout << "MIDI send: " << note << " : " << velocity << "\n";
        };
        
        // Start animation timer for playback marker
        tickTest = 0;
        startTimer();
    }
    
    virtual ~PianoRollEditorWindow()
    {
        stopTimer();
    }
    
    void startTimer()
    {
        timerRunning = true;
        timerThread = std::thread([this]() {
            while (timerRunning) {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                
                tickTest += 20;
                editor->setPlaybackMarkerPosition(tickTest);
                
                if (tickTest >= 480 * 4 * 10) {
                    tickTest = 0;
                }
                
                invalid();
            }
        });
    }
    
    void stopTimer()
    {
        timerRunning = false;
        if (timerThread.joinable()) {
            timerThread.join();
        }
    }
    
private:
    PianoRollEditorView* editor;
    st_int tickTest;
    bool timerRunning;
    std::thread timerThread;
};

int main(int argc, char* argv[])
{
    // Initialize VSTGUI platform
    IPlatformFrameConfig* config = nullptr;
    
#if WINDOWS
    config = new Win32FrameConfig();
#elif MAC
    config = new MacPlatformConfig();
#else
    // Linux/X11
    config = new X11FrameConfig();
#endif
    
    // Create window
    CRect windowSize(0, 0, 800, 600);
    PianoRollEditorWindow* frame = new PianoRollEditorWindow(windowSize);
    
    if (frame->open(nullptr, PlatformType::kDefaultNative, config)) {
        std::cout << "Piano Roll Editor - VSTGUI Version\n";
        std::cout << "Window opened successfully\n";
        
        // Simple event loop (in real app, this would be handled by platform)
        // For demonstration, we'll just keep the window alive
        std::cout << "Press Enter to exit...\n";
        std::cin.get();
        
        frame->close();
    }
    else {
        std::cerr << "Failed to open window\n";
    }
    
    frame->forget();
    
    return 0;
}
