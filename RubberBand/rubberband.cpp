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

#include "SC_InterfaceTable.h"
#include "FFT_UGens.h"
#include "SC_Unit.h"
#include <iostream>
#include "rubberband/RubberBandLiveShifter.h"
#include "ringbuffer.hpp"

InterfaceTable *ft;

struct RubberBandPS : public Unit {
    RubberBand::RubberBandLiveShifter* m_shifter;
    RingBuffer<float>* m_sendBuffer;
    RingBuffer<float>* m_receiveBuffer;
};

static void RubberBandPS_next(RubberBandPS *unit, int inNumSamples) {
    
}

static void RubberBandPS_Ctor(RubberBandPS *unit) {
    float pitchRatio = IN0(1);
    float formantRatio = IN0(2);
    // 0x01000000  // formant preserving
    // 0x00000000  // no formant preservation
    unit->m_shifter = (RubberBand::RubberBandLiveShifter*)RTAlloc(unit->mWorld, sizeof(RubberBand::RubberBandLiveShifter));
    new (unit->m_shifter) RubberBand::RubberBandLiveShifter(SAMPLERATE, 1, 0x01000000);
    unit->m_sendBuffer = (RingBuffer<float>*)RTAlloc(unit->mWorld, sizeof(RingBuffer<float>));
    new (unit->m_sendBuffer) RingBuffer<float>(BUFLENGTH, unit->m_shifter->getBlockSize(), 5);
    unit->m_receiveBuffer = (RingBuffer<float>*)RTAlloc(unit->mWorld, sizeof(RingBuffer<float>));
    new (unit->m_sendBuffer) RingBuffer<float>(unit->m_shifter->getBlockSize(), BUFLENGTH, 5);
    SETCALC(RubberBandPS_next);
    unit->m_shifter->setPitchScale(pitchRatio);
    std::cout << "RubberBand info:" << std::endl;
    std::cout << "Pitch Scale: " << unit->m_shifter->getPitchScale() << std::endl;
    std::cout << "Formant Scale: " << unit->m_shifter->getFormantScale() << std::endl;
    std::cout << "Start Delay: " << unit->m_shifter->getStartDelay() << std::endl;
    std::cout << "Channels: " << unit->m_shifter->getChannelCount() << std::endl;
    std::cout << "Block Size: " << unit->m_shifter->getBlockSize() << std::endl;
}

static void RubberBandPS_Dtor(RubberBandPS *unit) {
    RTFree(unit->mWorld, unit->m_shifter);
    RTFree(unit->mWorld, unit->m_sendBuffer);
    RTFree(unit->mWorld, unit->m_receiveBuffer);
}

PluginLoad(PV_Jeff) {
    ft = inTable;
    // DefineSimpleUnit(PV_MagMirror);
    DefineDtorUnit(RubberBandPS);
}
