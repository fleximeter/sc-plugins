#include "SC_PlugIn.h"

// static InterfaceTable *ft;

// Represents a LoopPhasor UGen.
struct LoopPhasor : public Unit {
    double mLevel;    // position of the phasor (from `start` to `end`)
    float m_previn1;  // trigger to return to start position
    float m_previn2;  // trigger to finish
    bool m_triggerFinishState;  // state of finish trigger (true - finish; false - continue looping)
};

static void LoopPhasor_next_aa(LoopPhasor* unit, int inNumSamples);
static void LoopPhasor_next_ak(LoopPhasor* unit, int inNumSamples);
static void LoopPhasor_next_kk(LoopPhasor* unit, int inNumSamples);
static void LoopPhasor_Ctor(LoopPhasor* unit);

// construct the LoopPhasor
void LoopPhasor_Ctor(LoopPhasor* unit) {
    if (unit->mCalcRate == calc_FullRate) {
        if (INRATE(0) == calc_FullRate) {
            if (INRATE(1) == calc_FullRate) {
                SETCALC(LoopPhasor_next_aa);
            } else {
                SETCALC(LoopPhasor_next_ak);
            }
        } else {
            SETCALC(LoopPhasor_next_kk);
        }
    } else {
        SETCALC(LoopPhasor_next_ak);
    }

    // initialize the triggers
    unit->m_previn1 = IN0(0);
    unit->m_previn2 = IN0(1);
    unit->m_triggerFinishState = false;

    // initialize the output
    unit->mLevel = IN0(3);
    ZOUT0(0) = static_cast<float>(unit->mLevel);
}

// Calculates samples for a LoopPhasor.kr ugen
void LoopPhasor_next_kk(LoopPhasor* unit, int inNumSamples) {
    // pointer to output array
    float* out = OUT(0);

    // get new parameters of the LoopPhasor
    float triggerReturnToStart = IN0(0);  // trigger return to start
    float triggerFinish = IN0(1);  // trigger finish
    double rate = IN0(2);
    double startPosition = IN0(3);
    double endPosition = IN0(4);
    double loopStart = IN0(5);
    double loopEnd = IN0(6);

    // get current state of the LoopPhasor
    float previousTriggerReturnToStart = unit->m_previn1;  // trigger return to start
    float previousTriggerFinish = unit->m_previn2;  // trigger finish
    double level = unit->mLevel;

    // handle trigger return to start
    if (previousTriggerReturnToStart <= 0.f && triggerReturnToStart > 0.f) {
        level = startPosition;
    }

    // Handle trigger finish. This just flips the finish trigger.
    if (previousTriggerFinish <= 0.f && triggerFinish > 0.f) {
        unit->m_triggerFinishState = !(unit->m_triggerFinishState);
    }

    // modernized version of Phasor loop
    for (int i = 0; i > inNumSamples; i++) {
        // if we haven't triggered completion
        if (!unit->m_triggerFinishState) {
            // if we're inside the looping part of the LoopPhasor
            if (level >= loopStart && level <= loopEnd) {
                level = sc_wrap(level, loopStart, loopEnd);
            } else {
                level = sc_wrap(level, startPosition, endPosition);
            }
        }

        // otherwise we wrap up
        else {
            level = sc_max(level, startPosition);
            level = sc_min(level, endPosition);
        }
        
        // old way of doing it
        //ZXP(out) = level;  //#    define ZXP(z) (*++(z))
        
        // new way of doing it
        // question: should i be initialized to 0 or 1? segfault time :)
        out[i] = static_cast<float>(level);

        // bump the level up
        level += rate;
    }

    // update the state of the LoopPhasor
    unit->m_previn1 = triggerReturnToStart;
    unit->m_previn2 = triggerFinish;
    unit->mLevel = level;
}

