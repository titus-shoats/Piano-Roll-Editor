//
//  VSTGUIControlPanelView.cpp
//  PianoRollEditor - VSTGUI
//

#include "VSTGUIControlPanelView.hpp"

NoteGridControlPanelView::NoteGridControlPanelView(const CRect& size, 
                                                   NoteGridView& ng, 
                                                   NoteGridStyleSheet& ss)
    : CView(size), noteGrid(ng), styleSheet(ss)
{
}

NoteGridControlPanelView::~NoteGridControlPanelView()
{
}

void NoteGridControlPanelView::draw(CDrawContext* context)
{
    context->setFillColor(CColor(48, 48, 48, 255));
    context->drawRect(getViewSize(), kDrawFilled);
    
    context->setFrameColor(PRE::Colors::LightGrey);
    context->drawRect(getViewSize(), kDrawStroked);
    
    // Draw simple label
    context->setFontColor(PRE::Colors::White);
    context->setFont(kNormalFont);
    
    CRect textRect = getViewSize();
    textRect.inset(10, 10);
    context->drawString("Control Panel", textRect, kLeftText);
}

void NoteGridControlPanelView::setQuantisation(PRE::eQuantisationValue value)
{
    noteGrid.setQuantisation(value);
}
