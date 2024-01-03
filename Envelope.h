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

        // �����ٶ�
        // ʹ�÷��Ⱥ������� 0-127 ӳ���� 0 - �� ��
        attack_v = 512.0 / (128.0 - attack_);
        // ˥������
        decay_r = decay_ / 128.0;
        // ����˥���ٶ�
        sustain_v = sustain_ / 256.0;
        // �����ٶ�
        release_v = release_ / 64.0;
    }
    
    ~Envelope() {}

    StkFloat tick() {
        switch (state_) {
        case Attack:
            // ����״̬
            // std::cout << "[INFO] value: " << value_ << "\n";
            value_ += attack_v / (double)LoadConfig::SAMPLE_RATE;
            if (value_ >= 1.0) {
				value_ = 1.0;
				state_ = Decay;
                // std::cout << "[INFO] Envelope: A -> D" << std::endl;
			}
            break;
        case Decay:
            // ˥��״̬
            value_ -= 2 / (double)LoadConfig::SAMPLE_RATE;
            if (value_ <= decay_r) {
                value_ = decay_r;
                state_ = Sustain;
                // std::cout << "[INFO] Envelope: D -> S" << std::endl;
            }
            break;
        case Sustain:
            // ����״̬
            value_ -= sustain_v / (double)LoadConfig::SAMPLE_RATE;
            if (value_ <= 0.0) {
				value_ = 0.0;
				state_ = Release;
				// std::cout << "[INFO] Envelope: S -> R" << std::endl;
			}
            break;
        case Release:
            // ����״̬
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
    State state_;           // ����״̬

    StkFloat attack_;       // ����ʱ�䣬[0, 1]
    StkFloat decay_;        // ˥��ʱ�䣬[0, 1]
    StkFloat sustain_;      // ����ʱ�䣬[0, 1]
    StkFloat release_;      // ����ʱ�䣬[0, 1]
    StkFloat value_;        // ����ֵ����������

    float attack_v;         // �����ٶȣ���λΪ Ratio/s
    float decay_r;          // ˥�����ʣ���λΪ Ratio
    float sustain_v;        // ����˥���ٶȣ���λΪ Ratio/s
    float release_v;        // �����ٶȣ���λΪ Ratio/s

};

#endif // ENVELOPE_H
