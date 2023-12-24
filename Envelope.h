#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "Stk.h"
#include "loadConfig.h"

using namespace stk;

class Envelope {
    friend class SynthNote;
    friend class Synth;
    friend class SynthThread;

public:
    Envelope() : state_(Done), value_(0.0) {
        attack_ = LoadConfig::ENVELOPE_PARAMETERS[0];
        decay_ = LoadConfig::ENVELOPE_PARAMETERS[1];
        sustain_ = LoadConfig::ENVELOPE_PARAMETERS[2];
        release_ = LoadConfig::ENVELOPE_PARAMETERS[3];

        // 起音速度
        // 使用反比函数，将 0-127 映射至 0 - ∞ ：
        attack_v = 512.0 / (128.0 - attack_);
        // 衰减比率
        decay_r = decay_ / 128.0;
        // 延音衰减速度
        sustain_v = sustain_ / 256.0;
        // 释音速度
        release_v = release_ / 64.0;
    }
    
    ~Envelope() {}

    StkFloat tick() {
        switch (state_) {
        case Attack:
            // 起音状态
            // std::cout << "[INFO] value: " << value_ << "\n";
            value_ += attack_v / (double)LoadConfig::SAMPLE_RATE;
            if (value_ >= 1.0) {
				value_ = 1.0;
				state_ = Decay;
                // std::cout << "[INFO] Envelope: A -> D" << std::endl;
			}
            break;
        case Decay:
            // 衰减状态
            value_ -= 2 / (double)LoadConfig::SAMPLE_RATE;
            if (value_ <= decay_r) {
                value_ = decay_r;
                state_ = Sustain;
                // std::cout << "[INFO] Envelope: D -> S" << std::endl;
            }
            break;
        case Sustain:
            // 延音状态
            value_ -= sustain_v / (double)LoadConfig::SAMPLE_RATE;
            if (value_ <= 0.0) {
				value_ = 0.0;
				state_ = Release;
				// std::cout << "[INFO] Envelope: S -> R" << std::endl;
			}
            break;
        case Release:
            // 释音状态
            value_ -= release_v / (double)LoadConfig::SAMPLE_RATE;
            if (value_ <= 0.0) {
                value_ = 0.0;
                state_ = Done;
            }
            break;
        case Done:
            break;
        }
        return value_;
    }

    void noteOn() {
        state_ = Attack;
        value_ = 0.0;
    }

    void noteOff() {
        state_ = Release;
    }

private:
    enum State { Attack, Decay, Sustain, Release, Done };
    State state_;           // 包络状态

    StkFloat attack_;       // 起音时间，[0, 1]
    StkFloat decay_;        // 衰减时间，[0, 1]
    StkFloat sustain_;      // 持续时间，[0, 1]
    StkFloat release_;      // 释音时间，[0, 1]
    StkFloat value_;        // 包络值，代表音量

    float attack_v;         // 起音速度，单位为 Ratio/s
    float decay_r;          // 衰减比率，单位为 Ratio
    float sustain_v;        // 延音衰减速度，单位为 Ratio/s
    float release_v;        // 释音速度，单位为 Ratio/s

};

#endif // ENVELOPE_H
