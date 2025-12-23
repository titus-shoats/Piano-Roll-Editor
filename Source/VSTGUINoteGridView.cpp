//
//  VSTGUINoteGridView.cpp
//  PianoRollEditor - VSTGUI
//

#include "VSTGUINoteGridView.hpp"

#define RETURN_IF_EDITING_DISABLED if(styleSheet.disableEditing) { return; }

NoteGridView::NoteGridView(const CRect& size, NoteGridStyleSheet& ss) 
    : CView(size), styleSheet(ss)
{
    blackPitches.push_back(1);
    blackPitches.push_back(3);
    blackPitches.push_back(6);
    blackPitches.push_back(8);
    blackPitches.push_back(10);
    
    selectorBox = new SelectionBoxView(CRect(0, 0, 1, 1));
    addView(selectorBox);
    selectorBox->setVisible(false);
    
    currentQValue = PRE::quantisedDivisionValues[PRE::eQuantisationValue1_32];
    lastNoteLength = PRE::quantisedDivisionValues[PRE::eQuantisationValue1_4];
    firstDrag = false;
    firstCall = false;
    lastTrigger = -1;
    ticksPerTimeSignature = PRE::defaultResolution * 4; // 4/4 assume
    isSelecting = false;
    noteCompHeight = 20;
    pixelsPerBar = 900;
}

NoteGridView::~NoteGridView()
{
    for (auto comp : noteComps) {
        removeView(comp);
        comp->forget();
    }
    if (selectorBox) {
        removeView(selectorBox);
        selectorBox->forget();
    }
}

void NoteGridView::draw(CDrawContext* context)
{
    context->setFillColor(PRE::Colors::DarkGrey);
    context->drawRect(getViewSize(), kDrawFilled);
    
    const int totalBars = (getWidth() / pixelsPerBar) + 1;
    
    // Draw piano roll background
    {
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
            
            CColor bgColor = isBlack ? 
                CColor(64, 64, 64, 128) : 
                CColor(96, 96, 96, 128);
            
            context->setFillColor(bgColor);
            CRect noteRect(0, line, getWidth(), line + noteCompHeight);
            context->drawRect(noteRect, kDrawFilled);
            
            line += noteCompHeight;
            context->setFrameColor(PRE::Colors::Black);
            context->drawLine(CPoint(0, line), CPoint(getWidth(), line));
        }
    }
    
    // Draw grid lines (assume 4/4)
    const CCoord increment = pixelsPerBar / 16;
    CCoord line = 0;
    context->setFrameColor(PRE::Colors::LightGrey);
    
    for (int i = 0; line < getWidth(); i++) {
        CCoord lineThickness = 1.0;
        if (i % 16 == 0) { // bar marker
            lineThickness = 3.0;
        }
        else if (i % 4 == 0) { // 1/4 div
            lineThickness = 2.0;
        }
        context->setLineWidth(lineThickness);
        context->drawLine(CPoint(line, 0), CPoint(line, getHeight()));
        
        line += increment;
    }
}

void NoteGridView::setupGrid(float px, float compHeight, const int bars)
{
    pixelsPerBar = px;
    noteCompHeight = compHeight;
    
    CRect newSize = getViewSize();
    newSize.setWidth(pixelsPerBar * bars);
    newSize.setHeight(compHeight * 128);
    setViewSize(newSize);
}

void NoteGridView::setQuantisation(const int val)
{
    if (val >= 0 && val < PRE::eQuantisationValueTotal) {
        currentQValue = PRE::quantisedDivisionValues[val];
    }
}

void NoteGridView::noteCompSelected(NoteView* nc, const CPoint& where, const CButtonState& buttons)
{
    RETURN_IF_EDITING_DISABLED
    
    int dragMove = 0;
    for (auto component : noteComps) {
        if (component->isMultiDrag) {
            dragMove++;
        }
    }
    
    for (auto component : noteComps) {
        if (component == nc) {
            component->setState(NoteView::eSelected);
        }
        else if (component->getState() == NoteView::eSelected && 
                 !buttons.isShiftSet() && !dragMove) {
            component->setState(NoteView::eNone);
        }
    }
    
    // Reset multidrag
    for (auto component : noteComps) {
        if (component->isMultiDrag) {
            component->isMultiDrag = false;
        }
    }
    
    sendEdit();
}

