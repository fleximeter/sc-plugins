#include "SC_PlugIn.h"


////////// Code from SC Phasor class /////////////

struct Phasor : public Unit {
    double mLevel;
    float m_previn;
};

void Phasor_Ctor(Phasor* unit);
void Phasor_next_kk(Phasor* unit, int inNumSamples);
void Phasor_next_ak(Phasor* unit, int inNumSamples);
void Phasor_next_aa(Phasor* unit, int inNumSamples);


void Phasor_Ctor(Phasor* unit) {
    if (unit->mCalcRate == calc_FullRate) {
        if (INRATE(0) == calc_FullRate) {
            if (INRATE(1) == calc_FullRate) {
                SETCALC(Phasor_next_aa);
            } else {
                SETCALC(Phasor_next_ak);
            }
        } else {
            SETCALC(Phasor_next_kk);
        }
    } else {
        SETCALC(Phasor_next_ak);
    }

    unit->m_previn = ZIN0(0);
    ZOUT0(0) = unit->mLevel = ZIN0(2);
}

void Phasor_next_kk(Phasor* unit, int inNumSamples) {
    float* out = ZOUT(0);

    float in = ZIN0(0);
    double rate = ZIN0(1);
    double start = ZIN0(2);
    double end = ZIN0(3);
    float resetPos = ZIN0(4);

    float previn = unit->m_previn;
    double level = unit->mLevel;

    if (previn <= 0.f && in > 0.f) {
        level = resetPos;
    }
    LOOP1(inNumSamples, level = sc_wrap(level, start, end); ZXP(out) = level; level += rate;);

    unit->m_previn = in;
    unit->mLevel = level;
}

void Phasor_next_ak(Phasor* unit, int inNumSamples) {
    float* out = ZOUT(0);

    float* in = ZIN(0);
    double rate = ZIN0(1);
    double start = ZIN0(2);
    double end = ZIN0(3);
    float resetPos = ZIN0(4);

    float previn = unit->m_previn;
    double level = unit->mLevel;

    LOOP1(
        inNumSamples, float curin = ZXP(in); if (previn <= 0.f && curin > 0.f) {
            float frac = 1.f - previn / (curin - previn);
            level = resetPos + frac * rate;
        } ZXP(out) = level;
        level += rate; level = sc_wrap(level, start, end);

        previn = curin;);

    unit->m_previn = previn;
    unit->mLevel = level;
}

void Phasor_next_aa(Phasor* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* in = ZIN(0);
    float* rate = ZIN(1);
    double start = ZIN0(2);
    double end = ZIN0(3);
    float resetPos = ZIN0(4);

    float previn = unit->m_previn;
    double level = unit->mLevel;

    LOOP1(
        inNumSamples, float curin = ZXP(in); double zrate = ZXP(rate); if (previn <= 0.f && curin > 0.f) {
            float frac = 1.f - previn / (curin - previn);
            level = resetPos + frac * zrate;
        } ZXP(out) = level;
        level += zrate; level = sc_wrap(level, start, end); previn = curin;);

    unit->m_previn = previn;
    unit->mLevel = level;
}
