#include "NotePlayer.h"


std::map<int, SynthNote*> SynthNote::noteMap;
ThreadPool* Synth::threadPool = nullptr;


Synth::Synth() {
    // ���ò�����
    initAudioDevice();
    initThreads();
}

Synth::~Synth() {
    stopThreads();
    stopAudioStream();
    audioOutput.closeStream();
}

void Synth::initAudioDevice() {
    // ��ʼ��RtAudio
    RtAudio::StreamParameters parameters;
    parameters.deviceId = audioOutput.getDefaultOutputDevice();
    parameters.nChannels = LoadConfig::CHANNELS;
    parameters.firstChannel = 0;

    RtAudio::StreamOptions options;
    options.flags = RTAUDIO_SCHEDULE_REALTIME; // ����Ϊʵʱ��

    unsigned int bufferFrames = LoadConfig::BUFFER_SIZE;

    // ��RtAudio��
    audioOutput.openStream(&parameters, nullptr, RTAUDIO_FLOAT32, static_cast<unsigned int>(LoadConfig::SAMPLE_RATE),
        &bufferFrames, &audioCallback, this, &options);
    // �ж��Ƿ�򿪳ɹ�
    if (audioOutput.isStreamOpen()) {
		std::cout << "[INFO] successfully opened RtAudio stream." << std::endl;
    }
    else {
        std::cout << "[ERROR] Failed to open RtAudio stream." << std::endl;
        QMessageBox::critical(nullptr, "Error", "Failed to open RtAudio stream.");
    }
    // ��ʼRtAudio��
    startAudioStream();
}

void Synth::startAudioStream() {
    // ��ʼRtAudio��
    audioOutput.startStream();
}

void Synth::stopAudioStream() {
    // ֹͣRtAudio��
    audioOutput.stopStream();
}

void Synth::noteOn(Music::Note& note, const int velocity) {
	SynthNote::noteMap[note.midiNumber - 21]->noteOn(velocity);
}

void Synth::noteOff(Music::Note& note, const int velocity) {
	SynthNote::noteMap[note.midiNumber - 21]->noteOff();
}

int Synth::audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
    double streamTime, RtAudioStreamStatus status, void* userData) {
    float* out = static_cast<float*>(outputBuffer);         // ���������
    Synth* synth = static_cast<Synth*>(userData);           // ��ǰSynth����

    if (1) {
        // ���߳�֮ǰ�Ļص�����ʵ��
        for (unsigned int i = 0; i < nBufferFrames; i++) {
            float sample = 0;
            for (int i = 0; i < 88; i++) {
                if (SynthNote::noteMap[i]->envelope->state_ != Envelope::Done)
                    SynthNote::noteMap[i]->getSample(&sample);
            }
            for (int i = 0; i < LoadConfig::CHANNELS; i++) {
                *out++ = sample;
            }
        }
    }
    else { // 1.���̳߳ط��������߳�1����sin�����߳�2�������
        auto sinResult = threadPool->enqueue([&out, nBufferFrames, synth] {
            // ����sin��
            for (unsigned int i = 0; i < nBufferFrames; i++) {
                float sample = 0;
                for (int i = 0; i < 88; i++) {
                    if (SynthNote::noteMap[i]->envelope->state_ != Envelope::Done)
                        SynthNote::noteMap[i]->getSinSample(&sample);
                }
                for (int i = 0; i < LoadConfig::CHANNELS; i++) {
                    *out++ = sample;
                }
            }
            });
        auto envelopeResult = threadPool->enqueue([&out, nBufferFrames, synth, &sinResult] {
            // �������
            for (unsigned int i = 0; i < nBufferFrames; i++) {
                float sample = 0;
                for (int i = 0; i < 88; i++) {
                    if (SynthNote::noteMap[i]->envelope->state_ != Envelope::Done)
                        SynthNote::noteMap[i]->getEnvelopeSample(&sample);
                }
                // �ȴ���һ���̼߳������
                sinResult.wait();
                for (int i = 0; i < LoadConfig::CHANNELS; i++) {
                    *out++ *= sample;
                }
            }
            });
        // �ȴ��̳߳��е��������
        envelopeResult.wait();
    }
    
    return 0;
}

void Synth::initThreads() {
    // ��ʼ���̳߳�
    threadPool = new ThreadPool(2);
}

void Synth::stopThreads() {
	// �ͷ��̳߳�
	delete threadPool;
}