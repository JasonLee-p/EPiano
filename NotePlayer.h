#ifndef NOTEPLAYER_H
#define NOTEPLAYER_H

#include <string>
#include <QMessageBox>
#include <thread>
#include <mutex>
#include <queue>
#include <QMutex>
#include "RtAudio.h"
#include "SineWave.h"
#include "Envelope.h"
#include "loadConfig.h"
#include "music.h"
#include "threadPool.h"


class SynthNote {
public:
    static std::map<int, SynthNote*> noteMap;                               // ��̬����SynthNotes

    bool passToNew = false;                                                 // �Ƿ񴫵ݸ��µ�SynthNote
    QMutex mutex;                                                           // ������
    Music::Note note;											            // ����
    Envelope* envelope;
    std::array<SineWave*, 13> synthWaves = std::array<SineWave*, 13>();
    int velocity;
    float sample;                                                           // ����ֵ

    static void initAllSynthWaves() {
        for (int i = 21; i <= 108; i++) {
            Music::Note note = Music::Note(i);
			SynthNote* synthNote = new SynthNote(&note, 127);
			synthNote->envelope = new Envelope();
			synthNote->initSynthWaves();
			noteMap[i-21] = synthNote;
		}
	}

    void initSynthWaves() {
        for (int i = 0; i < 13; i++) {
            // �����ʼ��λ
			synthWaves[i] = new SineWave(note.frequency * (i + 1), rand() / (double)RAND_MAX);
		}
	}


    /*
        �ϳɲ�������������г��ֵ��
    */
    SynthNote(Music::Note* note, const int velocity) : velocity(velocity) {
        this->note = *note;
        envelope = new Envelope();
        initSynthWaves();
        noteMap[note->midiNumber] = this;
    }
    ~SynthNote() {
        if (!this->passToNew) {
            try {
                delete envelope;
                for (int i = 0; i < 13; i++) {
					delete synthWaves[i];
				}
            }
            catch (std::exception& e) {
				std::cout << "[ERROR] " << e.what() << std::endl;
			}
		}
	}
    // ����hash
    struct hash {
        size_t operator()(const SynthNote* synthNote) const {
			return std::hash<int>()(synthNote->note.midiNumber);
		}
	};

    // ��Сг�������������ȣ����������ɫ
    static float adjustAmplitude(int index) {
        return 3.0 / (index + 3.0);
    }

    void getSample(float* sample, double phase = NULL) {
		// �ϳɲ�
        float add = 0;
        for (int i = 0; i < 13; i++) {
			add += synthWaves[i]->tick(phase) * LoadConfig::HARMONIC_INTENSITIES[i] * adjustAmplitude(i) / 128.0;
		}
		// ����
        *sample += add * envelope->tick() / 128.0;
	}

    void getSinSample(float* sample) {
        // �ϳɲ�
		float add = 0;
        for (int i = 0; i < 13; i++) {
            add += synthWaves[i]->tick() * LoadConfig::HARMONIC_INTENSITIES[i] * adjustAmplitude(i) / 128.0;
        }
        *sample += add;
    }

    void getEnvelopeSample(float* sample) {
		// ����
		*sample = velocity * envelope->tick() / 128.0;
	}

    void noteOn(int velocity) {
        this->velocity = velocity;
		envelope->noteOn();
	}

    void noteOff() {
		envelope->noteOff();
	}
};


class Synth {
public:

    std::mutex synthMutex;

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

    // �̹߳���
    static ThreadPool *threadPool;
    void initThreads();
    void stopThreads();
    RtAudio audioOutput;
};

#endif // !NOTEPLAYER_H