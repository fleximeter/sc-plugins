/*
File: pv.cpp
Author: Jeff Martin

Description:


Copyright © 2026 by Jeffrey Martin. All rights reserved.
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

#include "SC_Constants.h"
#include "SC_InterfaceTable.h"
#include "FFT_UGens.h"
#include "SC_Unit.h"

InterfaceTable *ft;

struct PV_CFreeze : public Unit {
    int mNumBins;    // The number of FFT bins
    int mNumFrames;  // The number of candidate FFT frames to maintain
    float *mMags;    // The 2D array of FFT mags
    float *mDc;      // The 1D array of FFT DC values
    float *mNyq;     // The 1D array of FFT Nyquist values
    float *mPhase;   // The most recent phase array
    float *mPhaseDiffs;  // The array of FFT phase differences
    size_t mWritePtr;   // The write pointer
};

static void PV_CFreeze_next(PV_CFreeze *unit, int inNumSamples) {
    PV_GET_BUF
    float freezeState = IN0(1);
    // allocate the buffers
    if (!unit->mMags) {
        // MxN where N is num bins, and M is num frames
        unit->mMags = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float) * unit->mNumFrames);
        // M (num frames)
        unit->mDc = (float*)RTAlloc(unit->mWorld, sizeof(float) * unit->mNumFrames);
        // M (num frames)
        unit->mNyq = (float*)RTAlloc(unit->mWorld, sizeof(float) * unit->mNumFrames);
        // N (num bins)
        unit->mPhase = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));
        // N (num bins)
        unit->mPhaseDiffs = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));
        ClearFFTUnitIfMemFailed(unit->mMags);
        ClearFFTUnitIfMemFailed(unit->mDc);
        ClearFFTUnitIfMemFailed(unit->mNyq);
        ClearFFTUnitIfMemFailed(unit->mPhase);
        ClearFFTUnitIfMemFailed(unit->mPhaseDiffs);
        unit->mNumBins = numbins;
        unit->mWritePtr = 0;
    } else if (numbins != unit->mNumBins) {
        return;
    }

    SCPolarBuf *p = ToPolarApx(buf);

    if (freezeState > 0.f) {
        RGET
        int idx = rgen.irand(unit->mNumFrames);
        float *mags = unit->mMags + (idx * unit->mNumBins);
        p->dc = unit->mDc[idx];
        p->nyq = unit->mNyq[idx];
        for (int i = 0; i < unit->mNumBins; i++) {
            p->bin[i].mag = mags[i];
            unit->mPhase[i] = sc_wrap(unit->mPhase[i] + unit->mPhaseDiffs[i], 0.f, static_cast<float>(twopi));
            p->bin[i].phase = unit->mPhase[i];
        }
    } else {
        float *currentMagArr = unit->mMags + (unit->mWritePtr * unit->mNumBins);
        for (int i = 0; i < numbins; i++) {
            currentMagArr[i] = p->bin[i].mag;
            unit->mPhaseDiffs[i] = sc_wrap(p->bin[i].phase - unit->mPhase[i], 0.f, static_cast<float>(twopi));
            unit->mPhase[i] = p->bin[i].phase;
        }
        unit->mDc[unit->mWritePtr] = p->dc;
        unit->mNyq[unit->mWritePtr] = p->nyq;
        unit->mWritePtr++;
        unit->mWritePtr %= unit->mNumFrames;
    }
}

static void PV_CFreeze_Ctor(PV_CFreeze *unit) {
    SETCALC(PV_CFreeze_next);
    OUT0(0) = IN0(0);
    unit->mMags = nullptr;
    unit->mDc = nullptr;
    unit->mNyq = nullptr;
    unit->mPhase = nullptr;
    unit->mPhaseDiffs = nullptr;
    int numFrames = IN0(2);
    // prevent the user from doing something nuts
    unit->mNumFrames = sc_wrap(numFrames, 1, 20);
}

static void PV_CFreeze_Dtor(PV_CFreeze *unit) {
    RTFree(unit->mWorld, unit->mMags);
    RTFree(unit->mWorld, unit->mDc);
    RTFree(unit->mWorld, unit->mNyq);
    RTFree(unit->mWorld, unit->mPhase);
    RTFree(unit->mWorld, unit->mPhaseDiffs);
}

PluginLoad(PV_CFreeze) {
    ft = inTable;
    DefineDtorUnit(PV_CFreeze);
}
