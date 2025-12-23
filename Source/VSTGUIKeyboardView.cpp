//
//  VSTGUIKeyboardView.cpp
//  PianoRollEditor - VSTGUI
//

#include "VSTGUIKeyboardView.hpp"
#include <sstream>

KeyboardView::KeyboardView(const CRect& size) : CView(size)
{
    blackPitches.push_back(1);
    blackPitches.push_back(3);
    blackPitches.push_back(6);
    blackPitches.push_back(8);
    blackPitches.push_back(10);
}

void KeyboardView::draw(CDrawContext* context)
{
    const CCoord noteCompHeight = getHeight() / 128.0;
    CCoord line = 0;
    
    for (int i = 127; i >= 0; i--) {
        const int pitch = i % 12;
        
        bool isBlack = false;
        for (int bp : blackPitches) {
            if (bp == pitch) {
                isBlack = true;
                break;
            }
        }
        
        context->setFillColor(isBlack ? PRE::Colors::DarkGrey : 
                             CColor(96, 96, 96, 255)); // darker light grey
        
        CRect noteRect(0, line, getWidth(), line + noteCompHeight);
        context->drawRect(noteRect, kDrawFilled);
        
        // Draw MIDI note number
        context->setFontColor(PRE::Colors::White);
        context->setFont(kNormalFont);
        
        std::stringstream ss;
        ss << i;
        std::string noteStr = ss.str();
        
        CRect textRect(5, line, 40, line + noteCompHeight);
        context->drawString(noteStr.c_str(), textRect);
        
        line += noteCompHeight;
        
        // Draw horizontal line
        context->setFrameColor(PRE::Colors::Black);
        context->drawLine(CPoint(0, line), CPoint(getWidth(), line));
    }
}
