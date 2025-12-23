//
//  PianoRollEditor.cpp
//  Win32 Piano Roll Editor
//
//  Implementation of main piano roll editor
//

#include "../include/PianoRollEditor.h"
#include <algorithm>
#include <windowsx.h>

#pragma comment(lib, "comctl32.lib")

PianoRollEditor::PianoRollEditor(HWND hwnd)
    : m_hwnd(hwnd)
    , m_width(800)
    , m_height(600)
    , m_bars(10)
    , m_pixelsPerBar(900)
    , m_noteHeight(20)
    , m_scrollX(0)
    , m_scrollY(0)
    , m_playbackTicks(0)
    , m_showPlaybackMarker(false)
    , m_keyboardWidth(80)
    , m_timelineHeight(50)
    , m_controlPanelHeight(140)
    , m_showControlPanel(true)
    , m_isDragging(false)
    , m_isSelecting(false)
    , m_isResizing(false)
    , m_quantization(PRE::quantisedDivisionValues[PRE::eQuantisationValue1_16])
    , m_sliderPixelsPerBar(NULL)
    , m_sliderNoteHeight(NULL)
    , m_checkDrawMIDINotes(NULL)
    , m_checkDrawMIDIText(NULL)
    , m_checkDrawVelocity(NULL)
    , m_comboQuantization(NULL)
    , m_labelPixelsPerBar(NULL)
    , m_labelNoteHeight(NULL)
    , m_labelQuantization(NULL)
    , m_drawMIDINotes(true)
    , m_drawMIDIText(false)
    , m_drawVelocity(false)
    , m_draggedNoteIndex(-1)
    , m_resizedNoteIndex(-1)
    , m_resizeStartWidth(0)
    , m_lastPlaybackTicks(0)
{
    m_lastMousePos.x = 0;
    m_lastMousePos.y = 0;
    
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_STANDARD_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);
    
    createControlPanel();
}

PianoRollEditor::~PianoRollEditor()
{
}

void PianoRollEditor::setup(const int bars, const int pixelsPerBar, const int noteHeight)
{
    if (bars > 1 && bars < 1000) {
        m_bars = bars;
        m_pixelsPerBar = pixelsPerBar;
        m_noteHeight = noteHeight;
        updateScrollBars();
        InvalidateRect(m_hwnd, NULL, TRUE);
    }
}

void PianoRollEditor::updateBars(const int newNumberOfBars)
{
    if (newNumberOfBars > 1 && newNumberOfBars < 1000) {
        m_bars = newNumberOfBars;
        updateScrollBars();
        InvalidateRect(m_hwnd, NULL, TRUE);
    }
}

void PianoRollEditor::loadSequence(PRESequence sequence)
{
    m_notes = sequence.events;
    InvalidateRect(m_hwnd, NULL, TRUE);
}

PRESequence PianoRollEditor::getSequence()
{
    PRESequence seq;
    seq.events = m_notes;
    seq.tsLow = 4;
    seq.tsHigh = 4;
    
    if (!m_notes.empty()) {
        seq.lowNote = 127;
        seq.highNote = 0;
        for (const auto& note : m_notes) {
            seq.lowNote = std::min(seq.lowNote, (int)note.getNote());
            seq.highNote = std::max(seq.highNote, (int)note.getNote());
        }
    } else {
        seq.lowNote = 0;
        seq.highNote = 127;
    }
    
    return seq;
}

void PianoRollEditor::setPlaybackMarkerPosition(const st_int ticks, bool isVisible, bool invalidate)
{
    if (invalidate && m_showPlaybackMarker) {
        // Invalidate old marker position
        int oldX = m_keyboardWidth + tickToPixel(m_lastPlaybackTicks) - m_scrollX;
        RECT oldMarkerRect = {oldX - 3, 0, oldX + 3, m_height};
        InvalidateRect(m_hwnd, &oldMarkerRect, FALSE);
    }
    
    m_lastPlaybackTicks = m_playbackTicks;
    m_playbackTicks = ticks;
    m_showPlaybackMarker = isVisible;
    
    if (invalidate && m_showPlaybackMarker) {
        // Invalidate new marker position
        int newX = m_keyboardWidth + tickToPixel(m_playbackTicks) - m_scrollX;
        RECT newMarkerRect = {newX - 3, 0, newX + 3, m_height};
        InvalidateRect(m_hwnd, &newMarkerRect, FALSE);
    }
}

