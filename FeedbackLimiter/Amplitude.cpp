#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct Amplitude : public Unit {
    float m_previn, m_clampcoef, m_relaxcoef, m_clamp_in, m_relax_in;
};

void Amplitude_next(Amplitude* unit, int inNumSamples);
void Amplitude_next_kk(Amplitude* unit, int inNumSamples);
void Amplitude_next_atok(Amplitude* unit, int inNumSamples);
void Amplitude_next_atok_kk(Amplitude* unit, int inNumSamples);
void Amplitude_Ctor(Amplitude* unit);

void Amplitude_Ctor(Amplitude* unit) {
    if (INRATE(1) != calc_ScalarRate || INRATE(2) != calc_ScalarRate) {
        if (INRATE(0) == calc_FullRate && unit->mCalcRate == calc_BufRate) {
            SETCALC(Amplitude_next_atok_kk);
        } else {
            SETCALC(Amplitude_next_kk);
        }

    } else {
        if (INRATE(0) == calc_FullRate && unit->mCalcRate == calc_BufRate) {
            SETCALC(Amplitude_next_atok);
        } else {
            SETCALC(Amplitude_next);
        }
    }

    float clamp = ZIN0(1);
    unit->m_clampcoef = clamp == 0.0 ? 0.0 : exp(log1 / (clamp * SAMPLERATE));

    float relax = ZIN0(2);
    unit->m_relaxcoef = relax == 0.0 ? 0.0 : exp(log1 / (relax * SAMPLERATE));

    unit->m_previn = std::abs(ZIN0(0));
    Amplitude_next(unit, 1);
}

void Amplitude_next(Amplitude* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);

    float relaxcoef = unit->m_relaxcoef;
    float clampcoef = unit->m_clampcoef;
    float previn = unit->m_previn;

    float val;
    LOOP1(
        inNumSamples, val = std::abs(ZXP(in)); if (val < previn) { val = val + (previn - val) * relaxcoef; } else {
            val = val + (previn - val) * clampcoef;
        } ZXP(out) = previn = val;);

    unit->m_previn = previn;
}

void Amplitude_next_atok(Amplitude* unit, int inNumSamples) {
    float* in = ZIN(0);

    float relaxcoef = unit->m_relaxcoef;
    float clampcoef = unit->m_clampcoef;
    float previn = unit->m_previn;

    float val;
    LOOP1(
        FULLBUFLENGTH, val = std::abs(ZXP(in)); if (val < previn) { val = val + (previn - val) * relaxcoef; } else {
            val = val + (previn - val) * clampcoef;
        } previn = val;);
    ZOUT0(0) = val;

    unit->m_previn = previn;
}

void Amplitude_next_kk(Amplitude* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float relaxcoef, clampcoef;

    if (ZIN0(1) != unit->m_clamp_in) {
        clampcoef = unit->m_clampcoef = exp(log1 / (ZIN0(1) * SAMPLERATE));
        unit->m_clamp_in = ZIN0(1);
    } else {
        clampcoef = unit->m_clampcoef;
    }

    if (ZIN0(2) != unit->m_relax_in) {
        relaxcoef = unit->m_relaxcoef = exp(log1 / (ZIN0(2) * SAMPLERATE));
        unit->m_relax_in = ZIN0(2);
    } else {
        relaxcoef = unit->m_relaxcoef;
    }

    float previn = unit->m_previn;

    float val;
    LOOP1(
        inNumSamples, val = std::abs(ZXP(in)); if (val < previn) { val = val + (previn - val) * relaxcoef; } else {
            val = val + (previn - val) * clampcoef;
        } ZXP(out) = previn = val;);

    unit->m_previn = previn;
}

void Amplitude_next_atok_kk(Amplitude* unit, int inNumSamples) {
    float* in = ZIN(0);
    float relaxcoef, clampcoef;

    if (ZIN0(1) != unit->m_clamp_in) {
        clampcoef = unit->m_clampcoef = exp(log1 / (ZIN0(1) * SAMPLERATE));
        unit->m_clamp_in = ZIN0(1);
    } else {
        clampcoef = unit->m_clampcoef;
    }

    if (ZIN0(2) != unit->m_relax_in) {
        relaxcoef = unit->m_relaxcoef = exp(log1 / (ZIN0(2) * SAMPLERATE));
        unit->m_relax_in = ZIN0(2);
    } else {
        relaxcoef = unit->m_relaxcoef;
    }

    float previn = unit->m_previn;

    float val;
    LOOP1(
        FULLBUFLENGTH, val = std::abs(ZXP(in)); if (val < previn) { val = val + (previn - val) * relaxcoef; } else {
            val = val + (previn - val) * clampcoef;
        } previn = val;);
    ZOUT0(0) = val;

    unit->m_previn = previn;
}

PluginLoad(Amplitude) {
    ft = inTable;
    DefineSimpleUnit(Amplitude);
}
