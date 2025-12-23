//
//  PianoRollEditor.cpp
//  Win32 Piano Roll Editor
//
//  Implementation of main piano roll editor
//

#include "../include/PianoRollEditor.h"
#include <algorithm>
#include <windowsx.h>

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
    , m_isDragging(false)
    , m_isSelecting(false)
    , m_quantization(PRE::quantisedDivisionValues[PRE::eQuantisationValue1_16])
{
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

void PianoRollEditor::setPlaybackMarkerPosition(const st_int ticks, bool isVisible)
{
    m_playbackTicks = ticks;
    m_showPlaybackMarker = isVisible;
    InvalidateRect(m_hwnd, NULL, TRUE);
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
    int gridHeight = m_height - m_timelineHeight;
    
    // Draw horizontal lines (notes)
    for (int i = 0; i <= 127; i++) {
        int y = gridTop + i * m_noteHeight - m_scrollY;
        if (y >= gridTop && y < m_height) {
            MoveToEx(hdc, gridLeft, y, NULL);
            LineTo(hdc, m_width, y);
        }
    }
    
    // Draw vertical lines (bars)
    for (int i = 0; i <= m_bars; i++) {
        int x = gridLeft + i * m_pixelsPerBar - m_scrollX;
        if (x >= gridLeft && x < m_width) {
            MoveToEx(hdc, x, gridTop, NULL);
            LineTo(hdc, x, m_height);
        }
        
        // Draw beat subdivisions
        for (int j = 1; j < 4; j++) {
            int beatX = gridLeft + (i * m_pixelsPerBar + j * m_pixelsPerBar / 4) - m_scrollX;
            if (beatX >= gridLeft && beatX < m_width) {
                HPEN lightPen = CreatePen(PS_DOT, 1, GRID_COLOR);
                SelectObject(hdc, lightPen);
                MoveToEx(hdc, beatX, gridTop, NULL);
                LineTo(hdc, beatX, m_height);
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
    
    RECT keyboardRect = {0, m_timelineHeight, m_keyboardWidth, m_height};
    HBRUSH bgBrush = CreateSolidBrush(RGB(50, 50, 50));
    FillRect(hdc, &keyboardRect, bgBrush);
    DeleteObject(bgBrush);
    
    for (int i = 0; i < 128; i++) {
        int y = m_timelineHeight + (127 - i) * m_noteHeight - m_scrollY;
        if (y + m_noteHeight < m_timelineHeight || y > m_height) continue;
        
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
    
    for (auto& note : m_notes) {
        int x = m_keyboardWidth + tickToPixel(note.getStartTime()) - m_scrollX;
        int y = m_timelineHeight + (127 - note.getNote()) * m_noteHeight - m_scrollY;
        int width = std::max(5, tickToPixel(note.getNoteLegnth()));
        int height = m_noteHeight - 2;
        
        if (x + width < m_keyboardWidth || x > m_width ||
            y + height < m_timelineHeight || y > m_height) {
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
    
    HPEN markerPen = CreatePen(PS_SOLID, 3, RGB(150, 255, 150));
    HPEN oldPen = (HPEN)SelectObject(hdc, markerPen);
    
    MoveToEx(hdc, x, 0, NULL);
    LineTo(hdc, x, m_height);
    
    SelectObject(hdc, oldPen);
    DeleteObject(markerPen);
}

void PianoRollEditor::onSize(int width, int height)
{
    m_width = width;
    m_height = height;
    updateScrollBars();
}

void PianoRollEditor::onMouseMove(int x, int y, WPARAM wParam)
{
    if (m_isDragging) {
        // Handle note dragging
    } else if (m_isSelecting) {
        m_selectionRect.right = x;
        m_selectionRect.bottom = y;
        InvalidateRect(m_hwnd, NULL, TRUE);
    }
}

void PianoRollEditor::onMouseDown(int x, int y, WPARAM wParam)
{
    if (x < m_keyboardWidth || y < m_timelineHeight) return;
    
    bool ctrlPressed = (wParam & MK_CONTROL) != 0;
    
    // Check if clicking on existing note
    bool clickedNote = false;
    for (auto& nr : m_noteRects) {
        if (PtInRect(&nr.rect, {x, y})) {
            clickedNote = true;
            break;
        }
    }
    
    if (!clickedNote) {
        // Start selection or create new note
        if (wParam & MK_LBUTTON) {
            if (wParam & MK_SHIFT) {
                m_isSelecting = true;
                m_selectionRect = {x, y, x, y};
            } else {
                // Double-click creates note
                addNote(x, y);
            }
        }
    }
}

void PianoRollEditor::onMouseUp(int x, int y, WPARAM wParam)
{
    if (m_isSelecting) {
        selectNotesInRect(m_selectionRect);
        m_isSelecting = false;
        InvalidateRect(m_hwnd, NULL, TRUE);
    }
    m_isDragging = false;
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

void PianoRollEditor::onHScroll(WPARAM wParam)
{
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

void PianoRollEditor::onVScroll(WPARAM wParam)
{
    int action = LOWORD(wParam);
    int maxScroll = 127 * m_noteHeight - (m_height - m_timelineHeight);
    
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
    si.nMin = 0;
    si.nMax = 127 * m_noteHeight;
    si.nPage = m_height - m_timelineHeight;
    si.nPos = m_scrollY;
    SetScrollInfo(m_hwnd, SB_VERT, &si, TRUE);
}
