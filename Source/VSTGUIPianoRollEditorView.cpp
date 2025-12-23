//
//  VSTGUIPianoRollEditorView.cpp
//  PianoRollEditor - VSTGUI
//

#include "VSTGUIPianoRollEditorView.hpp"

PianoRollEditorView::PianoRollEditorView(const CRect& size) : CViewContainer(size)
{
    setBackgroundColor(CColor(32, 32, 32, 255));
    
    // Create note grid
    noteGrid = new NoteGridView(CRect(0, 0, 4000, 20 * 127), styleSheet);
    
    // Create scroll view for grid
    CRect gridViewportSize(80, 50, size.right - 10, size.bottom - 60);
    CRect gridContainerSize(0, 0, 4000, 20 * 127);
    viewportGrid = new CustomScrollView(gridViewportSize, gridContainerSize);
    viewportGrid->setScrollbarWidth(10);
    viewportGrid->addView(noteGrid);
    addView(viewportGrid);
    
    // Create timeline
    timelineComp = new TimelineView(CRect(0, 0, 100, 45));
    CRect timelineViewportSize(80, 5, size.right - 10, 50);
    CRect timelineContainerSize(0, 0, 4000, 45);
    viewportTimeline = new CScrollView(timelineViewportSize, timelineContainerSize, 0); // No scrollbars
    viewportTimeline->addView(timelineComp);
    addView(viewportTimeline);
    
    // Create keyboard
    keyboardComp = new KeyboardView(CRect(0, 0, 70, 20 * 127));
    CRect keyboardViewportSize(5, 50, 75, size.bottom - 60);
    CRect keyboardContainerSize(0, 0, 70, 20 * 127);
    viewportPiano = new CScrollView(keyboardViewportSize, keyboardContainerSize, 0); // No scrollbars
    viewportPiano->addView(keyboardComp);
    addView(viewportPiano);
    
    // Synchronize scroll positions
    viewportGrid->positionMoved = [this](CCoord x, CCoord y) {
        if (viewportTimeline) {
            viewportTimeline->setScrollOffset(CPoint(x, 0));
        }
        if (viewportPiano) {
            viewportPiano->setScrollOffset(CPoint(0, y));
        }
    };
    
    // Create control panel
    controlPanel = new NoteGridControlPanelView(CRect(5, size.bottom - 145, size.right - 5, size.bottom - 5), 
                                                *noteGrid, styleSheet);
    addView(controlPanel);
    
    controlPanel->configureGrid = [this](int pixelsPerBar, int noteHeight) {
        setup(10, pixelsPerBar, noteHeight);
    };
    
    // Forward edit callbacks
    noteGrid->onEdit = [this]() {
        if (this->onEdit) {
            this->onEdit();
        }
    };
    
    noteGrid->sendChange = [this](int note, int vel) {
        if (this->sendChange) {
            this->sendChange(note, vel);
        }
    };
    
    showPlaybackMarker = false;
    playbackTicks = 0;
    currentBars = 10;
    currentPixelsPerBar = 900;
    currentNoteHeight = 20;
}

PianoRollEditorView::~PianoRollEditorView()
{
}

void PianoRollEditorView::setup(const int bars, const int pixelsPerBar, const int noteHeight)
{
    if (bars > 1 && bars < 1000) {
        currentBars = bars;
        currentPixelsPerBar = pixelsPerBar;
        currentNoteHeight = noteHeight;
        
        noteGrid->setupGrid(pixelsPerBar, noteHeight, bars);
        timelineComp->setup(bars, pixelsPerBar);
        
        // Update container sizes for scroll views
        CRect gridContainerSize(0, 0, pixelsPerBar * bars, noteHeight * 128);
        viewportGrid->setContainerSize(gridContainerSize);
        
        CRect timelineContainerSize(0, 0, pixelsPerBar * bars, timelineComp->getHeight());
        viewportTimeline->setContainerSize(timelineContainerSize);
        
        CRect keyRect = keyboardComp->getViewSize();
        keyRect.setHeight(noteGrid->getHeight());
        keyboardComp->setViewSize(keyRect);
        
        CRect keyboardContainerSize(0, 0, 70, noteGrid->getHeight());
        viewportPiano->setContainerSize(keyboardContainerSize);
        
        invalid();
    }
}

