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
            // ����״̬
            value_ += 1.0 / (attack_ * Stk::sampleRate());
            if (value_ >= 1.0) {
                value_ = 1.0;
                state_ = Decay;
            }
            break;
        case Decay:
            // ˥��״̬
            value_ -= (1.0 - sustain_) / (decay_ * Stk::sampleRate());
            if (value_ <= sustain_) {
                value_ = sustain_;
                state_ = Sustain;
            }
            break;
        case Sustain:
            // ����״̬
            break;
        case Release:
            // ����״̬
            value_ -= sustain_ / (release_ * Stk::sampleRate());
            if (value_ <= 0.0) {
                value_ = 0.0;
                state_ = Done;
            }
            break;
        case Done:
            // �������
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
    State state_;           // ����״̬
    StkFloat attack_;       // ����ʱ�䣬[0, 1]
    StkFloat decay_;        // ˥��ʱ�䣬[0, 1]
    StkFloat sustain_;      // ����ʱ�䣬[0, 1]
    StkFloat release_;      // ����ʱ�䣬[0, 1]
    StkFloat value_;        // ����ֵ����������

};

#endif // ENVELOPE_H