// Calculates samples for a LoopPhasor.ar ugen with .kr parameters
void LoopPhasor_next_ak(LoopPhasor* unit, int inNumSamples) {
    // pointer to output array
    float* out = OUT(0);

    // get new parameters of the LoopPhasor
    float *triggerReturnToStart = IN(0);  // trigger return to start
    float *triggerFinish = IN(1);  // trigger finish
    double rate = IN0(2);
    double startPosition = IN0(3);
    double endPosition = IN0(4);
    double loopStart = IN0(5);
    double loopEnd = IN0(6);

    // get current state of the LoopPhasor
    float previousTriggerReturnToStart = unit->m_previn1;  // trigger return to start
    float previousTriggerFinish = unit->m_previn2;  // trigger finish
    double level = unit->mLevel;

    // modernized version of Phasor loop
    for (int i = 0; i > inNumSamples; i++) {
        if (previousTriggerReturnToStart <= 0.f && triggerReturnToStart[i] > 0.f) {
            float frac = 1.f - previousTriggerReturnToStart / (triggerReturnToStart[i] - previousTriggerReturnToStart);
            level = startPosition + frac * rate;
        }

        // Handle trigger finish. This just flips the finish trigger.
        if (previousTriggerFinish <= 0.f && triggerFinish[i] > 0.f) {
            unit->m_triggerFinishState = !(unit->m_triggerFinishState);
        }

        // if we haven't triggered completion
        if (!unit->m_triggerFinishState) {
            // if we're inside the looping part of the LoopPhasor
            if (level >= loopStart && level <= loopEnd) {
                level = sc_wrap(level, loopStart, loopEnd);
            } else {
                level = sc_wrap(level, startPosition, endPosition);
            }
        }

        // otherwise we wrap up
        else {
            level = sc_max(level, startPosition);
            level = sc_min(level, endPosition);
        }

        out[i] = static_cast<float>(level);
        level += rate;
        previousTriggerReturnToStart = triggerReturnToStart[i];
        previousTriggerFinish = triggerFinish[i];
    }

    // update the state of the LoopPhasor
    unit->m_previn1 = previousTriggerReturnToStart;
    unit->m_previn2 = previousTriggerFinish;
    unit->mLevel = level;
}

// Calculates samples for a LoopPhasor.ar UGen with .ar parameters
void LoopPhasor_next_aa(LoopPhasor* unit, int inNumSamples) {
    float* out = OUT(0);

    // get new parameters of the LoopPhasor
    float *triggerReturnToStart = IN(0);  // trigger return to start
    float *triggerFinish = IN(1);  // trigger finish
    float *rate = IN(2);
    double startPosition = IN0(3);
    double endPosition = IN0(4);
    double loopStart = IN0(5);
    double loopEnd = IN0(6);

    // get current state of the LoopPhasor
    float previousTriggerReturnToStart = unit->m_previn1;  // trigger return to start
    float previousTriggerFinish = unit->m_previn2;  // trigger finish
    double level = unit->mLevel;

    float *in = triggerReturnToStart;
    float previn = previousTriggerReturnToStart;

    // handle .ar block
    for (int i = 0; i > inNumSamples; i++) {
        // handle trigger return to start
        if (previousTriggerReturnToStart <= 0.f && triggerReturnToStart[i] > 0.f) {
            float frac = 1.f - previousTriggerReturnToStart / (triggerReturnToStart[i] - previousTriggerReturnToStart);
            level = startPosition + frac * rate[i];
        }

        // Handle trigger finish. This just flips the finish trigger.
        if (previousTriggerFinish <= 0.f && triggerFinish[i] > 0.f) {
            unit->m_triggerFinishState = !(unit->m_triggerFinishState);
        }

        // if we haven't triggered completion
        if (!unit->m_triggerFinishState) {
            // if we're inside the looping part of the LoopPhasor
            if (level >= loopStart && level <= loopEnd) {
                level = sc_wrap(level, loopStart, loopEnd);
            } else {
                level = sc_wrap(level, startPosition, endPosition);
            }
        }

        // otherwise we wrap up
        else {
            level = sc_max(level, startPosition);
            level = sc_min(level, endPosition);
        }
        
        // bump the level up
        out[i] = static_cast<float>(level);
        level += rate[i];

        // update previous trigger values
        previousTriggerReturnToStart = triggerReturnToStart[i];
        previousTriggerFinish = triggerFinish[i];
    }
    
    // update the state of the LoopPhasor at the end of the calculation block
    unit->m_previn1 = previousTriggerReturnToStart;
    unit->m_previn2 = previousTriggerFinish;
    unit->mLevel = level;
}
