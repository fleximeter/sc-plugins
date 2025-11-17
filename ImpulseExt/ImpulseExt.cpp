/*
File: ImpulseExt.cpp
Author: Jeff Martin

Description:
This is a collection of modified Impulse UGens.

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

static InterfaceTable *ft;

static float Impulse_testWrapPhase(double prev_inc, double& phase);

// Represents an ImpulseDropout UGen.
struct ImpulseDropout : public Unit {
    double mPhase, mPhaseOffset, mPhaseIncrement;
    float mFreqMul;
};

static void ImpulseDropout_next_aa(ImpulseDropout* unit, int inNumSamples);

// Construct the ImpulseDropout
void ImpulseDropout_Ctor(ImpulseDropout* unit) {
    unit->mPhaseOffset = IN0(2);
    unit->mFreqMul = static_cast<float>(unit->mRate->mSampleDur);
    unit->mPhaseIncrement = IN0(0) * unit->mFreqMul;

    double initOff = unit->mPhaseOffset;
    double initInc = unit->mPhaseIncrement;
    double initPhase = sc_wrap(initOff, 0.0, 1.0);

    // Initial phase offset of 0 means output of 1 on first sample.
    // Set phase to wrap point to trigger impulse on first sample
    if (initPhase == 0.0 && initInc >= 0.0) {
        initPhase = 1.0; // positive frequency trigger/wrap position
    }
    unit->mPhase = initPhase;

    // Set the calculation function
    switch (INRATE(0)) {
        case calc_ScalarRate:
        case calc_BufRate:
        case calc_FullRate:
            switch (INRATE(1)) {
                case calc_ScalarRate:
                case calc_BufRate:
                case calc_FullRate:
                    SETCALC(ImpulseDropout_next_aa);
            }
    }

    // Initialize the first sample output
    unit->mCalcFunc(unit, 1);

    unit->mPhase = initPhase;
    unit->mPhaseOffset = initOff;
    unit->mPhaseIncrement = initInc;
}

// Calculates samples for an ImpulseDropout.ar UGen with .ar parameters
void ImpulseDropout_next_aa(ImpulseDropout* unit, int inNumSamples) {
    float* out = OUT(0);
    
    // Collect parameters
    float* freqin = IN(0);
    float* dropFracIn = IN(1);
    float* offIn = IN(2);

    // Collect UGen state
    float freqmul = unit->mFreqMul;
    double phase = unit->mPhase;
    double inc = unit->mPhaseIncrement;
    float prev_off = static_cast<float>(unit->mPhaseOffset);

    // REPLACE THIS IN PRODUCTION CODE
    // It will crash if the block size is too big. Need to use RTAlloc.
    // size_t ones[128];
    // size_t ones_len = 0;
    // size_t ones_idx = -1;
    
    // Compute provisional output block
    for (size_t xxn = 0; xxn < static_cast<size_t>(inNumSamples); xxn++) {
        float z = Impulse_testWrapPhase(inc, phase);
        // if (z > 0.f) {
        //     ones[++ones_idx] = xxn;
        // }
        float off = offIn[xxn];
        float offInc = off - prev_off;
        phase += offInc;
        Impulse_testWrapPhase(inc, phase);
        inc = freqin[xxn] * freqmul;
        phase += inc;
        prev_off = off;
        out[xxn] = z;
    }

    // ones_len = ones_idx;

    // Perform dropout
    // size_t num_dropout = static_cast<size_t>(sc_round(*dropFracIn * inNumSamples, 1.f));
    // RGET
    // while (num_dropout > 0) {
    //     size_t idx = static_cast<size_t>(sc_round(frand(s1, s2, s3) * ones_len, 1.f));
    //     out[ones[idx]] = 0.f;
    //     ones[idx] = ones[ones_len];
    //     ones_len--;
    //     num_dropout--;
    // }
    
    // update the state of the ImpulseDropout at the end of the calculation block
    unit->mPhase = phase;
    unit->mPhaseOffset = prev_off;
    unit->mPhaseIncrement = inc;
}

PluginLoad(ImpulseDropout) {
    ft = inTable;
    DefineSimpleUnit(ImpulseDropout);
}

// This is a copy of the static function from LFUGens.cpp in server/plugins.
// It detects if a phasor is out-of-bounds, triggers, and wraps [0, 1].
static inline float Impulse_testWrapPhase(double prev_inc, double& phase) {
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