void PianoRollEditor::setScroll(int x, int y)
{
    m_scrollX = x;
    m_scrollY = y;
    updateScrollBars();
    InvalidateRect(m_hwnd, NULL, TRUE);
}

void PianoRollEditor::onPaint(HDC hdc)
{
    // Create double buffer for flicker-free drawing
    RECT clientRect;
    GetClientRect(m_hwnd, &clientRect);
    
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
    
    // Fill background
    HBRUSH bgBrush = CreateSolidBrush(BG_COLOR);
    FillRect(memDC, &clientRect, bgBrush);
    DeleteObject(bgBrush);
    
    // Paint components
    paintGrid(memDC);
    paintNotes(memDC);
    paintKeyboard(memDC);
    paintTimeline(memDC);
    
    if (m_showControlPanel) {
        paintControlPanel(memDC);
    }
    
    if (m_showPlaybackMarker) {
        paintPlaybackMarker(memDC);
    }
    
    // Draw selection rectangle
    if (m_isSelecting) {
        HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
        HPEN oldPen = (HPEN)SelectObject(memDC, pen);
        HBRUSH brush = (HBRUSH)GetStockObject(NULL_BRUSH);
        HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, brush);
        
        Rectangle(memDC, m_selectionRect.left, m_selectionRect.top,
                  m_selectionRect.right, m_selectionRect.bottom);
        
        SelectObject(memDC, oldBrush);
        SelectObject(memDC, oldPen);
        DeleteObject(pen);
    }
    
    // Copy to screen
    BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);
    
    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
}

void PianoRollEditor::paintGrid(HDC hdc)
{
    HPEN gridPen = CreatePen(PS_SOLID, 1, GRID_COLOR);
    HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);
    
    int gridLeft = m_keyboardWidth;
    int gridTop = m_timelineHeight;
    int gridWidth = m_width - m_keyboardWidth;
    int gridBottom = m_showControlPanel ? (m_height - m_controlPanelHeight) : m_height;
    int gridHeight = gridBottom - m_timelineHeight;
    
    // Draw horizontal lines (notes)
    for (int i = 0; i <= 127; i++) {
        int y = gridTop + i * m_noteHeight - m_scrollY;
        if (y >= gridTop && y < gridBottom) {
            MoveToEx(hdc, gridLeft, y, NULL);
            LineTo(hdc, m_width, y);
        }
    }
    
    // Draw vertical lines (bars)
    for (int i = 0; i <= m_bars; i++) {
        int x = gridLeft + i * m_pixelsPerBar - m_scrollX;
        if (x >= gridLeft && x < m_width) {
            MoveToEx(hdc, x, gridTop, NULL);
            LineTo(hdc, x, gridBottom);
        }
        
        // Draw beat subdivisions
        for (int j = 1; j < 4; j++) {
            int beatX = gridLeft + (i * m_pixelsPerBar + j * m_pixelsPerBar / 4) - m_scrollX;
            if (beatX >= gridLeft && beatX < m_width) {
                HPEN lightPen = CreatePen(PS_DOT, 1, GRID_COLOR);
                SelectObject(hdc, lightPen);
                MoveToEx(hdc, beatX, gridTop, NULL);
                LineTo(hdc, beatX, gridBottom);
                SelectObject(hdc, gridPen);
                DeleteObject(lightPen);
            }
        }
    }
    
    SelectObject(hdc, oldPen);
    DeleteObject(gridPen);
}

