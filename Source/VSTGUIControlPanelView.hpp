//
//  VSTGUIControlPanelView.hpp
//  PianoRollEditor - VSTGUI
//
//  Control panel for piano roll editor
//

#ifndef VSTGUIControlPanelView_hpp
#define VSTGUIControlPanelView_hpp

#include "VSTGUIConstants.h"
#include "VSTGUINoteGridView.hpp"
#include "VSTGUINoteGridStyleSheet.hpp"
#include <functional>

class NoteGridControlPanelView : public CView
{
public:
    NoteGridControlPanelView(const CRect& size, NoteGridView& noteGrid, 
                             NoteGridStyleSheet& styleSheet);
    virtual ~NoteGridControlPanelView();
    
    void draw(CDrawContext* context) override;
    
    void setQuantisation(PRE::eQuantisationValue value);
    
    std::function<void(int pixelsPerBar, int noteHeight)> configureGrid;
    
private:
    NoteGridView& noteGrid;
    NoteGridStyleSheet& styleSheet;
};

#endif /* VSTGUIControlPanelView_hpp */
