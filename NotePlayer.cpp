#include "NotePlayer.h"


std::map<int, SynthNote*> SynthNote::noteMap;
ThreadPool* Synth::threadPool = nullptr;


Synth::Synth() {
    // 设置采样率
    initAudioDevice();
    initThreads();
}

Synth::~Synth() {
    stopThreads();
    stopAudioStream();
    audioOutput.closeStream();
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
		std::cout << "[INFO] successfully opened RtAudio stream." << std::endl;
    }
    else {
        std::cout << "[ERROR] Failed to open RtAudio stream." << std::endl;
        QMessageBox::critical(nullptr, "Error", "Failed to open RtAudio stream.");
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
	SynthNote::noteMap[note.midiNumber - 21]->noteOn(velocity);
}

void Synth::noteOff(Music::Note& note, const int velocity) {
	SynthNote::noteMap[note.midiNumber - 21]->noteOff();
}

int Synth::audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
    double streamTime, RtAudioStreamStatus status, void* userData) {
    float* out = static_cast<float*>(outputBuffer);         // 输出缓冲区
    Synth* synth = static_cast<Synth*>(userData);           // 当前Synth对象

    if (1) {
        // 分线程之前的回调函数实现
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
    else { // 1.给线程池分配任务：线程1计算sin波，线程2计算包络
        auto sinResult = threadPool->enqueue([&out, nBufferFrames, synth] {
            // 计算sin波
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
            // 计算包络
            for (unsigned int i = 0; i < nBufferFrames; i++) {
                float sample = 0;
                for (int i = 0; i < 88; i++) {
                    if (SynthNote::noteMap[i]->envelope->state_ != Envelope::Done)
                        SynthNote::noteMap[i]->getEnvelopeSample(&sample);
                }
                // 等待上一个线程计算完毕
                sinResult.wait();
                for (int i = 0; i < LoadConfig::CHANNELS; i++) {
                    *out++ *= sample;
                }
            }
            });
        // 等待线程池中的任务完成
        envelopeResult.wait();
    }
    
    return 0;
}

void Synth::initThreads() {
    // 初始化线程池
    threadPool = new ThreadPool(2);
}

void Synth::stopThreads() {
	// 释放线程池
	delete threadPool;
}