void PianoRollEditorView::updateBars(const int newNumberOfBars)
{
    if (newNumberOfBars > 1 && newNumberOfBars < 1000) {
        const float pPb = noteGrid->getPixelsPerBar();
        const float nH = noteGrid->getNoteCompHeight();
        
        noteGrid->setupGrid(pPb, nH, newNumberOfBars);
        timelineComp->setup(newNumberOfBars, pPb);
        
        // Update container sizes for scroll views
        CRect gridContainerSize(0, 0, pPb * newNumberOfBars, nH * 128);
        viewportGrid->setContainerSize(gridContainerSize);
        
        CRect timelineContainerSize(0, 0, pPb * newNumberOfBars, timelineComp->getHeight());
        viewportTimeline->setContainerSize(timelineContainerSize);
        
        CRect keyRect = keyboardComp->getViewSize();
        keyRect.setHeight(noteGrid->getHeight());
        keyboardComp->setViewSize(keyRect);
        
        CRect keyboardContainerSize(0, 0, 70, noteGrid->getHeight());
        viewportPiano->setContainerSize(keyboardContainerSize);
        
        invalid();
    }
}

void PianoRollEditorView::draw(CDrawContext* context)
{
    CViewContainer::draw(context);
}

void PianoRollEditorView::drawRect(CDrawContext* context, const CRect& updateRect)
{
    CViewContainer::drawRect(context, updateRect);
    
    // Draw playback marker
    if (showPlaybackMarker) {
        const CCoord x = noteGrid->getPixelsPerBar() * 
                        (playbackTicks / (4.0 * PRE::defaultResolution));
        const CPoint offset = viewportGrid->getScrollOffset();
        
        context->setFrameColor(PRE::Colors::GreenYellow);
        context->setLineWidth(5.0);
        
        CRect gridRect = viewportGrid->getViewSize();
        context->drawLine(CPoint(gridRect.left + x - offset.x, gridRect.top), 
                         CPoint(gridRect.left + x - offset.x, gridRect.bottom));
    }
}

void PianoRollEditorView::showControlPanel(bool state)
{
    controlPanel->setVisible(state);
    
    // Adjust viewport sizes when control panel visibility changes
    CRect gridRect = viewportGrid->getViewSize();
    if (state) {
        gridRect.bottom = getHeight() - 150;
    } else {
        gridRect.bottom = getHeight() - 5;
    }
    viewportGrid->setViewSize(gridRect);
    
    CRect pianoRect = viewportPiano->getViewSize();
    pianoRect.bottom = gridRect.bottom;
    viewportPiano->setViewSize(pianoRect);
    
    invalid();
}

void PianoRollEditorView::loadSequence(PRESequence sequence)
{
    noteGrid->loadSequence(sequence);
}

PRESequence PianoRollEditorView::getSequence()
{
    return noteGrid->getSequence();
}

void PianoRollEditorView::setScroll(double x, double y)
{
    CRect gridSize = noteGrid->getViewSize();
    CRect viewportSize = viewportGrid->getViewSize();
    
    CCoord scrollX = x * (gridSize.getWidth() - viewportSize.getWidth());
    CCoord scrollY = y * (gridSize.getHeight() - viewportSize.getHeight());
    
    viewportGrid->setScrollOffset(CPoint(scrollX, scrollY));
}

void PianoRollEditorView::setPlaybackMarkerPosition(const st_int ticks, bool isVisible)
{
    showPlaybackMarker = isVisible;
    playbackTicks = ticks;
    invalid();
}

void PianoRollEditorView::disableEditing(bool value)
{
    styleSheet.disableEditing = value;
    noteGrid->invalid();
}

NoteGridControlPanelView& PianoRollEditorView::getControlPanel()
{
    return *controlPanel;
}

PianoRollEditorView::ExternalModelEditor PianoRollEditorView::getSelectedNoteModels()
{
    ExternalModelEditor mEdit;
    mEdit.update = [this]() {
        noteGrid->setPositions();
        noteGrid->invalid();
    };
    mEdit.models = noteGrid->getSelectedModels();
    return mEdit;
}
