//
//  VSTGUIPianoRollEditorView.hpp
//  PianoRollEditor - VSTGUI
//
//  Main piano roll editor view
//

#ifndef VSTGUIPianoRollEditorView_hpp
#define VSTGUIPianoRollEditorView_hpp

#include "VSTGUIConstants.h"
#include "VSTGUINoteGridView.hpp"
#include "VSTGUITimelineView.hpp"
#include "VSTGUIKeyboardView.hpp"
#include "VSTGUIControlPanelView.hpp"
#include "VSTGUINoteGridStyleSheet.hpp"
#include <functional>

// Custom scroll view that notifies position changes
class CustomScrollView : public CScrollView
{
public:
    CustomScrollView(const CRect& size, const CRect& containerSize) 
        : CScrollView(size, containerSize, CScrollView::kHorizontalScrollbar | CScrollView::kVerticalScrollbar)
    {
    }
    
    void valueChanged(CControl* pControl) override
    {
        CScrollView::valueChanged(pControl);
        
        if (positionMoved) {
            CPoint offset = getScrollOffset();
            positionMoved(offset.x, offset.y);
        }
    }
    
    std::function<void(CCoord, CCoord)> positionMoved;
};

class PianoRollEditorView : public CViewContainer
{
public:
    struct ExternalModelEditor {
        std::vector<NoteModel*> models;
        std::function<void()> update;
    };
    
    PianoRollEditorView(const CRect& size);
    virtual ~PianoRollEditorView();
    
    void setup(const int bars, const int pixelsPerBar, const int noteHeight);
    void updateBars(const int newNumberOfBars);
    
    void draw(CDrawContext* context) override;
    void drawRect(CDrawContext* context, const CRect& updateRect) override;
    
    void showControlPanel(bool state);
    
    void loadSequence(PRESequence sequence);
    PRESequence getSequence();
    
    void setScroll(double x, double y);
    void setPlaybackMarkerPosition(const st_int ticks, bool isVisible = true);
    
    void disableEditing(bool value);
    NoteGridControlPanelView& getControlPanel();
    
    ExternalModelEditor getSelectedNoteModels();
    
    std::function<void()> onEdit;
    std::function<void(int note, int velocity)> sendChange;
    
private:
    NoteGridStyleSheet styleSheet;
    
    NoteGridView* noteGrid;
    TimelineView* timelineComp;
    KeyboardView* keyboardComp;
    
    CustomScrollView* viewportGrid;
    CScrollView* viewportPiano;
    CScrollView* viewportTimeline;
    
    NoteGridControlPanelView* controlPanel;
    
    st_int playbackTicks;
    bool showPlaybackMarker;
    
    int currentBars;
    int currentPixelsPerBar;
    int currentNoteHeight;
};

#endif /* VSTGUIPianoRollEditorView_hpp */