void PianoRollEditor::paintKeyboard(HDC hdc)
{
    static const int blackKeys[] = {1, 3, 6, 8, 10}; // C#, D#, F#, G#, A#
    
    int gridBottom = m_showControlPanel ? (m_height - m_controlPanelHeight) : m_height;
    RECT keyboardRect = {0, m_timelineHeight, m_keyboardWidth, gridBottom};
    HBRUSH bgBrush = CreateSolidBrush(RGB(50, 50, 50));
    FillRect(hdc, &keyboardRect, bgBrush);
    DeleteObject(bgBrush);
    
    for (int i = 0; i < 128; i++) {
        int y = m_timelineHeight + (127 - i) * m_noteHeight - m_scrollY;
        if (y + m_noteHeight < m_timelineHeight || y > gridBottom) continue;
        
        int noteInOctave = i % 12;
        bool isBlack = false;
        for (int bk : blackKeys) {
            if (noteInOctave == bk) {
                isBlack = true;
                break;
            }
        }
        
        RECT keyRect = {5, y, m_keyboardWidth - 5, y + m_noteHeight - 1};
        HBRUSH keyBrush = CreateSolidBrush(isBlack ? BLACK_KEY_COLOR : WHITE_KEY_COLOR);
        FillRect(hdc, &keyRect, keyBrush);
        DeleteObject(keyBrush);
        
        // Draw key border
        HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
        MoveToEx(hdc, keyRect.left, keyRect.top, NULL);
        LineTo(hdc, keyRect.right, keyRect.top);
        LineTo(hdc, keyRect.right, keyRect.bottom);
        LineTo(hdc, keyRect.left, keyRect.bottom);
        LineTo(hdc, keyRect.left, keyRect.top);
        SelectObject(hdc, oldPen);
        DeleteObject(borderPen);
        
        // Draw note name for C notes
        if (noteInOctave == 0) {
            char noteName[16];
            sprintf_s(noteName, "C%d", (i / 12) - 1);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(0, 0, 0));
            RECT textRect = keyRect;
            DrawTextA(hdc, noteName, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }
}

void PianoRollEditor::paintTimeline(HDC hdc)
{
    RECT timelineRect = {m_keyboardWidth, 0, m_width, m_timelineHeight};
    HBRUSH bgBrush = CreateSolidBrush(RGB(60, 60, 60));
    FillRect(hdc, &timelineRect, bgBrush);
    DeleteObject(bgBrush);
    
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(200, 200, 200));
    
    for (int i = 0; i <= m_bars; i++) {
        int x = m_keyboardWidth + i * m_pixelsPerBar - m_scrollX;
        if (x >= m_keyboardWidth && x < m_width) {
            char barLabel[16];
            sprintf_s(barLabel, "%d", i + 1);
            RECT textRect = {x - 20, 5, x + 20, m_timelineHeight - 5};
            DrawTextA(hdc, barLabel, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }
}

void PianoRollEditor::paintNotes(HDC hdc)
{
    m_noteRects.clear();
    
    int gridBottom = m_showControlPanel ? (m_height - m_controlPanelHeight) : m_height;
    
    for (auto& note : m_notes) {
        int x = m_keyboardWidth + tickToPixel(note.getStartTime()) - m_scrollX;
        int y = m_timelineHeight + (127 - note.getNote()) * m_noteHeight - m_scrollY;
        int width = std::max(5, tickToPixel(note.getNoteLegnth()));
        int height = m_noteHeight - 2;
        
        if (x + width < m_keyboardWidth || x > m_width ||
            y + height < m_timelineHeight || y > gridBottom) {
            continue;
        }
        
        NoteRect nr;
        nr.rect = {x, y, x + width, y + height};
        nr.model = &note;
        nr.selected = false; // Will be updated based on selection state
        
        COLORREF color = NOTE_COLOR;
        // You could check if note is selected and use NOTE_SELECTED_COLOR
        
        HBRUSH noteBrush = CreateSolidBrush(color);
        FillRect(hdc, &nr.rect, noteBrush);
        DeleteObject(noteBrush);
        
        // Draw note border
        HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
        MoveToEx(hdc, nr.rect.left, nr.rect.top, NULL);
        LineTo(hdc, nr.rect.right, nr.rect.top);
        LineTo(hdc, nr.rect.right, nr.rect.bottom);
        LineTo(hdc, nr.rect.left, nr.rect.bottom);
        LineTo(hdc, nr.rect.left, nr.rect.top);
        SelectObject(hdc, oldPen);
        DeleteObject(borderPen);
        
        m_noteRects.push_back(nr);
    }
}

void PianoRollEditor::paintPlaybackMarker(HDC hdc)
{
    int x = m_keyboardWidth + tickToPixel(m_playbackTicks) - m_scrollX;
    int gridBottom = m_showControlPanel ? (m_height - m_controlPanelHeight) : m_height;
    
    HPEN markerPen = CreatePen(PS_SOLID, 3, RGB(150, 255, 150));
    HPEN oldPen = (HPEN)SelectObject(hdc, markerPen);
    
    MoveToEx(hdc, x, 0, NULL);
    LineTo(hdc, x, gridBottom);
    
    SelectObject(hdc, oldPen);
    DeleteObject(markerPen);
}

void PianoRollEditor::onSize(int width, int height)
{
    m_width = width;
    m_height = height;
    
    // Reposition control panel controls
    if (m_showControlPanel) {
        int panelTop = m_height - m_controlPanelHeight;
        
        if (m_sliderPixelsPerBar) {
            SetWindowPos(m_sliderPixelsPerBar, NULL, 10, panelTop + 30, 300, 30, SWP_NOZORDER);
        }
        if (m_sliderNoteHeight) {
            SetWindowPos(m_sliderNoteHeight, NULL, 10, panelTop + 80, 300, 30, SWP_NOZORDER);
        }
        if (m_labelPixelsPerBar) {
            SetWindowPos(m_labelPixelsPerBar, NULL, 10, panelTop + 10, 150, 20, SWP_NOZORDER);
        }
        if (m_labelNoteHeight) {
            SetWindowPos(m_labelNoteHeight, NULL, 10, panelTop + 60, 150, 20, SWP_NOZORDER);
        }
        if (m_checkDrawMIDINotes) {
            SetWindowPos(m_checkDrawMIDINotes, NULL, 320, panelTop + 10, 150, 25, SWP_NOZORDER);
        }
        if (m_checkDrawMIDIText) {
            SetWindowPos(m_checkDrawMIDIText, NULL, 320, panelTop + 45, 150, 25, SWP_NOZORDER);
        }
        if (m_checkDrawVelocity) {
            SetWindowPos(m_checkDrawVelocity, NULL, 320, panelTop + 80, 150, 25, SWP_NOZORDER);
        }
        if (m_labelQuantization) {
            SetWindowPos(m_labelQuantization, NULL, 480, panelTop + 10, 100, 20, SWP_NOZORDER);
        }
        if (m_comboQuantization) {
            SetWindowPos(m_comboQuantization, NULL, 480, panelTop + 30, 100, 200, SWP_NOZORDER);
        }
    }
    
    updateScrollBars();
}

void PianoRollEditor::onMouseMove(int x, int y, WPARAM wParam)
{
    // Update cursor based on position
    if (!m_isDragging && !m_isResizing && !m_isSelecting) {
        // Check if mouse is over a note's right edge for resizing
        bool overResizeEdge = false;
        for (size_t i = 0; i < m_noteRects.size(); i++) {
            const auto& nr = m_noteRects[i];
            if (PtInRect(&nr.rect, {x, y})) {
                // Check if near right edge (within 8 pixels)
                if (x >= nr.rect.right - 8 && x <= nr.rect.right) {
                    SetCursor(LoadCursor(NULL, IDC_SIZEWE));
                    overResizeEdge = true;
                    break;
                }
            }
        }
        if (!overResizeEdge) {
            SetCursor(LoadCursor(NULL, IDC_ARROW));
        }
    }
    
    if (m_isDragging && m_draggedNoteIndex >= 0 && m_draggedNoteIndex < (int)m_notes.size()) {
        // Calculate drag delta
        int deltaX = x - m_lastMousePos.x;
        int deltaY = y - m_lastMousePos.y;
        
        // Convert delta to ticks and note change
        int deltaTicks = (deltaX * PRE::defaultResolution * 4) / m_pixelsPerBar;
        int deltaNotes = -deltaY / m_noteHeight;
        
        if (deltaTicks != 0 || deltaNotes != 0) {
            NoteModel& note = m_notes[m_draggedNoteIndex];
            
            // Update note position
            int newStartTime = (int)note.getStartTime() + deltaTicks;
            int newNote = (int)note.getNote() + deltaNotes;
            
            // Clamp values
            newStartTime = std::max(0, newStartTime);
            newNote = std::max(0, std::min(127, newNote));
            
            note.setStartTime(newStartTime);
            note.setNote(newNote);
            
            m_lastMousePos.x = x;
            m_lastMousePos.y = y;
            
            InvalidateRect(m_hwnd, NULL, TRUE);
            
            if (onEdit) {
                onEdit();
            }
        }
    }
    else if (m_isResizing && m_resizedNoteIndex >= 0 && m_resizedNoteIndex < (int)m_notes.size()) {
        // Calculate resize delta
        int deltaX = x - m_lastMousePos.x;
        int deltaTicks = (deltaX * PRE::defaultResolution * 4) / m_pixelsPerBar;
        
        if (deltaTicks != 0) {
            NoteModel& note = m_notes[m_resizedNoteIndex];
            
            // Update note length
            int newLength = (int)note.getNoteLegnth() + deltaTicks;
            newLength = std::max(PRE::defaultResolution / 16, newLength); // Minimum 1/64 note
            
            note.setNoteLegnth(newLength);
            
            m_lastMousePos.x = x;
            
            InvalidateRect(m_hwnd, NULL, TRUE);
            
            if (onEdit) {
                onEdit();
            }
        }
    }
    else if (m_isSelecting) {
        m_selectionRect.right = x;
        m_selectionRect.bottom = y;
        InvalidateRect(m_hwnd, NULL, TRUE);
    }
}

void PianoRollEditor::onMouseDown(int x, int y, WPARAM wParam)
{
    if (x < m_keyboardWidth || y < m_timelineHeight) return;
    
    int gridBottom = m_showControlPanel ? (m_height - m_controlPanelHeight) : m_height;
    if (y >= gridBottom) return;
    
    bool ctrlPressed = (wParam & MK_CONTROL) != 0;
    bool shiftPressed = (wParam & MK_SHIFT) != 0;
    
    // Check if clicking on existing note
    bool clickedNote = false;
    int clickedNoteIndex = -1;
    RECT clickedNoteRect = {0};
    
    for (size_t i = 0; i < m_noteRects.size(); i++) {
        const auto& nr = m_noteRects[i];
        if (PtInRect(&nr.rect, {x, y})) {
            clickedNote = true;
            clickedNoteRect = nr.rect;
            
            // Find the actual note in m_notes vector
            for (size_t j = 0; j < m_notes.size(); j++) {
                if (&m_notes[j] == nr.model) {
                    clickedNoteIndex = j;
                    break;
                }
            }
            break;
        }
    }
    
    if (clickedNote && clickedNoteIndex >= 0) {
        // Check if clicking on right edge for resizing
        if (x >= clickedNoteRect.right - 8 && x <= clickedNoteRect.right) {
            // Start resizing
            m_isResizing = true;
            m_resizedNoteIndex = clickedNoteIndex;
            m_resizeStartWidth = clickedNoteRect.right - clickedNoteRect.left;
            m_lastMousePos.x = x;
            m_lastMousePos.y = y;
            SetCursor(LoadCursor(NULL, IDC_SIZEWE));
        }
        else {
            // Start dragging
            m_isDragging = true;
            m_draggedNoteIndex = clickedNoteIndex;
            m_lastMousePos.x = x;
            m_lastMousePos.y = y;
        }
    }
    else {
        // No note clicked
        if (shiftPressed) {
            // Start selection rectangle
            m_isSelecting = true;
            m_selectionRect = {x, y, x, y};
        }
        // Note: Double-click will be handled by WM_LBUTTONDBLCLK to create notes
    }
}

void PianoRollEditor::onMouseUp(int x, int y, WPARAM wParam)
{
    if (m_isSelecting) {
        selectNotesInRect(m_selectionRect);
        m_isSelecting = false;
        InvalidateRect(m_hwnd, NULL, TRUE);
    }
    
    if (m_isDragging) {
        m_isDragging = false;
        m_draggedNoteIndex = -1;
        SetCursor(LoadCursor(NULL, IDC_ARROW));
    }
    
    if (m_isResizing) {
        m_isResizing = false;
        m_resizedNoteIndex = -1;
        m_resizeStartWidth = 0;
        SetCursor(LoadCursor(NULL, IDC_ARROW));
    }
}

void PianoRollEditor::onMouseDoubleClick(int x, int y, WPARAM wParam)
{
    if (x < m_keyboardWidth || y < m_timelineHeight) return;
    
    int gridBottom = m_showControlPanel ? (m_height - m_controlPanelHeight) : m_height;
    if (y >= gridBottom) return;
    
    // Add a new note at the double-click position
    addNote(x, y);
}

void PianoRollEditor::onMouseWheel(int delta)
{
    m_scrollY -= delta / 3;
    updateScrollBars();
    InvalidateRect(m_hwnd, NULL, TRUE);
}

void PianoRollEditor::onKeyDown(WPARAM key)
{
    if (key == VK_DELETE || key == VK_BACK) {
        deleteSelectedNotes();
    }
}

void PianoRollEditor::onHScroll(WPARAM wParam, LPARAM lParam)
{
    // Check if it's from a trackbar control
    if (lParam != 0) {
        HWND hControl = (HWND)lParam;
        if (hControl == m_sliderPixelsPerBar) {
            int newValue = SendMessage(m_sliderPixelsPerBar, TBM_GETPOS, 0, 0);
            m_pixelsPerBar = newValue;
            setup(m_bars, m_pixelsPerBar, m_noteHeight);
            return;
        }
        else if (hControl == m_sliderNoteHeight) {
            int newValue = SendMessage(m_sliderNoteHeight, TBM_GETPOS, 0, 0);
            m_noteHeight = newValue;
            setup(m_bars, m_pixelsPerBar, m_noteHeight);
            return;
        }
    }
    
    // Handle window scrollbar
    int action = LOWORD(wParam);
    int maxScroll = m_bars * m_pixelsPerBar - (m_width - m_keyboardWidth);
    
    switch (action) {
        case SB_LINELEFT: m_scrollX -= 20; break;
        case SB_LINERIGHT: m_scrollX += 20; break;
        case SB_PAGELEFT: m_scrollX -= m_width / 2; break;
        case SB_PAGERIGHT: m_scrollX += m_width / 2; break;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            m_scrollX = HIWORD(wParam);
            break;
    }
    
    m_scrollX = std::max(0, std::min(maxScroll, m_scrollX));
    updateScrollBars();
    InvalidateRect(m_hwnd, NULL, TRUE);
}

void PianoRollEditor::onVScroll(WPARAM wParam, LPARAM lParam)
{
    // lParam is the handle of the scrollbar control (0 for window scrollbar)
    int action = LOWORD(wParam);
    int gridHeight = m_showControlPanel ? (m_height - m_timelineHeight - m_controlPanelHeight) : (m_height - m_timelineHeight);
    int maxScroll = 127 * m_noteHeight - gridHeight;
    
    switch (action) {
        case SB_LINEUP: m_scrollY -= m_noteHeight; break;
        case SB_LINEDOWN: m_scrollY += m_noteHeight; break;
        case SB_PAGEUP: m_scrollY -= m_height / 2; break;
        case SB_PAGEDOWN: m_scrollY += m_height / 2; break;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            m_scrollY = HIWORD(wParam);
            break;
    }
    
    m_scrollY = std::max(0, std::min(maxScroll, m_scrollY));
    updateScrollBars();
    InvalidateRect(m_hwnd, NULL, TRUE);
}

void PianoRollEditor::addNote(int x, int y)
{
    int note = pixelToNote(y);
    int tick = pixelToTick(x);
    
    // Quantize
    if (m_quantization > 0) {
        int remainder = tick % m_quantization;
        if (remainder < m_quantization / 2) {
            tick -= remainder;
        } else {
            tick += (m_quantization - remainder);
        }
    }
    
    NoteModel newNote(note, 100, tick, PRE::defaultResolution / 4, NoteModel::Flags());
    m_notes.push_back(newNote);
    
    if (sendChange) {
        sendChange(note, 100);
    }
    
    if (onEdit) {
        onEdit();
    }
    
    InvalidateRect(m_hwnd, NULL, TRUE);
}

void PianoRollEditor::deleteSelectedNotes()
{
    // Placeholder for selection logic
    InvalidateRect(m_hwnd, NULL, TRUE);
}

void PianoRollEditor::selectNote(int x, int y, bool addToSelection)
{
    // Placeholder for selection logic
}

void PianoRollEditor::selectNotesInRect(RECT rect)
{
    // Placeholder for selection logic
}

void PianoRollEditor::deselectAll()
{
    // Placeholder for selection logic
}

int PianoRollEditor::pixelToNote(int y)
{
    int noteY = y - m_timelineHeight + m_scrollY;
    int note = 127 - (noteY / m_noteHeight);
    return std::max(0, std::min(127, note));
}

int PianoRollEditor::noteToPixel(int note)
{
    return (127 - note) * m_noteHeight;
}

int PianoRollEditor::pixelToTick(int x)
{
    int noteX = x - m_keyboardWidth + m_scrollX;
    return (noteX * PRE::defaultResolution * 4) / m_pixelsPerBar;
}

int PianoRollEditor::tickToPixel(st_int tick)
{
    return (tick * m_pixelsPerBar) / (PRE::defaultResolution * 4);
}

void PianoRollEditor::updateScrollBars()
{
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    
    // Horizontal scrollbar
    si.nMin = 0;
    si.nMax = m_bars * m_pixelsPerBar;
    si.nPage = m_width - m_keyboardWidth;
    si.nPos = m_scrollX;
    SetScrollInfo(m_hwnd, SB_HORZ, &si, TRUE);
    
    // Vertical scrollbar
    int gridHeight = m_showControlPanel ? (m_height - m_timelineHeight - m_controlPanelHeight) : (m_height - m_timelineHeight);
    si.nMin = 0;
    si.nMax = 127 * m_noteHeight;
    si.nPage = gridHeight;
    si.nPos = m_scrollY;
    SetScrollInfo(m_hwnd, SB_VERT, &si, TRUE);
}

void PianoRollEditor::createControlPanel()
{
    int panelTop = m_height - m_controlPanelHeight;
    
    // Create sliders for pixels per bar and note height
    m_sliderPixelsPerBar = CreateWindowEx(
        0, TRACKBAR_CLASS, NULL,
        WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS,
        10, panelTop + 30, 300, 30,
        m_hwnd, (HMENU)ID_SLIDER_PIXELS_PER_BAR, GetModuleHandle(NULL), NULL);
    
    SendMessage(m_sliderPixelsPerBar, TBM_SETRANGE, TRUE, MAKELPARAM(400, 2000));
    SendMessage(m_sliderPixelsPerBar, TBM_SETPOS, TRUE, m_pixelsPerBar);
    SendMessage(m_sliderPixelsPerBar, TBM_SETTICFREQ, 100, 0);
    
    m_sliderNoteHeight = CreateWindowEx(
        0, TRACKBAR_CLASS, NULL,
        WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_AUTOTICKS,
        10, panelTop + 80, 300, 30,
        m_hwnd, (HMENU)ID_SLIDER_NOTE_HEIGHT, GetModuleHandle(NULL), NULL);
    
    SendMessage(m_sliderNoteHeight, TBM_SETRANGE, TRUE, MAKELPARAM(10, 30));
    SendMessage(m_sliderNoteHeight, TBM_SETPOS, TRUE, m_noteHeight);
    SendMessage(m_sliderNoteHeight, TBM_SETTICFREQ, 2, 0);
    
    // Create labels
    m_labelPixelsPerBar = CreateWindowEx(
        0, L"STATIC", L"Pixels per bar",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        10, panelTop + 10, 150, 20,
        m_hwnd, NULL, GetModuleHandle(NULL), NULL);
    
    m_labelNoteHeight = CreateWindowEx(
        0, L"STATIC", L"Pixels per row",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        10, panelTop + 60, 150, 20,
        m_hwnd, NULL, GetModuleHandle(NULL), NULL);
    
    // Create checkboxes
    m_checkDrawMIDINotes = CreateWindowEx(
        0, L"BUTTON", L"Draw MIDI Notes",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        320, panelTop + 10, 150, 25,
        m_hwnd, (HMENU)ID_CHECK_DRAW_NOTES, GetModuleHandle(NULL), NULL);
    SendMessage(m_checkDrawMIDINotes, BM_SETCHECK, m_drawMIDINotes ? BST_CHECKED : BST_UNCHECKED, 0);
    
    m_checkDrawMIDIText = CreateWindowEx(
        0, L"BUTTON", L"Draw MIDI Text",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        320, panelTop + 45, 150, 25,
        m_hwnd, (HMENU)ID_CHECK_DRAW_TEXT, GetModuleHandle(NULL), NULL);
    SendMessage(m_checkDrawMIDIText, BM_SETCHECK, m_drawMIDIText ? BST_CHECKED : BST_UNCHECKED, 0);
    
    m_checkDrawVelocity = CreateWindowEx(
        0, L"BUTTON", L"Draw Velocity",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        320, panelTop + 80, 150, 25,
        m_hwnd, (HMENU)ID_CHECK_DRAW_VELOCITY, GetModuleHandle(NULL), NULL);
    SendMessage(m_checkDrawVelocity, BM_SETCHECK, m_drawVelocity ? BST_CHECKED : BST_UNCHECKED, 0);
    
    // Create quantization combobox
    m_labelQuantization = CreateWindowEx(
        0, L"STATIC", L"Quantization",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        480, panelTop + 10, 100, 20,
        m_hwnd, NULL, GetModuleHandle(NULL), NULL);
    
    m_comboQuantization = CreateWindowEx(
        0, L"COMBOBOX", NULL,
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        480, panelTop + 30, 100, 200,
        m_hwnd, (HMENU)ID_COMBO_QUANTIZATION, GetModuleHandle(NULL), NULL);
    
    SendMessage(m_comboQuantization, CB_ADDSTRING, 0, (LPARAM)L"1/64");
    SendMessage(m_comboQuantization, CB_ADDSTRING, 0, (LPARAM)L"1/32");
    SendMessage(m_comboQuantization, CB_ADDSTRING, 0, (LPARAM)L"1/16");
    SendMessage(m_comboQuantization, CB_ADDSTRING, 0, (LPARAM)L"1/8");
    SendMessage(m_comboQuantization, CB_ADDSTRING, 0, (LPARAM)L"1/4");
    SendMessage(m_comboQuantization, CB_SETCURSEL, 2, 0); // Default to 1/16
}

void PianoRollEditor::paintControlPanel(HDC hdc)
{
    int panelTop = m_height - m_controlPanelHeight;
    
    // Draw panel background
    RECT panelRect = {0, panelTop, m_width, m_height};
    HBRUSH panelBrush = CreateSolidBrush(RGB(50, 50, 50));
    FillRect(hdc, &panelRect, panelBrush);
    DeleteObject(panelBrush);
    
    // Draw separator line
    HPEN linePen = CreatePen(PS_SOLID, 2, RGB(80, 80, 80));
    HPEN oldPen = (HPEN)SelectObject(hdc, linePen);
    MoveToEx(hdc, 0, panelTop, NULL);
    LineTo(hdc, m_width, panelTop);
    SelectObject(hdc, oldPen);
    DeleteObject(linePen);
    
    // Note: The actual controls (sliders, checkboxes, combobox) are painted by Windows
}

void PianoRollEditor::updateControlsFromSettings()
{
    if (m_sliderPixelsPerBar) {
        SendMessage(m_sliderPixelsPerBar, TBM_SETPOS, TRUE, m_pixelsPerBar);
    }
    if (m_sliderNoteHeight) {
        SendMessage(m_sliderNoteHeight, TBM_SETPOS, TRUE, m_noteHeight);
    }
}

void PianoRollEditor::onCommand(WPARAM wParam, LPARAM lParam)
{
    int controlId = LOWORD(wParam);
    int notificationCode = HIWORD(wParam);
    
    switch (controlId) {
        case ID_CHECK_DRAW_NOTES:
            m_drawMIDINotes = (SendMessage(m_checkDrawMIDINotes, BM_GETCHECK, 0, 0) == BST_CHECKED);
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
            
        case ID_CHECK_DRAW_TEXT:
            m_drawMIDIText = (SendMessage(m_checkDrawMIDIText, BM_GETCHECK, 0, 0) == BST_CHECKED);
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
            
        case ID_CHECK_DRAW_VELOCITY:
            m_drawVelocity = (SendMessage(m_checkDrawVelocity, BM_GETCHECK, 0, 0) == BST_CHECKED);
            InvalidateRect(m_hwnd, NULL, TRUE);
            break;
            
        case ID_COMBO_QUANTIZATION:
            if (notificationCode == CBN_SELCHANGE) {
                int sel = SendMessage(m_comboQuantization, CB_GETCURSEL, 0, 0);
                if (sel >= 0 && sel < PRE::eQuantisationValueTotal) {
                    m_quantization = PRE::quantisedDivisionValues[sel];
                }
            }
            break;
    }
}
