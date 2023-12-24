#ifndef SINEWAVE_H
#define SINEWAVE_H

#include "Stk.h"
#include "loadConfig.h"

#define VOLUME 0.6

using namespace stk;

class SineWave {
    friend class SynthNote;

public:
    SineWave(StkFloat frequency, StkFloat phase) : frequency_(frequency), phase_(phase) {
        multiply_ = 2 * PI * frequency_;
    }

    // 定义hash
    struct hash {
        size_t operator()(const SineWave* sineWave) const {
			return std::hash<StkFloat>()(sineWave->frequency_);
		}
	};

    StkFloat tick(double phase = NULL) {
        // 生成正弦波
        if (phase != NULL) return sin(multiply_ * phase) * VOLUME;
        StkFloat sample = sin(multiply_ * phase_) * VOLUME;
        phase_ += 1.0 / static_cast<StkFloat>(LoadConfig::SAMPLE_RATE);
        if (phase_ > 1.0) phase_ -= 1.0;
        return sample;
    }

private:
    StkFloat multiply_ = 2 * PI * 440.0;
    StkFloat frequency_;
    StkFloat phase_;
};

#endif // SINEWAVE_H
