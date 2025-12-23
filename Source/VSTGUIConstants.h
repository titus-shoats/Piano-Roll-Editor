//
//  VSTGUIConstants.h
//  PianoRollEditor
//
//  VSTGUI version of PConstants.h
//

#ifndef VSTGUIConstants_h
#define VSTGUIConstants_h

#include "vstgui/vstgui.h"

using namespace VSTGUI;

#ifndef u8
typedef unsigned char u8;
#endif
#ifndef st_int
typedef unsigned int st_int;
#endif

namespace PRE { // Piano Roll Editor namespace
/*
 Although this could easily be updated 480 works nicely for MIDI timing and is common in MIDI file formats.
 480/16 = 30 hemidemisemiquaver 1/64
 */
static const int defaultResolution = 480; // per quarter note

static const char * pitches_names[] = {
    "C",
    "C#",
    "D",
    "D#",
    "E",
    "F",
    "F#",
    "G",
    "G#",
    "A",
    "A#",
    "B",
};

enum eQuantisationValue {
    eQuantisationValueNone = 0,
    eQuantisationValue1_32,
    eQuantisationValue1_16,
    eQuantisationValue1_8,
    eQuantisationValue1_4,
    eQuantisationValueTotal,
};

const int quantisedDivisionValues[eQuantisationValueTotal] = {
    1,
    (defaultResolution / 8),
    (defaultResolution / 4),
    (defaultResolution / 2),
    defaultResolution
};

// VSTGUI Color helpers
namespace Colors {
    const CColor DarkGrey = CColor(64, 64, 64, 255);
    const CColor LightGrey = CColor(192, 192, 192, 255);
    const CColor Black = CColor(0, 0, 0, 255);
    const CColor White = CColor(255, 255, 255, 255);
    const CColor GreenYellow = CColor(173, 255, 47, 255);
}

}
#endif /* VSTGUIConstants_h */
