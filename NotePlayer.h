#ifndef NOTEPLAYER_H
#define NOTEPLAYER_H

#include <string>
#include "RtAudio.h"
#include "SineWave.h"
#include "Envelope.h"
#include "loadConfig.h"
#include "music.h"


class SynthNote {
public:
    static std::map<int, SynthNote*> noteMap;       // ��̬����SynthNote

    bool passToNew = false;                         // �Ƿ񴫵ݸ��µ�SynthNote
    Music::Note note;
    Envelope* envelope;
    SineWave* baseWave;
    std::array<SineWave*, 13>* synthWaves;
    int velocity;

    void initSynthWaves() {
		synthWaves = new std::array<SineWave*, 13>();
        for (int i = 0; i < 13; i++) {
			(*synthWaves)[i] = new SineWave(note.frequency * (i + 1));
		}
	}

    /*
        �ϳɲ�������������г��ֵ��
    */
    SynthNote(Music::Note* note, const int velocity) : velocity(velocity) {
        // ����map�У����½�
        if (noteMap.find(note->midiNumber) == noteMap.end()) {
            this->note = *note;
            envelope = new Envelope(LoadConfig::ENVELOPE_PARAMETERS[0], LoadConfig::ENVELOPE_PARAMETERS[1],
                LoadConfig::ENVELOPE_PARAMETERS[2], LoadConfig::ENVELOPE_PARAMETERS[3]);
            baseWave = new SineWave(note->frequency);
            initSynthWaves();
            noteMap[note->midiNumber] = this;
        }
        // ��map�У����ã�ɾ��ԭ����
        else {
			this->note = *note;
			envelope = noteMap[note->midiNumber]->envelope;
            baseWave = noteMap[note->midiNumber]->baseWave;
            synthWaves = noteMap[note->midiNumber]->synthWaves;
            noteMap[note->midiNumber]->passToNew = true;
            noteMap[note->midiNumber]->~SynthNote();
            noteMap[note->midiNumber] = this;
		}
    }
    ~SynthNote() {
        if (!this->passToNew) {
			delete envelope;
			delete baseWave;
            delete synthWaves;
		}
	}

    // �����Ҳ���������е���
    static float adjustAmplitude(float frequency) {
        // ʹ�����Բ�ֵ
        float lowFreq = 27.5;
        float highFreq = 4186.01;
        float lowAmp = 0.1;
        float highAmp = 0.01;
        float amp = (frequency - lowFreq) / (highFreq - lowFreq) * (highAmp - lowAmp) + lowAmp;
        return amp;
    }

    void getSample(float* sample, std::array<unsigned int, 13>* synthSamples) {
        if (envelope->state_ == Envelope::Done) return;
		// �ϳɲ�
		*sample = baseWave->tick();
        for (int i = 0; i < 13; i++) {
			*sample += (*synthWaves)[i]->tick() * (*synthSamples)[i] * adjustAmplitude((*synthWaves)[i]->frequency_ / 128);
		}
		// ����
		*sample *= envelope->tick();
	}

    void noteOn() {
		envelope->noteOn();
	}

    void noteOff() {
		envelope->noteOff();
	}
};



class Synth {
public:
    Synth();
    ~Synth();

    void initAudioDevice();
    void startAudioStream();
    void stopAudioStream();

    void noteOn(Music::Note& note, const int velocity);
    void noteOff(Music::Note& note, const int velocity);

private:
    static int audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
        double streamTime, RtAudioStreamStatus status, void* userData);

    RtAudio audioOutput;
};

#endif // !NOTEPLAYER_H