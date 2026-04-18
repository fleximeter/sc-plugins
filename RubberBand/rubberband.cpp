/*
File: rubberband.cpp
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
#include "rubberband/RubberBandLiveShifter.h"

InterfaceTable *ft;

struct RubberBandPS : public Unit {
    RubberBand::RubberBandLiveShifter *shifter;
};

static void RubberBandPS_next(RubberBandPS *unit, int inNumSamples) {
    
}

static void RubberBandPS_Ctor(RubberBandPS *unit) {
    

    // 0x01000000  // formant preserving
    // 0x00000000  // no formant preservation
    // use sc malloc unit->shifter = RubberBand::RubberBandLiveShifter(SAMPLERATE, 1, 0x01000000);
    SETCALC(RubberBandPS_next);
}

static void RubberBandPS_Dtor(RubberBandPS *unit) {

}

PluginLoad(PV_Jeff) {
    ft = inTable;
    // DefineSimpleUnit(PV_MagMirror);
    DefineDtorUnit(RubberBandPS);
}
