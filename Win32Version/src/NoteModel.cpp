//
//  NoteModel.cpp
//  Win32 Piano Roll Editor
//
//  Implementation of note data model
//

#include "../include/NoteModel.h"
#include <algorithm>

NoteModel::NoteModel() 
    : note(60), velocity(100), startTime(0), noteLegnth(PRE::defaultResolution)
{
}

NoteModel::NoteModel(u8 n, u8 v, st_int st, st_int nl, Flags flags)
    : note(n), velocity(v), startTime(st), noteLegnth(nl), flags(flags)
{
}

NoteModel::NoteModel(const NoteModel& other)
    : note(other.note), velocity(other.velocity), 
      startTime(other.startTime), noteLegnth(other.noteLegnth),
      flags(other.flags), sendChange(other.sendChange)
{
}

void NoteModel::quantiseModel(int qValue, bool qStartTime, bool qNoteLegnth)
{
    if (qStartTime && qValue > 0) {
        int remainder = startTime % qValue;
        if (remainder < qValue / 2) {
            startTime -= remainder;
        } else {
            startTime += (qValue - remainder);
        }
    }
    
    if (qNoteLegnth && qValue > 0) {
        int remainder = noteLegnth % qValue;
        if (remainder < qValue / 2) {
            noteLegnth -= remainder;
        } else {
            noteLegnth += (qValue - remainder);
        }
        noteLegnth = std::max((st_int)1, noteLegnth);
    }
}

bool NoteModel::compare(const NoteModel& other, bool compareUIDs)
{
    return (note == other.note && 
            velocity == other.velocity &&
            startTime == other.startTime &&
            noteLegnth == other.noteLegnth);
}

void NoteModel::setNote(u8 _note)
{
    note = _note;
    trigger(note, velocity);
}

void NoteModel::setVelocity(u8 _velocity)
{
    velocity = _velocity;
}

void NoteModel::setStartTime(st_int _time)
{
    startTime = _time;
}

void NoteModel::setNoteLegnth(st_int _len)
{
    noteLegnth = _len;
}

void NoteModel::trigger()
{
    trigger(note, velocity);
}

void NoteModel::trigger(const u8 note, const u8 vel)
{
    if (sendChange) {
        sendChange(note, vel);
    }
}

void PRESequence::print()
{
    std::cout << "Sequence contains " << events.size() << " notes\n";
    for (const auto& note : events) {
        std::cout << "Note: " << (int)note.getNote() 
                  << " Vel: " << (int)note.getVelocity()
                  << " Start: " << note.getStartTime()
                  << " Length: " << note.getNoteLegnth() << "\n";
    }
}
