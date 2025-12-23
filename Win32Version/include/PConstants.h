//
//  PConstants.h
//  Win32 Piano Roll Editor
//
//  Constants and type definitions for Piano Roll Editor
//

#ifndef PCONSTANTS_H
#define PCONSTANTS_H

#include <windows.h>
#include <vector>
#include <string>
#include <functional>

#ifndef u8
typedef unsigned char u8;
#endif

#ifndef st_int
typedef unsigned int st_int;
#endif

namespace PRE { // Piano Roll Editor namespace

// Resolution for timing (ticks per quarter note)
// 480 works well for MIDI timing and is common in MIDI file formats
// 480/16 = 30 hemidemisemiquaver 1/64
static const int defaultResolution = 480; // per quarter note

static const char* pitches_names[] = {
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

}

#endif /* PCONSTANTS_H */
