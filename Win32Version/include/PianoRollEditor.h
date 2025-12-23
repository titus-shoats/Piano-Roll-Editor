//
//  PianoRollEditor.h
//  Win32 Piano Roll Editor
//
//  Main piano roll editor window
//

#ifndef PIANOROLLEDITOR_H
#define PIANOROLLEDITOR_H

#include "PConstants.h"
#include "NoteModel.h"
#include <windows.h>
#include <vector>
#include <map>

// Forward declarations
class NoteComponent;

class PianoRollEditor {
public:
    PianoRollEditor(HWND hwnd);
    ~PianoRollEditor();
    
    void setup(const int bars, const int pixelsPerBar, const int noteHeight);
    void updateBars(const int newNumberOfBars);
    
    void loadSequence(PRESequence sequence);
    PRESequence getSequence();
    
    void setPlaybackMarkerPosition(const st_int ticks, bool isVisible = true);
    void setScroll(int x, int y);
    
    void onPaint(HDC hdc);
    void onSize(int width, int height);
    void onMouseMove(int x, int y, WPARAM wParam);
    void onMouseDown(int x, int y, WPARAM wParam);
    void onMouseUp(int x, int y, WPARAM wParam);
    void onMouseWheel(int delta);
    void onKeyDown(WPARAM key);
    void onHScroll(WPARAM wParam);
    void onVScroll(WPARAM wParam);
    
    std::function<void()> onEdit;
    std::function<void(int note, int velocity)> sendChange;
    
private:
    struct NoteRect {
        RECT rect;
        NoteModel* model;
        bool selected;
    };
    
    void paintGrid(HDC hdc);
    void paintKeyboard(HDC hdc);
    void paintTimeline(HDC hdc);
    void paintNotes(HDC hdc);
    void paintPlaybackMarker(HDC hdc);
    
    void addNote(int x, int y);
    void deleteSelectedNotes();
    void selectNote(int x, int y, bool addToSelection);
    void selectNotesInRect(RECT rect);
    void deselectAll();
    
    int pixelToNote(int y);
    int noteToPixel(int note);
    int pixelToTick(int x);
    int tickToPixel(st_int tick);
    
    void updateScrollBars();
    
    HWND m_hwnd;
    int m_width;
    int m_height;
    
    int m_bars;
    int m_pixelsPerBar;
    int m_noteHeight;
    
    int m_scrollX;
    int m_scrollY;
    
    st_int m_playbackTicks;
    bool m_showPlaybackMarker;
    
    int m_keyboardWidth;
    int m_timelineHeight;
    
    std::vector<NoteModel> m_notes;
    std::vector<NoteRect> m_noteRects;
    
    bool m_isDragging;
    bool m_isSelecting;
    POINT m_dragStart;
    RECT m_selectionRect;
    
    int m_quantization;
    
    static const int GRID_COLOR = RGB(60, 60, 60);
    static const int BG_COLOR = RGB(40, 40, 40);
    static const int NOTE_COLOR = RGB(100, 150, 200);
    static const int NOTE_SELECTED_COLOR = RGB(150, 200, 255);
    static const int WHITE_KEY_COLOR = RGB(220, 220, 220);
    static const int BLACK_KEY_COLOR = RGB(100, 100, 100);
};

#endif /* PIANOROLLEDITOR_H */
