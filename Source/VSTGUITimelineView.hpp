//
//  VSTGUITimelineView.hpp
//  PianoRollEditor - VSTGUI
//
//  Timeline component for bar/beat markers
//

#ifndef VSTGUITimelineView_hpp
#define VSTGUITimelineView_hpp

#include "VSTGUIConstants.h"

class TimelineView : public CView
{
public:
    TimelineView(const CRect& size);
    
    void setup(const int barsToDraw, const int pixelsPerBar);
    void draw(CDrawContext* context) override;
    
private:
    int barsToDraw;
    int pixelsPerBar;
};

#endif /* VSTGUITimelineView_hpp */
