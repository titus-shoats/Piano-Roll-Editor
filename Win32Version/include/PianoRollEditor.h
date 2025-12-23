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
#include <commctrl.h>
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
    
    void setPlaybackMarkerPosition(const st_int ticks, bool isVisible = true, bool invalidate = true);
    void setScroll(int x, int y);
    
    void onPaint(HDC hdc);
    void onSize(int width, int height);
    void onMouseMove(int x, int y, WPARAM wParam);
    void onMouseDown(int x, int y, WPARAM wParam);
    void onMouseUp(int x, int y, WPARAM wParam);
    void onMouseWheel(int delta);
    void onKeyDown(WPARAM key);
    void onHScroll(WPARAM wParam, LPARAM lParam);
    void onVScroll(WPARAM wParam, LPARAM lParam);
    void onCommand(WPARAM wParam, LPARAM lParam);
    
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
    void paintControlPanel(HDC hdc);
    
    void createControlPanel();
    void updateControlsFromSettings();
    
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
    int m_controlPanelHeight;
    bool m_showControlPanel;
    
    std::vector<NoteModel> m_notes;
    std::vector<NoteRect> m_noteRects;
    
    bool m_isDragging;
    bool m_isSelecting;
    POINT m_dragStart;
    RECT m_selectionRect;
    
    int m_quantization;
    
    // Control panel controls
    HWND m_sliderPixelsPerBar;
    HWND m_sliderNoteHeight;
    HWND m_checkDrawMIDINotes;
    HWND m_checkDrawMIDIText;
    HWND m_checkDrawVelocity;
    HWND m_comboQuantization;
    HWND m_labelPixelsPerBar;
    HWND m_labelNoteHeight;
    HWND m_labelQuantization;
    
    bool m_drawMIDINotes;
    bool m_drawMIDIText;
    bool m_drawVelocity;
    
    static const int GRID_COLOR = RGB(60, 60, 60);
    static const int BG_COLOR = RGB(40, 40, 40);
    static const int NOTE_COLOR = RGB(100, 150, 200);
    static const int NOTE_SELECTED_COLOR = RGB(150, 200, 255);
    static const int WHITE_KEY_COLOR = RGB(220, 220, 220);
    static const int BLACK_KEY_COLOR = RGB(100, 100, 100);
    
    // Control IDs
    static const int ID_SLIDER_PIXELS_PER_BAR = 1001;
    static const int ID_SLIDER_NOTE_HEIGHT = 1002;
    static const int ID_CHECK_DRAW_NOTES = 1003;
    static const int ID_CHECK_DRAW_TEXT = 1004;
    static const int ID_CHECK_DRAW_VELOCITY = 1005;
    static const int ID_COMBO_QUANTIZATION = 1006;
};

#endif /* PIANOROLLEDITOR_H */
