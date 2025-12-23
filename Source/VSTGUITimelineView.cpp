//
//  VSTGUITimelineView.cpp
//  PianoRollEditor - VSTGUI
//

#include "VSTGUITimelineView.hpp"
#include <sstream>

TimelineView::TimelineView(const CRect& size) : CView(size)
{
    barsToDraw = 0;
    pixelsPerBar = 0;
}

void TimelineView::setup(const int bd, const int px)
{
    barsToDraw = bd;
    pixelsPerBar = px;
    
    CRect newSize = getViewSize();
    newSize.setWidth(pixelsPerBar * barsToDraw);
    setViewSize(newSize);
}

void TimelineView::draw(CDrawContext* context)
{
    context->setFillColor(PRE::Colors::DarkGrey);
    context->drawRect(getViewSize(), kDrawFilled);
    
    if (pixelsPerBar == 0 || barsToDraw == 0) {
        return;
    }
    
    const int marks = barsToDraw * 4; // assume 4/4
    const CCoord increment = getWidth() / (CCoord)(marks);
    CCoord yPos = 0;
    
    context->setFrameColor(PRE::Colors::White);
    context->setFontColor(PRE::Colors::White);
    context->setFont(kNormalFont);
    
    for (int i = 0; i < marks; i++) {
        if (i % 4 == 0) {
            // Draw bar number
            std::stringstream ss;
            ss << (i/4 + 1);
            std::string txt = ss.str();
            
            CRect textRect(yPos + 5, 3, yPos + 35, 23);
            context->drawString(txt.c_str(), textRect);
            context->drawLine(CPoint(yPos, 0), CPoint(yPos, getHeight()));
        }
        else if (i % 2 == 0) {
            context->drawLine(CPoint(yPos, getHeight() * 0.66), 
                            CPoint(yPos, getHeight()));
        }
        else {
            context->drawLine(CPoint(yPos, getHeight() * 0.33), 
                            CPoint(yPos, getHeight()));
        }
        
        yPos += increment;
    }
}
