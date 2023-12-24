#ifndef SINEWAVE_H
#define SINEWAVE_H

#include "Stk.h"

using namespace stk;

class SineWave {
    friend class SynthNote;

public:
    SineWave() : frequency_(440.0), phase_(0.0) {}
    SineWave(float frequency) : frequency_(frequency), phase_(0.0) {}
    SineWave(StkFloat frequency) : frequency_(frequency), phase_(0.0) {}
    SineWave(float frequency, float phase) : frequency_(frequency), phase_(phase) {}
    SineWave(StkFloat frequency, StkFloat phase) : frequency_(frequency), phase_(phase) {}

    void setFrequency(StkFloat frequency) {
        frequency_ = frequency;
    }

    StkFloat tick() {
        // Éú³ÉÕýÏÒ²¨
        StkFloat sample = sin(2 * PI * frequency_ * phase_);
        phase_ += 1.0 / Stk::sampleRate();
        if (phase_ > 1.0) phase_ -= 1.0;
        return sample;
    }

private:
    StkFloat frequency_;
    StkFloat phase_;
};

#endif // SINEWAVE_H
