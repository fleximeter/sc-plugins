/*
File: ImpulseJitter.cpp
Author: Jeff Martin

Description:
This file contains the ImpulseJitter UGen implementation.

Copyright © 2025 by Jeffrey Martin. All rights reserved.
Website: https://www.jeffreymartincomposer.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "SC_PlugIn.h"
#include <stdio.h>

static InterfaceTable *ft;

// This is a copy of the static function from LFUGens.cpp in server/plugins.
// It detects if a phasor is out-of-bounds, triggers, and wraps [0, 1].
static inline float testWrapPhase(double prev_inc, double& phase) {
    if (prev_inc < 0.f) { // negative freqs
        if (phase <= 0.f) {
            phase += 1.f;
            if (phase <= 0.f) { // catch large phase jumps
                phase -= sc_ceil(phase);
            }
            return 1.f;
        } else {
            return 0.f;
        }
    } else { // positive freqs
        if (phase >= 1.f) {
            phase -= 1.f;
            if (phase >= 1.f) {
                phase -= sc_floor(phase);
            }
            return 1.f;
        } else {
            return 0.f;
        }
    }
}

// Represents an ImpulseJitter UGen.
struct ImpulseJitter : public Unit {
    double mPhase, mPhaseOffset, mPhaseIncrement;
    float mFreqMul;
    int mTableMaxSize;
    int mTableSize;
    int *mImpulseTable;
};

void ImpulseJitter_next_aa(ImpulseJitter* unit, int inNumSamples) {
    float* out = OUT(0);
    float* freqIn = IN(0);
    float* offIn = IN(1);
    float jitterFracIn = IN0(2);
    
    // Collect UGen state
    double phase = unit->mPhase;
    double inc = unit->mPhaseIncrement;
    float freqMul = unit->mFreqMul;
    double prevOff = unit->mPhaseOffset;
    
    int jitterWidth = static_cast<int>(jitterFracIn * inNumSamples);

    // Zero out the output buffer
    for (int xxn = 0; xxn < inNumSamples; xxn++) {
        out[xxn] = 0.f;
    }

    RGET
    for (int xxn = 0; xxn < inNumSamples; xxn++) {
        float impulseResult = testWrapPhase(inc, phase);
        if (impulseResult > 0.5f) {
            int randLow = xxn - jitterWidth;
            int randHigh = xxn + jitterWidth;
            if (randLow < 0) {
                randLow = 0;
            }
            if (randHigh >= inNumSamples) {
                randHigh = inNumSamples - 1;
            }
            int idx = rgen.irand(randHigh - randLow) + randLow;
            out[idx] = 1.f;
        }
        double off = static_cast<double>(offIn[xxn]);
        double offInc = off - prevOff;
        phase += offInc;
        testWrapPhase(inc, phase);
        inc = freqIn[xxn] * freqMul;
        phase += inc;
        prevOff = off;
    }

    unit->mPhase = phase;
    unit->mPhaseOffset = prevOff;
    unit->mPhaseIncrement = inc;
}

void ImpulseJitter_next_ai(ImpulseJitter* unit, int inNumSamples) {
    float* out = OUT(0);
    float freqIn = IN0(0);
    float jitterFracIn = IN0(2);

    // Collect UGen state
    double phase = unit->mPhase;
    double inc = unit->mPhaseIncrement;
    float freqMul = unit->mFreqMul;

    int jitterWidth = static_cast<int>(jitterFracIn * inNumSamples);

    // Zero out the output buffer
    for (int xxn = 0; xxn < inNumSamples; xxn++) {
        out[xxn] = 0.f;
    }

    RGET
    for (int xxn = 0; xxn < inNumSamples; xxn++) {
        float impulseResult = testWrapPhase(inc, phase);
        if (impulseResult > 0.5f) {
            int randLow = xxn - jitterWidth;
            int randHigh = xxn + jitterWidth;
            if (randLow < 0) {
                randLow = 0;
            }
            if (randHigh >= inNumSamples) {
                randHigh = inNumSamples - 1;
            }
            int idx = rgen.irand(randHigh - randLow) + randLow;
            out[idx] = 1.f;
        }
        inc = freqIn * freqMul;
        phase += inc;
    }

    unit->mPhase = phase;
    unit->mPhaseIncrement = inc;
}

void ImpulseJitter_next_ak(ImpulseJitter* unit, int inNumSamples) {
    float* out = OUT(0);
    float freqIn = IN0(0);
    double off = IN0(1);
    float jitterFracIn = IN0(2);
    
    // Collect UGen state
    double phase = unit->mPhase;
    double inc = unit->mPhaseIncrement;
    float freqMul = unit->mFreqMul;
    double prevOff = unit->mPhaseOffset;

    double offSlope = CALCSLOPE(off, prevOff);
    bool offChanged = offSlope != 0.f;

    int jitterWidth = static_cast<int>(jitterFracIn * inNumSamples);
    
    // Zero out the output buffer
    for (int xxn = 0; xxn < inNumSamples; xxn++) {
        out[xxn] = 0.f;
    }

    RGET
    for (int xxn = 0; xxn < inNumSamples; xxn++) {
        float impulseResult = testWrapPhase(inc, phase);
        if (impulseResult > 0.5f) {
            int randLow = xxn - jitterWidth;
            int randHigh = xxn + jitterWidth;
            if (randLow < 0) {
                randLow = 0;
            }
            if (randHigh >= inNumSamples) {
                randHigh = inNumSamples - 1;
            }
            int idx = rgen.irand(randHigh - randLow) + randLow;
            out[idx] = 1.f;
        }
        if (offChanged) {
            phase += offSlope;
            testWrapPhase(inc, phase);
        }
        inc = freqIn * freqMul;
        phase += inc;
    }

    unit->mPhase = phase;
    unit->mPhaseOffset = off;
    unit->mPhaseIncrement = inc;
}

void ImpulseJitter_next_ki(ImpulseJitter* unit, int inNumSamples) {
    float* out = OUT(0);
    double inc = IN0(0) * unit->mFreqMul;
    float jitterFracIn = IN0(2);

    // Collect UGen state
    double phase = unit->mPhase;
    double prevInc = unit->mPhaseIncrement;
    
    double incSlope = CALCSLOPE(inc, prevInc);
    
    // The maximum distance an impulse can be displaced
    int jitterWidth = static_cast<int>(jitterFracIn * unit->mTableMaxSize);

    // Zero out the output buffer
    for (int xxn = 0; xxn < inNumSamples; xxn++) {
        out[xxn] = 0.f;
    }

    // Update the impulse table indices
    for (int xxn = 0; xxn < unit->mTableSize; xxn++) {
        unit->mImpulseTable[xxn] -= inNumSamples;
        if (unit->mImpulseTable[xxn] < inNumSamples) {
            out[unit->mImpulseTable[xxn]] = 1.f;
            unit->mImpulseTable[xxn] = -1;
        }
    }

    // TODO: Change the table to a min heap

    RGET
    for (int xxn = 0; xxn < inNumSamples; xxn++) {
        float impulseResult = testWrapPhase(prevInc, phase);
        if (impulseResult > 0.5f) {
            int idx = rgen.irand(jitterWidth) + xxn;
            if (idx < inNumSamples) {
                out[idx] = 1.f;
            } else if (unit->mTableSize < unit->mTableMaxSize) {
                unit->mImpulseTable[unit->mTableSize] = idx;
                unit->mTableSize++;
            }
        }
        prevInc += incSlope;
        phase += prevInc;
    }

    unit->mPhase = phase;
    unit->mPhaseIncrement = inc;
}

void ImpulseJitter_next_kk(ImpulseJitter* unit, int inNumSamples) {
    float* out = OUT(0);
    double inc = IN0(0) * unit->mFreqMul;
    double off = IN0(1);
    float jitterFracIn = IN0(2);

    // Collect UGen state
    double phase = unit->mPhase;
    double prevInc = unit->mPhaseIncrement;
    double prevOff = unit->mPhaseOffset;
    
    double incSlope = CALCSLOPE(inc, prevInc);
    double phaseSlope = CALCSLOPE(off, prevOff);
    bool phOffChanged = phaseSlope != 0.f;

    int jitterWidth = static_cast<int>(jitterFracIn * inNumSamples);

    // Zero out the output buffer
    for (int xxn = 0; xxn < inNumSamples; xxn++) {
        out[xxn] = 0.f;
    }

    RGET
    for (int xxn = 0; xxn < inNumSamples; xxn++) {
        float impulseResult = testWrapPhase(prevInc, phase);
        if (impulseResult > 0.5f) {
            int randLow = xxn - jitterWidth;
            int randHigh = xxn + jitterWidth;
            if (randLow < 0) {
                randLow = 0;
            }
            if (randHigh >= inNumSamples) {
                randHigh = inNumSamples - 1;
            }
            int idx = rgen.irand(randHigh - randLow) + randLow;
            out[idx] = 1.f;
        }
        if (phOffChanged) {
            phase += phaseSlope;
            testWrapPhase(prevInc, phase);
        }
        prevInc += incSlope;
        phase += prevInc;
    }

    unit->mPhase = phase;
    unit->mPhaseOffset = off;
    unit->mPhaseIncrement = inc;
}

void ImpulseJitter_next_ii(ImpulseJitter* unit, int inNumSamples) {
    float* out = OUT(0);
    float jitterFracIn = IN0(2);
    
    // Collect UGen state
    double inc = unit->mPhaseIncrement;
    double phase = unit->mPhase;

    // Compute the jitter width
    int jitterMaxWidth = inNumSamples + unit->mTableMaxSize;
    int jitterWidth = static_cast<int>(jitterFracIn * jitterMaxWidth);
    if (jitterWidth < 0) {
        jitterWidth = 0;
    } else if (jitterWidth > jitterMaxWidth) {
        jitterWidth = jitterMaxWidth;
    }
    int jitterLow = 0;
    //printf("Jitter width: %d\n", jitterWidth);

    // Zero out the output buffer
    for (int xxn = 0; xxn < inNumSamples; xxn++) {
        out[xxn] = 0.f;
    }

    // Update the future buffer and pull impulses from it as necessary
    for (int xxn = 0; xxn < unit->mTableSize; xxn++) {
        unit->mImpulseTable[xxn] -= inNumSamples;
        if (unit->mImpulseTable[xxn] < inNumSamples) {
            out[unit->mImpulseTable[xxn]] = 1.f;
            unit->mImpulseTable[xxn] = unit->mImpulseTable[unit->mTableSize - 1];
            unit->mTableSize--;
        }
    }

    RGET
    for (int xxn = 0; xxn < inNumSamples; xxn++) {
        float impulseResult = testWrapPhase(inc, phase);
        if (impulseResult > 0.5f) {
            int newIdx = rgen.irand(jitterWidth - 1) + jitterLow;
            if (newIdx >= inNumSamples) {
                unit->mImpulseTable[unit->mTableSize] = newIdx;
                unit->mTableSize++;
            } else {
                out[newIdx] = 1.f;
            }
        }
        if (jitterLow < xxn / 2 && jitterLow + jitterWidth < jitterMaxWidth - 2) {
            jitterLow++;
        }
        phase += inc;
    }

    unit->mPhase = phase;
}

void ImpulseJitter_next_ik(ImpulseJitter* unit, int inNumSamples) {
    float* out = OUT(0);
    double off = IN0(1);
    float jitterFracIn = IN0(2);

    // Collect UGen state
    double phase = unit->mPhase;
    double inc = unit->mPhaseIncrement;
    double prevOff = unit->mPhaseOffset;

    int jitterWidth = static_cast<int>(jitterFracIn * inNumSamples);
    double phaseSlope = CALCSLOPE(off, prevOff);
    bool phOffChanged = phaseSlope != 0.f;

    RGET
    for (int xxn = 0; xxn < inNumSamples; xxn++) {
        float impulseResult = testWrapPhase(inc, phase);
        if (impulseResult > 0.5f) {
            int randLow = xxn - jitterWidth;
            int randHigh = xxn + jitterWidth;
            if (randLow < 0) {
                randLow = 0;
            }
            if (randHigh >= inNumSamples) {
                randHigh = inNumSamples - 1;
            }
            int idx = rgen.irand(randHigh - randLow) + randLow;
            out[idx] = 1.f;
        }
        if (phOffChanged) {
            phase += phaseSlope;
            testWrapPhase(inc, phase);
        }
        phase += inc;
    }

    unit->mPhase = phase;
    unit->mPhaseOffset = off;
}

// Construct the ImpulseJitter
void ImpulseJitter_Ctor(ImpulseJitter* unit) {
    unit->mPhaseIncrement = IN0(0) * unit->mFreqMul;
    unit->mPhaseOffset = IN0(1);
    unit->mFreqMul = static_cast<float>(unit->mRate->mSampleDur);
    unit->mTableMaxSize = 2048;  // hard coded for now
    unit->mTableSize = 0;
    unit->mImpulseTable = (int*)RTAlloc(unit->mWorld, unit->mTableMaxSize * sizeof(int));

    double initOff = unit->mPhaseOffset;
    double initInc = unit->mPhaseIncrement;
    double initPhase = sc_wrap(initOff, 0.0, 1.0);

    // Initial phase offset of 0 means output of 1 on first sample.
    // Set phase to wrap point to trigger impulse on first sample
    if (initPhase == 0.0 && initInc >= 0.0) {
        initPhase = 1.0; // positive frequency trigger/wrap position
    }
    unit->mPhase = initPhase;

    UnitCalcFunc func;
    switch (INRATE(0)) {
    case calc_FullRate:
        switch (INRATE(1)) {
        case calc_ScalarRate:
            func = (UnitCalcFunc)ImpulseJitter_next_ai;
            //printf("Calc function set to ai\n");
            break;
        case calc_BufRate:
            func = (UnitCalcFunc)ImpulseJitter_next_ak;
            //printf("Calc function set to ak\n");
            break;
        case calc_FullRate:
            func = (UnitCalcFunc)ImpulseJitter_next_aa;
            //printf("Calc function set to aa\n");
            break;
        }
        break;
    case calc_BufRate:
        if (INRATE(1) == calc_ScalarRate) {
            func = (UnitCalcFunc)ImpulseJitter_next_ki;
            //printf("Calc function set to ki\n");
        } else {
            func = (UnitCalcFunc)ImpulseJitter_next_kk;
            //printf("Calc function set to kk\n");
        }
        break;
    case calc_ScalarRate:
        if (INRATE(1) == calc_ScalarRate) {
            func = (UnitCalcFunc)ImpulseJitter_next_ki;
            //printf("Calc function set to ki\n");
        } else {
            func = (UnitCalcFunc)ImpulseJitter_next_kk;
            //printf("Calc function set to kk\n");
        }
        break;
    }
    unit->mCalcFunc = func;
    func(unit, 1);

    unit->mPhase = initPhase;
    unit->mPhaseOffset = initOff;
    unit->mPhaseIncrement = initInc;
}

void ImpulseJitter_Dtor(ImpulseJitter* unit) {
    RTFree(unit->mWorld, unit->mImpulseTable);
}

PluginLoad(ImpulseJitter) {
    ft = inTable;
    DefineDtorUnit(ImpulseJitter);
}