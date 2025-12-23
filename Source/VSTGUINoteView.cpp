//
//  VSTGUINoteView.cpp
//  PianoRollEditor - VSTGUI
//

#include "VSTGUINoteView.hpp"
#include <sstream>

NoteView::NoteView(const CRect& size, NoteGridStyleSheet& ss) 
    : CView(size), styleSheet(ss)
{
    mouseOver = false;
    useCustomColour = false;
    resizeEnabled = true;
    isDragging = false;
    isResizing = false;
    state = eNone;
    coordinatesDiffer = false;
    isMultiDrag = false;
    startWidth = 0;
    startX = 0;
    startY = 0;
    
    customColour = CColor(100, 150, 200, 255);
}

void NoteView::draw(CDrawContext* context)
{
    CRect rect = getViewSize();
    
    // Determine color based on state
    CColor fillColor;
    if (useCustomColour) {
        fillColor = customColour;
    } else if (state == eSelected) {
        fillColor = CColor(100, 150, 255, 255); // Blue for selected
    } else if (mouseOver) {
        fillColor = CColor(150, 200, 255, 200); // Light blue for hover
    } else {
        fillColor = CColor(80, 120, 180, 220); // Default note color
    }
    
    context->setFillColor(fillColor);
    context->drawRect(rect, kDrawFilled);
    
    // Draw border
    context->setFrameColor(state == eSelected ? PRE::Colors::White : PRE::Colors::Black);
    context->setLineWidth(state == eSelected ? 2.0 : 1.0);
    context->drawRect(rect, kDrawStroked);
    
    // Draw velocity/note info if enabled
    if (styleSheet.getDrawVelocity() || styleSheet.getDrawMIDINum() || 
        styleSheet.getDrawMIDINoteStr()) {
        
        context->setFontColor(PRE::Colors::White);
        context->setFont(kNormalFontSmall);
        
        std::stringstream ss;
        
        if (styleSheet.getDrawMIDINum()) {
            ss << (int)model.getNote();
        }
        if (styleSheet.getDrawMIDINoteStr()) {
            if (ss.str().length() > 0) ss << " ";
            ss << PRE::pitches_names[model.getNote() % 12];
        }
        if (styleSheet.getDrawVelocity()) {
            if (ss.str().length() > 0) ss << " ";
            ss << "v" << (int)model.getVelocity();
        }
        
        if (ss.str().length() > 0) {
            CRect textRect = rect;
            textRect.inset(2, 2);
            context->drawString(ss.str().c_str(), textRect, kLeftText);
        }
    }
}

void NoteView::setCustomColour(const CColor& c)
{
    customColour = c;
    useCustomColour = true;
}

void NoteView::setValues(NoteModel m)
{
    model = m;
}

NoteModel NoteView::getModel()
{
    return model;
}

NoteModel* NoteView::getModelPtr()
{
    return &model;
}

void NoteView::setState(eState s)
{
    state = s;
    invalid();
}

NoteView::eState NoteView::getState()
{
    return state;
}

CMouseEventResult NoteView::onMouseDown(CPoint& where, const CButtonState& buttons)
{
    if (styleSheet.disableEditing) {
        return kMouseEventNotHandled;
    }
    
    lastMousePos = where;
    CRect rect = getViewSize();
    
    // Check if near right edge for resizing
    if (resizeEnabled && where.x > rect.right - 10) {
        isResizing = true;
        startWidth = rect.getWidth();
    } else {
        isDragging = true;
        startX = rect.left;
        startY = rect.top;
    }
    
    if (onNoteSelect) {
        onNoteSelect(this, where, buttons);
    }
    
    return kMouseEventHandled;
}

CMouseEventResult NoteView::onMouseUp(CPoint& where, const CButtonState& buttons)
{
    if (isDragging && onPositionMoved) {
        onPositionMoved(this);
    }
    
    isDragging = false;
    isResizing = false;
    
    return kMouseEventHandled;
}

CMouseEventResult NoteView::onMouseMoved(CPoint& where, const CButtonState& buttons)
{
    if (styleSheet.disableEditing) {
        return kMouseEventNotHandled;
    }
    
    if (buttons.isLeftButton()) {
        CRect rect = getViewSize();
        CCoord dx = where.x - lastMousePos.x;
        CCoord dy = where.y - lastMousePos.y;
        
        if (isResizing) {
            // Resize width
            CCoord newWidth = rect.getWidth() + dx;
            if (newWidth >= minWidth) {
                rect.right = rect.left + newWidth;
                setViewSize(rect);
                
                if (onLengthChange) {
                    onLengthChange(this, (int)dx);
                }
            }
        } else if (isDragging) {
            // Move position
            rect.offset(dx, dy);
            setViewSize(rect);
            coordinatesDiffer = true;
            
            if (onDragging) {
                onDragging(this, where, buttons);
            }
        }
        
        lastMousePos = where;
        invalid();
        
        return kMouseEventHandled;
    }
    
    return kMouseEventNotHandled;
}

CMouseEventResult NoteView::onMouseEntered(CPoint& where, const CButtonState& buttons)
{
    mouseOver = true;
    invalid();
    return kMouseEventHandled;
}

CMouseEventResult NoteView::onMouseExited(CPoint& where, const CButtonState& buttons)
{
    mouseOver = false;
    invalid();
    return kMouseEventHandled;
}
