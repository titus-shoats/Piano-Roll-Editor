//
//  VSTGUINoteGridView.hpp
//  PianoRollEditor - VSTGUI
//
//  Main note grid component for piano roll
//

#ifndef VSTGUINoteGridView_hpp
#define VSTGUINoteGridView_hpp

#include "VSTGUIConstants.h"
#include "VSTGUINoteView.hpp"
#include "VSTGUINoteGridStyleSheet.hpp"
#include <vector>
#include <functional>

class SelectionBoxView : public CView
{
public:
    SelectionBoxView(const CRect& size) : CView(size) {}
    
    void draw(CDrawContext* context) override
    {
        CColor c(255, 255, 255, 128); // Semi-transparent white
        context->setFillColor(c);
        context->drawRect(getViewSize(), kDrawFilled);
    }
    
    CCoord startX, startY;
};

class NoteGridView : public CViewContainer
{
public:
    NoteGridView(const CRect& size, NoteGridStyleSheet& styleSheet);
    virtual ~NoteGridView();
    
    void draw(CDrawContext* context) override;
    
    void noteCompSelected(NoteView* note, const CPoint& where, const CButtonState& buttons);
    void noteCompPositionMoved(NoteView* note, bool callResize = true);
    void noteCompLengthChanged(NoteView* note, int diff);
    void noteCompDragging(NoteView* note, const CPoint& where, const CButtonState& buttons);
    void setPositions();
    
    CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;
    CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override;
    CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override;
    
    void setupGrid(float pixelsPerBar, float compHeight, const int bars);
    void setQuantisation(const int val);
    
    void deleteAllSelected();
    
    PRESequence getSequence();
    void loadSequence(PRESequence sq);
    
    float getNoteCompHeight();
    float getPixelsPerBar();
    
    std::vector<NoteModel*> getSelectedModels();
    
    std::function<void(int note, int velocity)> sendChange;
    std::function<void()> onEdit;
    
private:
    void sendEdit();
    
    NoteGridStyleSheet& styleSheet;
    SelectionBoxView* selectorBox;
    std::vector<NoteView*> noteComps;
    
    std::vector<int> blackPitches;
    
    float noteCompHeight;
    float pixelsPerBar;
    st_int ticksPerTimeSignature;
    st_int currentQValue;
    st_int lastNoteLength;
    bool firstDrag;
    bool firstCall;
    int lastTrigger;
    
    bool isSelecting;
    CPoint selectionStart;
};

#endif /* VSTGUINoteGridView_hpp */
