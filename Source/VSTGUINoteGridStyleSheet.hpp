//
//  VSTGUINoteGridStyleSheet.hpp
//  PianoRollEditor - VSTGUI
//
//  Style sheet for note grid - framework independent
//

#ifndef VSTGUINoteGridStyleSheet_hpp
#define VSTGUINoteGridStyleSheet_hpp

class NoteGridStyleSheet {
public:
    friend class NoteGridControlPanelView;
    
    NoteGridStyleSheet ();
    
    bool getDrawMIDINum ();
    bool getDrawMIDINoteStr ();
    bool getDrawVelocity ();
    
    bool disableEditing;
    
private:
    bool drawMIDINum, drawMIDINoteStr, drawVelocity;
};

#endif /* VSTGUINoteGridStyleSheet_hpp */