void NoteGridView::noteCompPositionMoved(NoteView* comp, bool callResize)
{
    RETURN_IF_EDITING_DISABLED
    
    if (!firstDrag) {
        firstDrag = true;
        // Move all selected components
        for (auto n : noteComps) {
            if (n != comp && n->getState() == NoteView::eSelected) {
                noteCompPositionMoved(n, false);
            }
        }
        firstDrag = false;
    }
    
    // Convert from component representation to model representation
    CRect bounds = comp->getViewSize();
    int xPos = (bounds.left / ((float)pixelsPerBar)) * ticksPerTimeSignature;
    int note = 127 - (bounds.top / noteCompHeight);
    
    if (note > 127) note = 127;
    else if (note < 0) note = 0;
    
    if (xPos <= 0) xPos = 0;
    
    const int len = (bounds.getWidth() / ((float)pixelsPerBar)) * ticksPerTimeSignature;
    NoteModel nm = comp->getModel();
    nm.setNote(note);
    nm.setStartTime(xPos);
    nm.setNoteLegnth(len);
    nm.quantiseModel(currentQValue, true, true);
    nm.sendChange = sendChange;
    
    lastNoteLength = nm.getNoteLegnth();
    
    comp->startY = -1;
    comp->startX = -1;
    comp->setValues(nm);
    
    if (callResize) {
        setPositions();
    }
    
    sendEdit();
}

void NoteGridView::noteCompLengthChanged(NoteView* original, int diff)
{
    RETURN_IF_EDITING_DISABLED
    
    for (auto n : noteComps) {
        if (n->getState() == NoteView::eSelected || n == original) {
            if (n->startWidth == -1) {
                n->startWidth = n->getViewSize().getWidth();
                n->coordinatesDiffer = true;
            }
            
            const int newWidth = n->startWidth - diff;
            if (newWidth > 20) {
                CRect rect = n->getViewSize();
                rect.setWidth(newWidth);
                n->setViewSize(rect);
            }
        }
    }
    sendEdit();
}

void NoteGridView::noteCompDragging(NoteView* original, const CPoint& where, const CButtonState& buttons)
{
    RETURN_IF_EDITING_DISABLED
    
    // This is simplified - in full implementation would track drag distances
    for (auto n : noteComps) {
        if (n->getState() == NoteView::eSelected && n != original) {
            n->isMultiDrag = true;
        }
    }
    
    // Trigger note while dragging
    CRect bounds = original->getViewSize();
    int note = 127 - (bounds.top / noteCompHeight);
    if (note > 127) note = 127;
    else if (note < 0) note = 0;
    
    if (note != lastTrigger) {
        original->getModel().trigger(note, 100);
        lastTrigger = note;
    }
}

void NoteGridView::setPositions()
{
    for (auto component : noteComps) {
        if (component->coordinatesDiffer) {
            noteCompPositionMoved(component, false);
        }
        
        // Convert from model to component representation
        const float xPos = (component->getModel().getStartTime() / 
                           ((float)ticksPerTimeSignature)) * pixelsPerBar;
        const float yPos = (getHeight() - (component->getModel().getNote() * noteCompHeight)) 
                          - noteCompHeight;
        const float len = (component->getModel().getNoteLegnth() / 
                          ((float)ticksPerTimeSignature)) * pixelsPerBar;
        
        CRect bounds(xPos, yPos, xPos + len, yPos + noteCompHeight);
        component->setViewSize(bounds);
    }
}

CMouseEventResult NoteGridView::onMouseDown(CPoint& where, const CButtonState& buttons)
{
    RETURN_IF_EDITING_DISABLED
    
    // Check for double-click to create note
    if (buttons.isDoubleClick()) {
        const int xPos = (where.x / ((float)pixelsPerBar)) * ticksPerTimeSignature;
        const int yIn = ((float)where.y / noteCompHeight);
        const int note = 127 - yIn;
        
        if (note >= 0 && note <= 127) {
            NoteView* nn = new NoteView(CRect(0, 0, 100, noteCompHeight), styleSheet);
            
            nn->onNoteSelect = [this](NoteView* n, const CPoint& p, const CButtonState& b) {
                this->noteCompSelected(n, p, b);
            };
            nn->onPositionMoved = [this](NoteView* n) {
                this->noteCompPositionMoved(n);
            };
            nn->onLengthChange = [this](NoteView* n, int diff) {
                this->noteCompLengthChanged(n, diff);
            };
            nn->onDragging = [this](NoteView* n, const CPoint& p, const CButtonState& b) {
                this->noteCompDragging(n, p, b);
            };
            
            addView(nn);
            
            const int defaultVelocity = 100;
            NoteModel nModel((u8)note, defaultVelocity, (st_int)xPos, lastNoteLength, {});
            nModel.quantiseModel(currentQValue, true, true);
            nModel.sendChange = sendChange;
            nModel.trigger();
            nn->setValues(nModel);
            
            noteComps.push_back(nn);
            
            setPositions();
            invalid();
            sendEdit();
        }
        
        return kMouseEventHandled;
    }
    
    // Deselect all
    for (NoteView* component : noteComps) {
        component->setState(NoteView::eNone);
    }
    
    // Start selection box
    isSelecting = true;
    selectionStart = where;
    
    sendEdit();
    
    return kMouseEventHandled;
}

