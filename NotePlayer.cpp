#include "NotePlayer.h"


std::map<int, SynthNote*> SynthNote::noteMap;


Synth::Synth() {
    // 设置采样率
    initAudioDevice();
}

Synth::~Synth() {
    stopAudioStream();
}

void Synth::initAudioDevice() {
    // 初始化RtAudio
    RtAudio::StreamParameters parameters;
    parameters.deviceId = audioOutput.getDefaultOutputDevice();
    parameters.nChannels = LoadConfig::CHANNELS;
    parameters.firstChannel = 0;

    RtAudio::StreamOptions options;
    options.flags = RTAUDIO_SCHEDULE_REALTIME; // 设置为实时流

    unsigned int bufferFrames = LoadConfig::BUFFER_SIZE;

    // 打开RtAudio流
    audioOutput.openStream(&parameters, nullptr, RTAUDIO_FLOAT32, static_cast<unsigned int>(LoadConfig::SAMPLE_RATE),
        &bufferFrames, &audioCallback, this, &options);
    // 判断是否打开成功
    if (audioOutput.isStreamOpen()) {
		std::cout << "[INFO] Successfully opened RtAudio stream." << std::endl;
        }
	else {
        std::cout << "[ERROR] Failed to open RtAudio stream." << std::endl;
	}
    // 开始RtAudio流
    startAudioStream();
}

void Synth::startAudioStream() {
    // 开始RtAudio流
    audioOutput.startStream();
}

void Synth::stopAudioStream() {
    // 停止RtAudio流
    audioOutput.stopStream();
}

void Synth::noteOn(Music::Note& note, const int velocity) {
    // 根据音符信息生成SynthNote
	SynthNote* synthNote = new SynthNote(&note, velocity);
	// 调用SynthNote的noteOn
	synthNote->noteOn();
}

void Synth::noteOff(Music::Note& note, const int velocity) {
    // 根据音符信息生成SynthNote
	SynthNote* synthNote = new SynthNote(&note, velocity);
	// 调用SynthNote的noteOff
	synthNote->noteOff();
}

int Synth::audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
    double streamTime, RtAudioStreamStatus status, void* userData) {
    // 回调函数，遍历所有SynthNote，调用getSample()，将结果写入outputBuffer
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