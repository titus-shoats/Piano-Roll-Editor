//
//  VSTGUINoteView.hpp
//  PianoRollEditor - VSTGUI
//
//  Individual note view component
//

#ifndef VSTGUINoteView_hpp
#define VSTGUINoteView_hpp

#include "VSTGUIConstants.h"
#include "VSTGUINoteModel.hpp"
#include "VSTGUINoteGridStyleSheet.hpp"
#include <functional>

class NoteView : public CView
{
public:
    enum eState {
        eNone,
        eSelected,
    };
    
    NoteView(const CRect& size, NoteGridStyleSheet& styleSheet);
    
    void draw(CDrawContext* context) override;
    void setCustomColour(const CColor& c);
    
    void setValues(NoteModel model);
    NoteModel getModel();
    NoteModel* getModelPtr();
    
    void setState(eState state);
    eState getState();
    
    CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;
    CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override;
    CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override;
    CMouseEventResult onMouseEntered(CPoint& where, const CButtonState& buttons) override;
    CMouseEventResult onMouseExited(CPoint& where, const CButtonState& buttons) override;
    
    std::function<void(NoteView*, const CPoint&, const CButtonState&)> onNoteSelect;
    std::function<void(NoteView*)> onPositionMoved;
    std::function<void(NoteView*, const CPoint&, const CButtonState&)> onDragging;
    std::function<void(NoteView*, int)> onLengthChange;
    
    int minWidth = 10;
    int startWidth;
    CCoord startX, startY;
    bool coordinatesDiffer;
    bool isMultiDrag;
    
private:
    NoteGridStyleSheet& styleSheet;
    bool mouseOver;
    bool useCustomColour;
    bool resizeEnabled;
    bool isDragging;
    bool isResizing;
    CPoint lastMousePos;
    
    CColor customColour;
    NoteModel model;
    eState state;
};

#endif /* VSTGUINoteView_hpp */
