//
//  VSTGUIKeyboardView.hpp
//  PianoRollEditor - VSTGUI
//
//  Keyboard component showing MIDI notes
//

#ifndef VSTGUIKeyboardView_hpp
#define VSTGUIKeyboardView_hpp

#include "VSTGUIConstants.h"
#include <vector>

class KeyboardView : public CView
{
public:
    KeyboardView(const CRect& size);
    
    void draw(CDrawContext* context) override;
    
private:
    std::vector<int> blackPitches;
};

#endif /* VSTGUIKeyboardView_hpp */