CMouseEventResult NoteGridView::onMouseMoved(CPoint& where, const CButtonState& buttons)
{
    RETURN_IF_EDITING_DISABLED
    
    if (buttons.isLeftButton() && isSelecting) {
        if (!selectorBox->isVisible()) {
            selectorBox->setVisible(true);
            CRect rect(selectionStart.x, selectionStart.y, selectionStart.x + 1, selectionStart.y + 1);
            selectorBox->setViewSize(rect);
            selectorBox->startX = selectionStart.x;
            selectorBox->startY = selectionStart.y;
        }
        else {
            CCoord xDir = where.x - selectorBox->startX;
            CCoord yDir = where.y - selectorBox->startY;
            
            CRect newRect;
            if (xDir < 0 && yDir < 0) { // top left
                newRect = CRect(where.x, where.y, selectorBox->startX, selectorBox->startY);
            }
            else if (xDir > 0 && yDir < 0) { // top right
                newRect = CRect(selectorBox->startX, where.y, where.x, selectorBox->startY);
            }
            else if (xDir < 0 && yDir > 0) { // bottom left
                newRect = CRect(where.x, selectorBox->startY, selectorBox->startX, where.y);
            }
            else { // bottom right
                newRect = CRect(selectorBox->startX, selectorBox->startY, where.x, where.y);
            }
            
            selectorBox->setViewSize(newRect);
        }
        
        return kMouseEventHandled;
    }
    
    return kMouseEventNotHandled;
}

CMouseEventResult NoteGridView::onMouseUp(CPoint& where, const CButtonState& buttons)
{
    RETURN_IF_EDITING_DISABLED
    
    if (selectorBox->isVisible()) {
        CRect selRect = selectorBox->getViewSize();
        
        for (NoteView* component : noteComps) {
            CRect compRect = component->getViewSize();
            if (compRect.rectOverlap(selRect)) {
                component->setState(NoteView::eSelected);
            }
            else {
                component->setState(NoteView::eNone);
            }
        }
        
        selectorBox->setVisible(false);
        selectorBox->setViewSize(CRect(0, 0, 1, 1));
    }
    
    isSelecting = false;
    sendEdit();
    
    return kMouseEventHandled;
}

void NoteGridView::deleteAllSelected()
{
    std::vector<NoteView*> toDelete;
    
    for (auto comp : noteComps) {
        if (comp->getState() == NoteView::eSelected) {
            toDelete.push_back(comp);
        }
    }
    
    for (auto comp : toDelete) {
        auto it = std::find(noteComps.begin(), noteComps.end(), comp);
        if (it != noteComps.end()) {
            noteComps.erase(it);
        }
        removeView(comp);
        comp->forget();
    }
    
    invalid();
}

PRESequence NoteGridView::getSequence()
{
    PRESequence seq;
    seq.lowNote = 127;
    seq.highNote = 0;
    
    for (auto comp : noteComps) {
        NoteModel model = comp->getModel();
        seq.events.push_back(model);
        
        if (model.getNote() < seq.lowNote) {
            seq.lowNote = model.getNote();
        }
        if (model.getNote() > seq.highNote) {
            seq.highNote = model.getNote();
        }
    }
    
    seq.tsLow = 4;
    seq.tsHight = 4;
    
    return seq;
}

void NoteGridView::loadSequence(PRESequence sq)
{
    // Clear existing notes
    for (auto comp : noteComps) {
        removeView(comp);
        comp->forget();
    }
    noteComps.clear();
    
    // Load new notes
    for (auto& model : sq.events) {
        NoteView* nn = new NoteView(CRect(0, 0, 100, noteCompHeight), styleSheet);
        
        nn->onNoteSelect = [this](NoteView* n, const CPoint& p, const CButtonState& b) {
            this->noteCompSelected(n, p, b);
        };
        nn->onPositionMoved = [this](NoteView* n) {
            this->noteCompPositionMoved(n);
        };
        nn->onLengthChange = [this](NoteView* n, int diff) {
            this->noteCompLengthChanged(n, diff);
        };
        nn->onDragging = [this](NoteView* n, const CPoint& p, const CButtonState& b) {
            this->noteCompDragging(n, p, b);
        };
        
        model.sendChange = sendChange;
        nn->setValues(model);
        addView(nn);
        noteComps.push_back(nn);
    }
    
    setPositions();
    invalid();
}

float NoteGridView::getNoteCompHeight()
{
    return noteCompHeight;
}

float NoteGridView::getPixelsPerBar()
{
    return pixelsPerBar;
}

std::vector<NoteModel*> NoteGridView::getSelectedModels()
{
    std::vector<NoteModel*> models;
    
    for (auto comp : noteComps) {
        if (comp->getState() == NoteView::eSelected) {
            models.push_back(comp->getModelPtr());
        }
    }
    
    return models;
}

void NoteGridView::sendEdit()
{
    if (onEdit) {
        onEdit();
    }
}
