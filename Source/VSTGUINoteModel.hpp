//
//  VSTGUINoteModel.hpp
//  PianoRollEditor - VSTGUI
//
//  VSTGUI version - data model remains framework independent
//

#ifndef VSTGUINoteModel_hpp
#define VSTGUINoteModel_hpp

#include "VSTGUIConstants.h"
#include <vector>
#include <iostream>

class NoteModel {
public:
    
    struct Flags {
        Flags ()
        {
            state = 0;
            isGenerative = 0;
        }
        unsigned int state : 2;
        unsigned int isGenerative : 2;
        
    };
    
    NoteModel ();
    NoteModel (u8 n, u8 v, st_int st, st_int nl, Flags flags);
    NoteModel (const NoteModel & other);
    
    
    void quantiseModel (int qValue, bool qStartTime, bool qNoteLegnth);
    
    bool compare (const NoteModel & other, bool compareUIDs = true);
    
    // getters and setters
    void setNote (u8 _note);
    void setVelocity (u8 _velocity);
    void setStartTime (st_int _time);
    void setNoteLegnth (st_int _len);
    
    u8 getNote () {return note;}
    u8 getVelocity () {return velocity;}
    st_int getStartTime () {return startTime;}
    st_int getNoteLegnth () {return noteLegnth;}
    
    
    Flags flags;
    
    std::function<void(int note,int velocity)> sendChange;
    void trigger();
    void trigger(const u8 note, const u8 vel);
    
private:
    u8 note;
    u8 velocity;
    st_int startTime;
    st_int noteLegnth;
};

class PRESequence { //Piano Roll Editor Sequence
public:
    std::vector<NoteModel> events;
    int tsLow;
    int tsHight;
    
    int lowNote;
    int highNote;
    
    void print ();
};

#endif /* VSTGUINoteModel_hpp */
