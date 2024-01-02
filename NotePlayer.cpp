#include "NotePlayer.h"


std::map<int, SynthNote*> SynthNote::noteMap;


Synth::Synth() {
    // ���ò�����
    initAudioDevice();
}

Synth::~Synth() {
    stopAudioStream();
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
		std::cout << "[INFO] Successfully opened RtAudio stream." << std::endl;
        }
	else {
        std::cout << "[ERROR] Failed to open RtAudio stream." << std::endl;
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
    // ����������Ϣ����SynthNote
	SynthNote* synthNote = new SynthNote(&note, velocity);
	// ����SynthNote��noteOn
	synthNote->noteOn();
}

void Synth::noteOff(Music::Note& note, const int velocity) {
    // ����������Ϣ����SynthNote
	SynthNote* synthNote = new SynthNote(&note, velocity);
	// ����SynthNote��noteOff
	synthNote->noteOff();
}

int Synth::audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
    double streamTime, RtAudioStreamStatus status, void* userData) {
    // �ص���������������SynthNote������getSample()�������д��outputBuffer
    float* out = static_cast<float*>(outputBuffer);
    for (unsigned int i = 0; i < nBufferFrames; i++) {
		float sample = 0;
        for (auto& [key, value] : SynthNote::noteMap) {
			value->getSample(&sample, &LoadConfig::HARMONIC_INTENSITIES);
		}
		*out++ = sample;
		*out++ = sample;
	}
    return 0;
}