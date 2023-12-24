#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "Stk.h"
#include "loadConfig.h"

using namespace stk;

class Envelope {
    friend class SynthNote;

public:
    Envelope(StkFloat attack, StkFloat decay, StkFloat sustain, StkFloat release)
        : attack_(attack), decay_(decay), sustain_(sustain), release_(release), state_(Done), value_(0.0) {}

    Envelope() : state_(Done), value_(0.0) {
        attack_ = LoadConfig::ENVELOPE_PARAMETERS[0];
        decay_ = LoadConfig::ENVELOPE_PARAMETERS[1];
        sustain_ = LoadConfig::ENVELOPE_PARAMETERS[2];
        release_ = LoadConfig::ENVELOPE_PARAMETERS[3];
    }

    StkFloat tick() {
        switch (state_) {
        case Attack:
            // 起音状态
            value_ += 1.0 / (attack_ * Stk::sampleRate());
            if (value_ >= 1.0) {
                value_ = 1.0;
                state_ = Decay;
            }
            break;
        case Decay:
            // 衰减状态
            value_ -= (1.0 - sustain_) / (decay_ * Stk::sampleRate());
            if (value_ <= sustain_) {
                value_ = sustain_;
                state_ = Sustain;
            }
            break;
        case Sustain:
            // 延音状态
            break;
        case Release:
            // 释音状态
            value_ -= sustain_ / (release_ * Stk::sampleRate());
            if (value_ <= 0.0) {
                value_ = 0.0;
                state_ = Done;
            }
            break;
        case Done:
            // 包络结束
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

};

#endif // ENVELOPE_H
