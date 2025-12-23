//
//  VSTGUINoteGridStyleSheet.cpp
//  PianoRollEditor - VSTGUI
//

#include "VSTGUINoteGridStyleSheet.hpp"

NoteGridStyleSheet::NoteGridStyleSheet ()
{
    drawMIDINum = false;
    drawMIDINoteStr = false;
    drawVelocity = true;
    disableEditing = false;
}

bool NoteGridStyleSheet::getDrawMIDINum ()
{
    return drawMIDINum;
}

bool NoteGridStyleSheet::getDrawMIDINoteStr ()
{
    return drawMIDINoteStr;
}

bool NoteGridStyleSheet::getDrawVelocity ()
{
    return drawVelocity;
}
